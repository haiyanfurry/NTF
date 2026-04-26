//! 设备指纹模块
//! 生成和验证设备指纹

use sha2::{Sha256, Digest};
use hmac::{Hmac, Mac};
use rand::RngCore;
use rand_chacha::ChaCha20Rng;
use std::collections::HashMap;
use std::sync::RwLock;
use thiserror::Error;

#[derive(Error, Debug)]
pub enum DeviceError {
    #[error("Random generation failed")]
    RandomError,
    #[error("Hash computation failed")]
    HashError,
    #[error("Invalid fingerprint format")]
    InvalidFormat,
}

type HmacSha256 = Hmac<Sha256>;

struct DeviceCache {
    fingerprints: HashMap<String, Vec<u8>>,
    rng: ChaCha20Rng,
}

static mut DEVICE_CACHE: Option<DeviceCache> = None;

pub fn init() {
    unsafe {
        if DEVICE_CACHE.is_none() {
            DEVICE_CACHE = Some(DeviceCache {
                fingerprints: HashMap::new(),
                rng: ChaCha20Rng::from_entropy(),
            });
        }
    }
}

pub fn clear_cache() {
    unsafe {
        if let Some(mut cache) = DEVICE_CACHE.take() {
            cache.fingerprints.clear();
        }
    }
}

pub fn generate_fingerprint() -> Result<String, DeviceError> {
    let mut hasher = Sha256::new();
    
    hasher.update(b"FindFriend");
    
    let mut random_bytes = [0u8; 32];
    unsafe {
        if let Some(ref mut cache) = DEVICE_CACHE {
            cache.rng.fill_bytes(&mut random_bytes);
        }
    }
    
    hasher.update(&random_bytes);
    
    let mut machine_id = [0u8; 64];
    if gethostname(&mut machine_id).is_ok() {
        hasher.update(&machine_id);
    }
    
    let mut username = [0u8; 64];
    if get_username(&mut username).is_ok() {
        hasher.update(&username);
    }
    
    let result = hasher.finalize();
    
    Ok(format!("FF-{:32x}-{:16x}", 
        u64::from_le_bytes(result[..8].try_into().unwrap()),
        u64::from_le_bytes(result[8..16].try_into().unwrap())))
}

fn gethostname(buf: &mut [u8]) -> Result<(), DeviceError> {
    #[cfg(unix)]
    {
        use std::ffi::CStr;
        let mut name = [0u8; 256];
        if unsafe { libc::gethostname(name.as_mut_ptr() as *mut libc::c_char, 256) } == 0 {
            if let Ok(s) = CStr::from_ptr(name.as_ptr()).to_str() {
                let copy_len = std::cmp::min(buf.len() - 1, s.len());
                buf[..copy_len].copy_from_slice(s[..copy_len].as_bytes());
                buf[copy_len] = 0;
                return Ok(());
            }
        }
    }
    Err(DeviceError::RandomError)
}

fn get_username(buf: &mut [u8]) -> Result<(), DeviceError> {
    #[cfg(unix)]
    {
        use std::ffi::CStr;
        let uid = unsafe { libc::getuid() };
        if let Some(pw) = unsafe { libc::getpwuid(uid) } {
            if let Ok(s) = CStr::from_ptr(pw).to_str() {
                let copy_len = std::cmp::min(buf.len() - 1, s.len());
                buf[..copy_len].copy_from_slice(s[..copy_len].as_bytes());
                buf[copy_len] = 0;
                return Ok(());
            }
        }
    }
    Err(DeviceError::RandomError)
}

pub fn verify_fingerprint(fingerprint: &str) -> Result<bool, DeviceError> {
    if fingerprint.len() != 49 {
        return Ok(false);
    }
    
    if !fingerprint.starts_with("FF-") {
        return Ok(false);
    }
    
    let parts: Vec<&str> = fingerprint[3..].split('-').collect();
    if parts.len() != 2 {
        return Ok(false);
    }
    
    if parts[0].len() != 32 || parts[1].len() != 16 {
        return Ok(false);
    }
    
    Ok(true)
}

pub fn sign_fingerprint(fingerprint: &str) -> Result<Vec<u8>, DeviceError> {
    let mut mac = HmacSha256::new_from_slice(b"FindFriend Device Signature")
        .map_err(|_| DeviceError::HashError)?;
    
    mac.update(fingerprint.as_bytes());
    
    let mut random_bytes = [0u8; 16];
    unsafe {
        if let Some(ref mut cache) = DEVICE_CACHE {
            cache.rng.fill_bytes(&mut random_bytes);
        }
    }
    mac.update(&random_bytes);
    
    let result = mac.finalize();
    
    let mut signature = random_bytes.to_vec();
    signature.extend_from_slice(&result);
    
    Ok(signature)
}

pub fn verify_fingerprint_signature(fingerprint: &str, signature: &[u8]) -> Result<bool, DeviceError> {
    if signature.len() < 48 {
        return Err(DeviceError::InvalidFormat);
    }
    
    let provided_random = &signature[..16];
    let provided_hmac = &signature[16..48];
    
    let mut mac = HmacSha256::new_from_slice(b"FindFriend Device Signature")
        .map_err(|_| DeviceError::HashError)?;
    
    mac.update(fingerprint.as_bytes());
    mac.update(provided_random);
    
    let expected_hmac = mac.finalize();
    
    Ok(provided_hmac == expected_hmac.as_slice())
}
