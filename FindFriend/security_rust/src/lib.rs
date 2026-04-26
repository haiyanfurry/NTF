// FindFriend Rust 安全模块 - 扩展功能

use std::sync::Once;
use std::collections::HashMap;
use libc::{c_char, c_void, size_t, uint8_t, uint32_t, uint64_t, bool as c_bool};
use log::{info, error, debug, warn};

mod memory;
mod device;
mod mitm;
mod integrity;
mod location;
mod input;
mod anti_crawler;
mod anti_dos;
mod anti_fake_gps;
mod root_detect;

// ====================================================================
// 错误类型
// ====================================================================
#[repr(C)]
pub enum FFSRResult {
    Ok = 0,
    InvalidParam = -1,
    Crypto = -2,
    Hash = -3,
    Validation = -4,
    Device = -5,
    Memory = -6,
    Timeout = -7,
    RootDetected = -8,
    InvalidLocation = -9,
    CrawlerDetected = -10,
    DoSDetected = -11,
    Unknown = -99,
}

// ====================================================================
// 全局状态
// ====================================================================
static mut INITIALIZED: bool = false;
static INIT: Once = Once::new();

// ====================================================================
// FFI 接口 - 初始化
// ====================================================================
#[no_mangle]
pub extern "C" fn ff_sr_is_initialized() -> c_bool {
    unsafe { INITIALIZED }
}

#[no_mangle]
pub extern "C" fn ff_sr_init() -> FFSRResult {
    INIT.call_once(|| {
        info!("[FF-Security] Initializing Rust security module...");
        
        if let Err(e) = memory::init() {
            error!("[FF-Security] Failed to initialize memory: {:?}", e);
            return;
        }
        
        if let Err(e) = device::init() {
            error!("[FF-Security] Failed to initialize device: {:?}", e);
            return;
        }
        
        if let Err(e) = mitm::init() {
            error!("[FF-Security] Failed to initialize MITM: {:?}", e);
            return;
        }
        
        if let Err(e) = integrity::init() {
            error!("[FF-Security] Failed to initialize integrity: {:?}", e);
            return;
        }
        
        if let Err(e) = location::init() {
            error!("[FF-Security] Failed to initialize location: {:?}", e);
            return;
        }
        
        if let Err(e) = input::init() {
            error!("[FF-Security] Failed to initialize input: {:?}", e);
            return;
        }
        
        if let Err(e) = anti_crawler::init() {
            error!("[FF-Security] Failed to initialize anti-crawler: {:?}", e);
            return;
        }
        
        if let Err(e) = anti_dos::init() {
            error!("[FF-Security] Failed to initialize anti-DOS: {:?}", e);
            return;
        }
        
        if let Err(e) = anti_fake_gps::init() {
            error!("[FF-Security] Failed to initialize anti-fake GPS: {:?}", e);
            return;
        }
        
        if let Err(e) = root_detect::init() {
            error!("[FF-Security] Failed to initialize root detect: {:?}", e);
            return;
        }
        
        unsafe { INITIALIZED = true; }
        info!("[FF-Security] Rust security module initialized successfully");
    });
    
    FFSRResult::Ok
}

#[no_mangle]
pub extern "C" fn ff_sr_shutdown() -> FFSRResult {
    unsafe {
        if !INITIALIZED {
            return FFSRResult::Ok;
        }
        
        info!("[FF-Security] Shutting down Rust security module...");
        
        memory::shutdown();
        device::shutdown();
        mitm::shutdown();
        integrity::shutdown();
        location::shutdown();
        input::shutdown();
        anti_crawler::shutdown();
        anti_dos::shutdown();
        anti_fake_gps::shutdown();
        root_detect::shutdown();
        
        INITIALIZED = false;
        info!("[FF-Security] Rust security module shut down");
    }
    
    FFSRResult::Ok
}

// ====================================================================
// FFI 接口 - 内存加密
// ====================================================================
#[no_mangle]
pub extern "C" fn ff_sr_memory_encrypt(buffer: *mut uint8_t, len: size_t, key_id: uint32_t) -> FFSRResult {
    if buffer.is_null() || len == 0 {
        return FFSRResult::InvalidParam;
    }
    
    memory::encrypt_region(unsafe { std::slice::from_raw_parts_mut(buffer, len) }, key_id)
        .map(|_| FFSRResult::Ok)
        .unwrap_or(FFSRResult::Crypto)
}

#[no_mangle]
pub extern "C" fn ff_sr_memory_decrypt(buffer: *mut uint8_t, len: size_t, key_id: uint32_t) -> FFSRResult {
    if buffer.is_null() || len == 0 {
        return FFSRResult::InvalidParam;
    }
    
    memory::decrypt_region(unsafe { std::slice::from_raw_parts_mut(buffer, len) }, key_id)
        .map(|_| FFSRResult::Ok)
        .unwrap_or(FFSRResult::Crypto)
}

#[no_mangle]
pub extern "C" fn ff_sr_secure_malloc(size: size_t) -> *mut c_void {
    memory::secure_alloc(size)
}

#[no_mangle]
pub extern "C" fn ff_sr_secure_free(ptr: *mut c_void, size: size_t) {
    if !ptr.is_null() {
        memory::secure_free(ptr, size);
    }
}

// ====================================================================
// FFI 接口 - 设备指纹
// ====================================================================
#[no_mangle]
pub extern "C" fn ff_sr_generate_device_fingerprint(fingerprint: *mut c_char, len: size_t) -> FFSRResult {
    if fingerprint.is_null() || len < 256 {
        return FFSRResult::InvalidParam;
    }
    
    match device::generate_fingerprint() {
        Ok(fp) => {
            let fp_bytes = fp.as_bytes();
            if fp_bytes.len() >= len {
                return FFSRResult::InvalidParam;
            }
            
            unsafe {
                std::ptr::copy_nonoverlapping(
                    fp_bytes.as_ptr(),
                    fingerprint as *mut uint8_t,
                    fp_bytes.len()
                );
                *fingerprint.add(fp_bytes.len()) = 0;
            }
            
            FFSRResult::Ok
        },
        Err(e) => {
            error!("[FF-Security] Failed to generate fingerprint: {:?}", e);
            FFSRResult::Device
        }
    }
}

#[no_mangle]
pub extern "C" fn ff_sr_verify_device_fingerprint(fingerprint: *const c_char) -> c_bool {
    if fingerprint.is_null() {
        return false;
    }
    
    let fp_str = unsafe { std::ffi::CStr::from_ptr(fingerprint) }
        .to_str()
        .unwrap_or("");
    
    device::verify_fingerprint(fp_str).unwrap_or(false)
}

// ====================================================================
// FFI 接口 - MITM 防护
// ====================================================================
#[no_mangle]
pub extern "C" fn ff_sr_verify_certificate(hostname: *const c_char, cert_pem: *const c_char, cert_len: size_t) -> FFSRResult {
    if hostname.is_null() || cert_pem.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    let host = unsafe { std::ffi::CStr::from_ptr(hostname) }
        .to_str()
        .unwrap_or("");
    
    let cert = unsafe { std::ffi::CStr::from_ptr(cert_pem) }
        .to_str()
        .unwrap_or("");
    
    mitm::verify_certificate(host, cert)
        .map(|_| FFSRResult::Ok)
        .unwrap_or(FFSRResult::Crypto)
}

#[no_mangle]
pub extern "C" fn ff_sr_generate_signature(data: *const uint8_t, data_len: size_t, signature: *mut uint8_t, sig_len: *mut size_t) -> FFSRResult {
    if data.is_null() || signature.is_null() || sig_len.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    let data_slice = unsafe { std::slice::from_raw_parts(data, data_len) };
    
    match mitm::generate_signature(data_slice) {
        Ok(sig) => {
            if sig.len() > unsafe { *sig_len } {
                return FFSRResult::InvalidParam;
            }
            
            unsafe {
                std::ptr::copy_nonoverlapping(sig.as_ptr(), signature, sig.len());
                *sig_len = sig.len();
            }
            
            FFSRResult::Ok
        },
        Err(e) => {
            error!("[FF-Security] Failed to generate signature: {:?}", e);
            FFSRResult::Crypto
        }
    }
}

// ====================================================================
// FFI 接口 - 数据完整性
// ====================================================================
#[no_mangle]
pub extern "C" fn ff_sr_compute_checksum(data: *const uint8_t, data_len: size_t, checksum: *mut uint64_t) -> FFSRResult {
    if data.is_null() || checksum.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    let data_slice = unsafe { std::slice::from_raw_parts(data, data_len) };
    
    match integrity::compute_checksum(data_slice) {
        Ok(cs) => {
            unsafe { *checksum = cs; }
            FFSRResult::Ok
        },
        Err(e) => {
            error!("[FF-Security] Failed to compute checksum: {:?}", e);
            FFSRResult::Hash
        }
    }
}

#[no_mangle]
pub extern "C" fn ff_sr_verify_checksum(data: *const uint8_t, data_len: size_t, expected: uint64_t) -> FFSRResult {
    if data.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    let data_slice = unsafe { std::slice::from_raw_parts(data, data_len) };
    
    match integrity::verify_checksum(data_slice, expected) {
        Ok(valid) => {
            if valid { FFSRResult::Ok } else { FFSRResult::Validation }
        },
        Err(e) => {
            error!("[FF-Security] Failed to verify checksum: {:?}", e);
            FFSRResult::Hash
        }
    }
}

// ====================================================================
// FFI 接口 - 位置模糊化
// ====================================================================
#[no_mangle]
pub extern "C" fn ff_sr_obfuscate_location(exact_lat: f64, exact_lon: f64, radius_meters: f64, out_lat: *mut f64, out_lon: *mut f64) -> FFSRResult {
    if out_lat.is_null() || out_lon.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    match location::obfuscate(exact_lat, exact_lon, radius_meters) {
        Ok((lat, lon)) => {
            unsafe {
                *out_lat = lat;
                *out_lon = lon;
            }
            FFSRResult::Ok
        },
        Err(e) => {
            error!("[FF-Security] Failed to obfuscate location: {:?}", e);
            FFSRResult::InvalidLocation
        }
    }
}

#[no_mangle]
pub extern "C" fn ff_sr_validate_location(lat: f64, lon: f64, speed: f64, heading: f64, is_reasonable: *mut c_bool) -> FFSRResult {
    if is_reasonable.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    let reasonable = location::validate_plausibility(lat, lon, speed, heading);
    unsafe { *is_reasonable = reasonable; }
    
    FFSRResult::Ok
}

// ====================================================================
// FFI 接口 - 输入验证
// ====================================================================
#[no_mangle]
pub extern "C" fn ff_sr_validate_email(email: *const c_char) -> FFSRResult {
    if email.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    let email_str = unsafe { std::ffi::CStr::from_ptr(email) }
        .to_str()
        .unwrap_or("");
    
    input::validate_email(email_str)
        .map(|_| FFSRResult::Ok)
        .unwrap_or(FFSRResult::Validation)
}

#[no_mangle]
pub extern "C" fn ff_sr_validate_username(username: *const c_char) -> FFSRResult {
    if username.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    let user_str = unsafe { std::ffi::CStr::from_ptr(username) }
        .to_str()
        .unwrap_or("");
    
    input::validate_username(user_str)
        .map(|_| FFSRResult::Ok)
        .unwrap_or(FFSRResult::Validation)
}

#[no_mangle]
pub extern "C" fn ff_sr_check_sql_injection(input: *const c_char) -> FFSRResult {
    if input.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    let input_str = unsafe { std::ffi::CStr::from_ptr(input) }
        .to_str()
        .unwrap_or("");
    
    if input::check_sql_injection(input_str) {
        FFSRResult::Validation
    } else {
        FFSRResult::Ok
    }
}

#[no_mangle]
pub extern "C" fn ff_sr_check_xss_injection(input: *const c_char) -> FFSRResult {
    if input.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    let input_str = unsafe { std::ffi::CStr::from_ptr(input) }
        .to_str()
        .unwrap_or("");
    
    if input::check_xss_injection(input_str) {
        FFSRResult::Validation
    } else {
        FFSRResult::Ok
    }
}

// ====================================================================
// FFI 接口 - 反爬虫
// ====================================================================
#[no_mangle]
pub extern "C" fn ff_sr_check_crawler_behavior(uid: uint32_t, query_count: uint32_t, time_window: uint32_t, is_crawler: *mut c_bool) -> FFSRResult {
    if is_crawler.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    let result = anti_crawler::detect_crawler(uid, query_count, time_window);
    unsafe { *is_crawler = result };
    
    if result {
        FFSRResult::CrawlerDetected
    } else {
        FFSRResult::Ok
    }
}

// ====================================================================
// FFI 接口 - 反 DoS
// ====================================================================
#[no_mangle]
pub extern "C" fn ff_sr_check_dos_attack(ip: *const c_char, request_count: uint32_t, time_window: uint32_t, is_dos: *mut c_bool) -> FFSRResult {
    if ip.is_null() || is_dos.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    let ip_str = unsafe { std::ffi::CStr::from_ptr(ip) }
        .to_str()
        .unwrap_or("");
    
    let result = anti_dos::detect_dos(ip_str, request_count, time_window);
    unsafe { *is_dos = result };
    
    if result {
        FFSRResult::DoSDetected
    } else {
        FFSRResult::Ok
    }
}

// ====================================================================
// FFI 接口 - 反虚假 GPS
// ====================================================================
#[no_mangle]
pub extern "C" fn ff_sr_detect_fake_gps(lat: f64, lon: f64, speed: f64, last_lat: f64, last_lon: f64, time_diff: uint32_t, is_fake: *mut c_bool) -> FFSRResult {
    if is_fake.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    let result = anti_fake_gps::detect_fake_location(lat, lon, speed, last_lat, last_lon, time_diff);
    unsafe { *is_fake = result };
    
    if result {
        FFSRResult::InvalidLocation
    } else {
        FFSRResult::Ok
    }
}

// ====================================================================
// FFI 接口 - Root 检测
// ====================================================================
#[no_mangle]
pub extern "C" fn ff_sr_check_root() -> FFSRResult {
    if root_detect::is_rooted() {
        FFSRResult::RootDetected
    } else {
        FFSRResult::Ok
    }
}

// ====================================================================
// FFI 接口 - 安全随机数
// ====================================================================
#[no_mangle]
pub extern "C" fn ff_sr_random_bytes(buffer: *mut uint8_t, len: size_t) -> FFSRResult {
    if buffer.is_null() {
        return FFSRResult::InvalidParam;
    }
    
    let slice = unsafe { std::slice::from_raw_parts_mut(buffer, len) };
    
    integrity::random_bytes(slice)
        .map(|_| FFSRResult::Ok)
        .unwrap_or(FFSRResult::Crypto)
}

// ====================================================================
// FFI 接口 - 错误处理
// ====================================================================
#[no_mangle]
pub extern "C" fn ff_sr_get_error_message(result: FFSRResult) -> *const c_char {
    match result {
        FFSRResult::Ok => "Success\0",
        FFSRResult::InvalidParam => "Invalid parameter\0",
        FFSRResult::Crypto => "Cryptographic error\0",
        FFSRResult::Hash => "Hash error\0",
        FFSRResult::Validation => "Validation failed\0",
        FFSRResult::Device => "Device error\0",
        FFSRResult::Memory => "Memory error\0",
        FFSRResult::Timeout => "Timeout\0",
        FFSRResult::RootDetected => "Root/Jailbreak detected\0",
        FFSRResult::InvalidLocation => "Invalid location\0",
        FFSRResult::CrawlerDetected => "Crawler behavior detected\0",
        FFSRResult::DoSDetected => "DoS attack detected\0",
        FFSRResult::Unknown => "Unknown error\0",
    }.as_ptr() as *const c_char
}
