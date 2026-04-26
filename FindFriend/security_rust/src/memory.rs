//! 内存加密模块
//! 提供安全的内存加密和解密功能

use std::collections::HashMap;
use std::sync::RwLock;
use aes_gcm_siv::{Aes256GcmSiv, KeyInit, Nonce};
use aes_gcm_siv::aead::{Aead, Key};
use rand::RngCore;
use rand_chacha::ChaCha20Rng;
use zeroize::{Zeroize, ZeroizeOnDrop};
use thiserror::Error;

#[derive(Error, Debug)]
pub enum MemoryError {
    #[error("Key not found")]
    KeyNotFound,
    #[error("Encryption failed")]
    EncryptionFailed,
    #[error("Decryption failed")]
    DecryptionFailed,
    #[error("Invalid length")]
    InvalidLength,
}

#[derive(Zeroize, ZeroizeOnDrop)]
struct SecureKey {
    key: [u8; 32],
}

struct KeyStore {
    keys: HashMap<u32, SecureKey>,
    rng: ChaCha20Rng,
}

static mut KEY_STORE: Option<KeyStore> = None;

pub fn init_keys() -> Result<(), MemoryError> {
    unsafe {
        if KEY_STORE.is_none() {
            KEY_STORE = Some(KeyStore {
                keys: HashMap::new(),
                rng: ChaCha20Rng::from_entropy(),
            });
        }
    }
    Ok(())
}

pub fn destroy_keys() {
    unsafe {
        if let Some(mut store) = KEY_STORE.take() {
            for (_, mut key) in store.keys.drain() {
                key.zeroize();
            }
        }
    }
}

pub fn generate_key(key_id: u32, key_len: usize) -> Result<Vec<u8>, MemoryError> {
    if key_len != 32 {
        return Err(MemoryError::InvalidLength);
    }
    
    let mut key = vec![0u8; key_len];
    
    unsafe {
        if let Some(ref mut store) = KEY_STORE {
            store.rng.fill_bytes(&mut key);
            
            let mut secure_key = SecureKey { key: [0u8; 32] };
            secure_key.key.copy_from_slice(&key);
            store.keys.insert(key_id, secure_key);
        }
    }
    
    Ok(key)
}

pub fn encrypt_region(data: &mut [u8], key_id: u32) -> Result<(), MemoryError> {
    if data.len() % 16 != 0 {
        return Err(MemoryError::InvalidLength);
    }
    
    let key = unsafe {
        if let Some(ref store) = KEY_STORE {
            store.keys.get(&key_id)
        } else {
            None
        }
    };
    
    let key = key.ok_or(MemoryError::KeyNotFound)?;
    
    let cipher = Aes256GcmSiv::new(Key::<Aes256GcmSiv>::from_slice(&key.key));
    
    let mut nonce_bytes = [0u8; 12];
    unsafe {
        if let Some(ref mut store) = KEY_STORE {
            store.rng.fill_bytes(&mut nonce_bytes);
        }
    }
    
    let nonce = Nonce::from_slice(&nonce_bytes);
    
    let mut buffer = data.to_vec();
    cipher.encrypt(nonce, &mut buffer, data).map_err(|_| MemoryError::EncryptionFailed)?;
    
    data[..12].copy_from_slice(&nonce_bytes);
    data[12..].copy_from_slice(&buffer);
    
    Ok(())
}

pub fn decrypt_region(data: &mut [u8], key_id: u32) -> Result<(), MemoryError> {
    if data.len() < 12 || (data.len() - 12) % 16 != 0 {
        return Err(MemoryError::InvalidLength);
    }
    
    let key = unsafe {
        if let Some(ref store) = KEY_STORE {
            store.keys.get(&key_id)
        } else {
            None
        }
    };
    
    let key = key.ok_or(MemoryError::KeyNotFound)?;
    
    let cipher = Aes256GcmSiv::new(Key::<Aes256GcmSiv>::from_slice(&key.key));
    
    let nonce = Nonce::from_slice(&data[..12]);
    let ciphertext = &data[12..];
    
    let mut buffer = ciphertext.to_vec();
    cipher.decrypt(nonce, &mut buffer).map_err(|_| MemoryError::DecryptionFailed)?;
    
    data[..buffer.len()].copy_from_slice(&buffer);
    data[buffer.len()..].zeroize();
    
    Ok(())
}

pub fn secure_alloc(size: usize) -> *mut std::ffi::c_void {
    use std::alloc::{alloc, Layout};
    
    let layout = Layout::from_size_align(size, 16).unwrap();
    let ptr = unsafe { alloc(layout) };
    
    if !ptr.is_null() {
        unsafe {
            let slice = std::slice::from_raw_parts_mut(ptr as *mut u8, size);
            if let Some(ref mut store) = KEY_STORE {
                store.rng.fill_bytes(slice);
            }
        }
    }
    
    ptr as *mut std::ffi::c_void
}

pub fn secure_free(ptr: *mut std::ffi::c_void, size: usize) {
    use std::alloc::{dealloc, Layout};
    
    if ptr.is_null() {
        return;
    }
    
    unsafe {
        let slice = std::slice::from_raw_parts_mut(ptr as *mut u8, size);
        slice.zeroize();
        
        let layout = Layout::from_size_align(size, 16).unwrap();
        dealloc(ptr as *mut u8, layout);
    }
}
