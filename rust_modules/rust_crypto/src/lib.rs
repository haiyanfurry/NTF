//! FindFriend Rust 加密安全模块
//! 提供内存加密、TLS 辅助、安全密钥管理，带 FFI 接口

use std::ptr;
use std::ffi::{CStr, CString};
use std::sync::{Arc, Mutex, Once};
use libc::{c_char, c_int, c_void, size_t};
use log::{info, warn, error, debug};
use zeroize::{Zeroize, ZeroizeOnDrop};
use sha2::{Sha256, Sha512, Digest};
use hmac::{Hmac, Mac};
use rand::{RngCore, SeedableRng};
use rand_chacha::ChaCha20Rng;
use aes::Aes256;
use aes::cipher::{KeyInit, BlockEncrypt, BlockDecrypt, KeyIvInit};
use aes_gcm_siv::{Aes256GcmSiv, Nonce};
use aes_gcm_siv::aead::{AeadInPlace, Key, NewAead, generic_array::GenericArray};

// ====================================================================
// 错误码（与网络模块保持一致）
// ====================================================================
#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FFCryptoErrorCode {
    Ok = 0,
    InvalidParam = -1,
    KeyError = -2,
    EncryptError = -3,
    DecryptError = -4,
    HashError = -5,
    RandomError = -6,
    BufferTooSmall = -10,
    NotInitialized = -7,
    UnknownError = -99,
}

// ====================================================================
// 安全密钥结构（自动归零）
// ====================================================================
#[derive(Zeroize, ZeroizeOnDrop)]
struct SecureKey {
    key: [u8; 32],  // AES-256 密钥
    iv: [u8; 12],   // AES-GCM-SIV Nonce
    valid: bool,
}

impl Default for SecureKey {
    fn default() -> Self {
        SecureKey {
            key: [0u8; 32],
            iv: [0u8; 12],
            valid: false,
        }
    }
}

// ====================================================================
// 加密管理器
// ====================================================================
struct CryptoManager {
    initialized: bool,
    master_key: Arc<Mutex<SecureKey>>,
    rng: Arc<Mutex<ChaCha20Rng>>,
}

impl CryptoManager {
    fn new() -> Self {
        CryptoManager {
            initialized: false,
            master_key: Arc::new(Mutex::new(SecureKey::default())),
            rng: Arc::new(Mutex::new(ChaCha20Rng::from_entropy())),
        }
    }
}

static mut CRYPTO_MANAGER: Option<Arc<CryptoManager>> = None;
static INIT: Once = Once::new();

// ====================================================================
// FFI 接口 - 加密初始化
// ====================================================================

#[no_mangle]
pub extern "C" fn rust_crypto_init() -> FFCryptoErrorCode {
    unsafe {
        INIT.call_once(|| {
            CRYPTO_MANAGER = Some(Arc::new(CryptoManager::new()));
        });

        if let Some(manager) = &CRYPTO_MANAGER {
            if !manager.initialized {
                info!("[Rust-Crypto] Initializing crypto module...");
                
                // 生成主密钥（在实际实现中应该用安全的密钥派生）
                let mut master_key = manager.master_key.lock().unwrap();
                let mut rng = manager.rng.lock().unwrap();
                
                rng.fill_bytes(&mut master_key.key);
                rng.fill_bytes(&mut master_key.iv);
                master_key.valid = true;
                
                info!("[Rust-Crypto] Crypto module initialized successfully");
                FFCryptoErrorCode::Ok
            } else {
                FFCryptoErrorCode::Ok  // 已经初始化
            }
        } else {
            FFCryptoErrorCode::NotInitialized
        }
    }
}

#[no_mangle]
pub extern "C" fn rust_crypto_destroy() -> FFCryptoErrorCode {
    unsafe {
        if let Some(manager) = &CRYPTO_MANAGER {
            info!("[Rust-Crypto] Destroying crypto module...");
            
            let mut key = manager.master_key.lock().unwrap();
            key.zeroize();  // 安全归零
            key.valid = false;
        }
        
        // 注意：我们不重置静态变量以避免内存问题
        FFCryptoErrorCode::Ok
    }
}

// ====================================================================
// FFI 接口 - 哈希函数
// ====================================================================

#[no_mangle]
pub extern "C" fn rust_sha256(
    input: *const u8,
    input_len: size_t,
    output: *mut u8,
    output_size: size_t
) -> FFCryptoErrorCode {
    unsafe {
        if input.is_null() || output.is_null() {
            return FFCryptoErrorCode::InvalidParam;
        }
        if output_size < 32 {
            return FFCryptoErrorCode::BufferTooSmall;
        }

        let input_slice = std::slice::from_raw_parts(input, input_len);
        
        let mut hasher = Sha256::new();
        hasher.update(input_slice);
        let result = hasher.finalize();
        
        let output_slice = std::slice::from_raw_parts_mut(output, 32);
        output_slice.copy_from_slice(&result);
        
        FFCryptoErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_sha512(
    input: *const u8,
    input_len: size_t,
    output: *mut u8,
    output_size: size_t
) -> FFCryptoErrorCode {
    unsafe {
        if input.is_null() || output.is_null() {
            return FFCryptoErrorCode::InvalidParam;
        }
        if output_size < 64 {
            return FFCryptoErrorCode::BufferTooSmall;
        }

        let input_slice = std::slice::from_raw_parts(input, input_len);
        
        let mut hasher = Sha512::new();
        hasher.update(input_slice);
        let result = hasher.finalize();
        
        let output_slice = std::slice::from_raw_parts_mut(output, 64);
        output_slice.copy_from_slice(&result);
        
        FFCryptoErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_hmac_sha256(
    key: *const u8,
    key_len: size_t,
    data: *const u8,
    data_len: size_t,
    output: *mut u8,
    output_size: size_t
) -> FFCryptoErrorCode {
    unsafe {
        if key.is_null() || data.is_null() || output.is_null() {
            return FFCryptoErrorCode::InvalidParam;
        }
        if output_size < 32 {
            return FFCryptoErrorCode::BufferTooSmall;
        }
        
        let key_slice = std::slice::from_raw_parts(key, key_len);
        let data_slice = std::slice::from_raw_parts(data, data_len);
        
        let mac_result = <Hmac<Sha256> as Mac>::new_from_slice(key_slice);
        let mut mac = match mac_result {
            Ok(m) => m,
            Err(_) => return FFCryptoErrorCode::KeyError,
        };
        
        mac.update(data_slice);
        let result = mac.finalize().into_bytes();
        
        let output_slice = std::slice::from_raw_parts_mut(output, 32);
        output_slice.copy_from_slice(&result);
        
        FFCryptoErrorCode::Ok
    }
}

// ====================================================================
// FFI 接口 - 加解密
// ====================================================================

#[no_mangle]
pub extern "C" fn rust_encrypt_aes256_gcm(
    plaintext: *const u8,
    plaintext_len: size_t,
    associated_data: *const u8,
    ad_len: size_t,
    key: *const u8,
    key_len: size_t,
    nonce: *const u8,
    nonce_len: size_t,
    ciphertext: *mut u8,
    ciphertext_size: *mut size_t
) -> FFCryptoErrorCode {
    unsafe {
        if plaintext.is_null() || ciphertext.is_null() || ciphertext_size.is_null() 
            || key.is_null() || nonce.is_null() {
            return FFCryptoErrorCode::InvalidParam;
        }
        
        if key_len < 32 || nonce_len < 12 {
            return FFCryptoErrorCode::KeyError;
        }
        
        let required_size = plaintext_len + 16;  // 额外 16 字节 tag
        if *ciphertext_size < required_size {
            *ciphertext_size = required_size;
            return FFCryptoErrorCode::BufferTooSmall;
        }
        
        let plain_slice = std::slice::from_raw_parts(plaintext, plaintext_len);
        let ad_slice = std::slice::from_raw_parts(associated_data, ad_len);
        let key_slice = std::slice::from_raw_parts(key, 32);
        let nonce_slice = std::slice::from_raw_parts(nonce, 12);
        
        let key_array = GenericArray::from_slice(&key_slice[..32]);
        let cipher = Aes256GcmSiv::new(key_array);
        let nonce_obj = Nonce::from_slice(&nonce_slice[..12]);
        
        let mut buffer = plain_slice.to_vec();
        let encrypt_result = cipher.encrypt_in_place(nonce_obj, ad_slice, &mut buffer);
        if encrypt_result.is_err() {
            return FFCryptoErrorCode::EncryptError;
        }
        
        let cipher_slice = std::slice::from_raw_parts_mut(ciphertext, buffer.len());
        cipher_slice.copy_from_slice(&buffer);
        
        *ciphertext_size = buffer.len();
        
        debug!("[Rust-Crypto] Encrypted {} bytes -> {} bytes (with tag)", plaintext_len, buffer.len());
        
        FFCryptoErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_decrypt_aes256_gcm(
    ciphertext: *const u8,
    ciphertext_len: size_t,
    associated_data: *const u8,
    ad_len: size_t,
    key: *const u8,
    key_len: size_t,
    nonce: *const u8,
    nonce_len: size_t,
    plaintext: *mut u8,
    plaintext_size: *mut size_t
) -> FFCryptoErrorCode {
    unsafe {
        if ciphertext.is_null() || plaintext.is_null() || plaintext_size.is_null()
            || key.is_null() || nonce.is_null() {
            return FFCryptoErrorCode::InvalidParam;
        }
        
        let required_size = ciphertext_len - 16;  // 减去 tag
        if *plaintext_size < required_size {
            *plaintext_size = required_size;
            return FFCryptoErrorCode::BufferTooSmall;
        }
        
        let cipher_slice = std::slice::from_raw_parts(ciphertext, ciphertext_len);
        let ad_slice = std::slice::from_raw_parts(associated_data, ad_len);
        let key_slice = std::slice::from_raw_parts(key, 32);
        let nonce_slice = std::slice::from_raw_parts(nonce, 12);
        
        let key_array = GenericArray::from_slice(&key_slice[..32]);
        let cipher = Aes256GcmSiv::new(key_array);
        let nonce_obj = Nonce::from_slice(&nonce_slice[..12]);
        
        let mut buffer = cipher_slice.to_vec();
        let decrypt_result = cipher.decrypt_in_place(nonce_obj, ad_slice, &mut buffer);
        if decrypt_result.is_err() {
            return FFCryptoErrorCode::DecryptError;
        }
        
        let plain_slice = std::slice::from_raw_parts_mut(plaintext, buffer.len());
        plain_slice.copy_from_slice(&buffer);
        
        *plaintext_size = buffer.len();
        
        debug!("[Rust-Crypto] Decrypted {} bytes -> {} bytes", ciphertext_len, buffer.len());
        
        FFCryptoErrorCode::Ok
    }
}

// ====================================================================
// FFI 接口 - 随机数生成
// ====================================================================

#[no_mangle]
pub extern "C" fn rust_random_bytes(
    buffer: *mut u8,
    buffer_size: size_t
) -> FFCryptoErrorCode {
    unsafe {
        if buffer.is_null() {
            return FFCryptoErrorCode::InvalidParam;
        }
        
        if CRYPTO_MANAGER.is_none() {
            return FFCryptoErrorCode::NotInitialized;
        }
        
        let manager = CRYPTO_MANAGER.as_ref().unwrap();
        let mut rng = manager.rng.lock().unwrap();
        
        let slice = std::slice::from_raw_parts_mut(buffer, buffer_size);
        rng.fill_bytes(slice);
        
        debug!("[Rust-Crypto] Generated {} random bytes", buffer_size);
        
        FFCryptoErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_secure_zero(
    buffer: *mut u8,
    buffer_size: size_t
) -> FFCryptoErrorCode {
    unsafe {
        if buffer.is_null() {
            return FFCryptoErrorCode::InvalidParam;
        }
        
        let slice = std::slice::from_raw_parts_mut(buffer, buffer_size);
        slice.zeroize();
        
        debug!("[Rust-Crypto] Securely zeroed {} bytes", buffer_size);
        
        FFCryptoErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_get_error_message(code: FFCryptoErrorCode) -> *const c_char {
    static MSG_OK: &str = "Crypto operation successful";
    static MSG_INVALID: &str = "Invalid parameter";
    static MSG_KEY: &str = "Key error";
    static MSG_ENCRYPT: &str = "Encryption failed";
    static MSG_DECRYPT: &str = "Decryption failed";
    static MSG_HASH: &str = "Hash error";
    static MSG_RANDOM: &str = "Random number error";
    static MSG_BUFFER: &str = "Buffer too small";
    static MSG_NOT_INIT: &str = "Not initialized";
    static MSG_UNKNOWN: &str = "Unknown crypto error";

    let msg = match code {
        FFCryptoErrorCode::Ok => MSG_OK,
        FFCryptoErrorCode::InvalidParam => MSG_INVALID,
        FFCryptoErrorCode::KeyError => MSG_KEY,
        FFCryptoErrorCode::EncryptError => MSG_ENCRYPT,
        FFCryptoErrorCode::DecryptError => MSG_DECRYPT,
        FFCryptoErrorCode::HashError => MSG_HASH,
        FFCryptoErrorCode::RandomError => MSG_RANDOM,
        FFCryptoErrorCode::BufferTooSmall => MSG_BUFFER,
        FFCryptoErrorCode::NotInitialized => MSG_NOT_INIT,
        FFCryptoErrorCode::UnknownError => MSG_UNKNOWN,
    };
    
    CString::new(msg).unwrap().into_raw()
}

#[no_mangle]
pub extern "C" fn rust_crypto_is_initialized() -> bool {
    unsafe {
        CRYPTO_MANAGER.is_some()
    }
}
