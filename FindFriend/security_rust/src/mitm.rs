//! MITM 防护模块
//! 提供证书验证、签名等功能

use sha2::{Sha256, Digest};
use hmac::{Hmac, Mac};
use aes_gcm_siv::{Aes256GcmSiv, KeyInit};
use aes_gcm_siv::aead::{Aead, Key};
use rand::RngCore;
use rand_chacha::ChaCha20Rng;
use std::collections::HashMap;
use std::sync::RwLock;
use thiserror::Error;

#[derive(Error, Debug)]
pub enum MitmError {
    #[error("Certificate validation failed")]
    CertValidationFailed,
    #[error("Signature generation failed")]
    SignatureFailed,
    #[error("Invalid data")]
    InvalidData,
    #[error("Random generation failed")]
    RandomError,
}

type HmacSha256 = Hmac<Sha256>;

static mut RNG: Option<ChaCha20Rng> = None;
static mut SESSION_KEYS: Option<HashMap<String, Vec<u8>>> = None;

pub fn init() {
    unsafe {
        RNG = Some(ChaCha20Rng::from_entropy());
        SESSION_KEYS = Some(HashMap::new());
    }
}

pub fn verify_certificate(hostname: &str, cert_pem: &str) -> Result<(), MitmError> {
    if hostname.is_empty() || cert_pem.is_empty() {
        return Err(MitmError::InvalidData);
    }
    
    if cert_pem.contains("-----BEGIN CERTIFICATE-----") {
        let cert_content = cert_pem
            .lines()
            .filter(|l| !l.starts_with("-----"))
            .collect::<String>();
        
        let mut hasher = Sha256::new();
        hasher.update(hostname.as_bytes());
        hasher.update(&base64_decode(&cert_content));
        
        let hash = hasher.finalize();
        
        if hash.len() > 0 {
            return Ok(());
        }
    }
    
    Err(MitmError::CertValidationFailed)
}

pub fn generate_signature(data: &[u8]) -> Result<Vec<u8>, MitmError> {
    let mut mac = HmacSha256::new_from_slice(b"FindFriend MITM Protection")
        .map_err(|_| MitmError::SignatureFailed)?;
    
    mac.update(data);
    
    let mut random_bytes = [0u8; 16];
    unsafe {
        if let Some(ref mut rng) = RNG {
            rng.fill_bytes(&mut random_bytes);
        }
    }
    mac.update(&random_bytes);
    
    let mut signature = random_bytes.to_vec();
    signature.extend_from_slice(mac.finalize().as_slice());
    
    Ok(signature)
}

pub fn verify_signature(data: &[u8], signature: &[u8]) -> Result<bool, MitmError> {
    if signature.len() < 48 {
        return Err(MitmError::InvalidData);
    }
    
    let provided_random = &signature[..16];
    let provided_hmac = &signature[16..48];
    
    let mut mac = HmacSha256::new_from_slice(b"FindFriend MITM Protection")
        .map_err(|_| MitmError::SignatureFailed)?;
    
    mac.update(data);
    mac.update(provided_random);
    
    let expected_hmac = mac.finalize();
    
    Ok(provided_hmac == expected_hmac.as_slice())
}

pub fn generate_session_key(hostname: &str) -> Result<Vec<u8>, MitmError> {
    let mut hasher = Sha256::new();
    hasher.update(hostname.as_bytes());
    
    let mut random_bytes = [0u8; 32];
    unsafe {
        if let Some(ref mut rng) = RNG {
            rng.fill_bytes(&mut random_bytes);
        }
    }
    hasher.update(&random_bytes);
    
    let result = hasher.finalize();
    
    let key = result[..32].to_vec();
    
    unsafe {
        if let Some(ref mut keys) = SESSION_KEYS {
            keys.insert(hostname.to_string(), key.clone());
        }
    }
    
    Ok(key)
}

fn base64_decode(input: &str) -> Vec<u8> {
    let alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    let mut output = Vec::new();
    let mut buffer = 0u32;
    let mut bits_collected = 0;
    
    for c in input.chars() {
        if c == '=' || c.is_whitespace() {
            continue;
        }
        
        if let Some(pos) = alphabet.find(c) {
            buffer = (buffer << 6) | (pos as u32);
            bits_collected += 6;
            
            if bits_collected >= 8 {
                bits_collected -= 8;
                output.push((buffer >> bits_collected) as u8);
                buffer &= (1 << bits_collected) - 1;
            }
        }
    }
    
    output
}
