//! 数据完整性模块
//! 提供校验和、HMAC、随机数生成

use sha2::{Sha256, Sha512, Digest};
use hmac::{Hmac, Mac};
use rand::RngCore;
use rand_chacha::ChaCha20Rng;
use thiserror::Error;

#[derive(Error, Debug)]
pub enum IntegrityError {
    #[error("Checksum computation failed")]
    ChecksumFailed,
    #[error("HMAC computation failed")]
    HmacFailed,
    #[error("Invalid input")]
    InvalidInput,
    #[error("Random generation failed")]
    RandomError,
}

type HmacSha256 = Hmac<Sha256>;
type HmacSha512 = Hmac<Sha512>;

static mut RNG: Option<ChaCha20Rng> = None;

pub fn init() {
    unsafe {
        if RNG.is_none() {
            RNG = Some(ChaCha20Rng::from_entropy());
        }
    }
}

pub fn compute_checksum(data: &[u8]) -> Result<u64, IntegrityError> {
    let mut hasher = Sha256::new();
    hasher.update(data);
    let result = hasher.finalize();
    
    let mut checksum = 0u64;
    for (i, &byte) in result.iter().enumerate().take(8) {
        checksum ^= (byte as u64) << ((i % 8) * 8);
    }
    
    Ok(checksum)
}

pub fn verify_checksum(data: &[u8], expected: u64) -> Result<bool, IntegrityError> {
    let computed = compute_checksum(data)?;
    Ok(computed == expected)
}

pub fn compute_hmac(key: &[u8], data: &[u8]) -> Result<Vec<u8>, IntegrityError> {
    let mut mac = HmacSha256::new_from_slice(key)
        .map_err(|_| IntegrityError::HmacFailed)?;
    
    mac.update(data);
    
    Ok(mac.finalize().to_vec())
}

pub fn verify_hmac(key: &[u8], data: &[u8], expected_hmac: &[u8]) -> Result<bool, IntegrityError> {
    let computed = compute_hmac(key, data)?;
    Ok(computed == expected_hmac)
}

pub fn random_bytes(buffer: &mut [u8]) -> Result<(), IntegrityError> {
    unsafe {
        if let Some(ref mut rng) = RNG {
            rng.fill_bytes(buffer);
            Ok(())
        } else {
            Err(IntegrityError::RandomError)
        }
    }
}
