//! FindFriend Rust 安全模块
//! 输入验证、防注入、数据校验，带 FFI 接口

use std::ptr;
use std::ffi::{CStr, CString};
use std::sync::Once;
use regex::Regex;
use serde_json;
use phf::phf_map;
use libc::{c_char, c_int, c_void, size_t};
use log::{info, warn, error, debug};

// ====================================================================
// 安全错误码
// ====================================================================
#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FFSecurityErrorCode {
    Ok = 0,
    InvalidInput = -1,
    SqlInjection = -2,
    XssInjection = -3,
    JsonInjection = -4,
    XmlInjection = -5,
    RateLimit = -6,
    InvalidJson = -7,
    InvalidXml = -8,
    InvalidEmail = -9,
    InvalidPhone = -10,
    InvalidUsername = -11,
    InvalidPassword = -12,
    TooLong = -13,
    Blacklisted = -14,
    UnknownError = -99,
}

// ====================================================================
// 黑名单模式（编译时构建）
// ====================================================================
static SQL_INJECTION_PATTERNS: phf::Map<&'static str, &'static str> = phf_map! {
    "UNION SELECT" => "sql_union_select",
    "SELECT FROM" => "sql_select",
    "DROP TABLE" => "sql_drop",
    "OR 1=1" => "sql_or_1",
    "AND 1=1" => "sql_and_1",
    "INSERT INTO" => "sql_insert",
    "DELETE FROM" => "sql_delete",
    "UPDATE SET" => "sql_update",
    "EXEC(" => "sql_exec",
    "--" => "sql_comment",
    "/*" => "sql_block_comment",
    "@@" => "sql_variable",
    "CHAR(" => "sql_char",
    "ASCII(" => "sql_ascii",
    "SLEEP(" => "sql_sleep",
    "WAITFOR" => "sql_waitfor",
};

static XSS_INJECTION_PATTERNS: phf::Map<&'static str, &'static str> = phf_map! {
    "<script" => "xss_script",
    "javascript:" => "xss_js_url",
    "onerror" => "xss_onerror",
    "onload" => "xss_onload",
    "onclick" => "xss_onclick",
    "<iframe" => "xss_iframe",
    "<img" => "xss_img",
    "<svg" => "xss_svg",
    "<object" => "xss_object",
    "alert(" => "xss_alert",
    "eval(" => "xss_eval",
    "document." => "xss_document",
    "window." => "xss_window",
};

// ====================================================================
// 安全管理器
// ====================================================================
struct SecurityManager {
    initialized: bool,
    email_regex: Regex,
    phone_regex: Regex,
    username_regex: Regex,
}

impl SecurityManager {
    fn new() -> Self {
        SecurityManager {
            initialized: false,
            email_regex: Regex::new(r"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$").unwrap(),
            phone_regex: Regex::new(r"^\+?[0-9\s\-()]{7,20}$").unwrap(),
            username_regex: Regex::new(r"^[a-zA-Z0-9_]{3,32}$").unwrap(),
        }
    }
}

static mut SECURITY_MANAGER: Option<SecurityManager> = None;
static INIT: Once = Once::new();

// ====================================================================
// FFI - 初始化与销毁
// ====================================================================

#[no_mangle]
pub extern "C" fn rust_security_init() -> FFSecurityErrorCode {
    unsafe {
        INIT.call_once(|| {
            SECURITY_MANAGER = Some(SecurityManager::new());
        });

        if let Some(manager) = &mut SECURITY_MANAGER {
            if !manager.initialized {
                info!("[Rust-Security] Initializing security module...");
                manager.initialized = true;
                
                info!("[Rust-Security] Security module initialized successfully");
            }
        }
        
        FFSecurityErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_security_destroy() -> FFSecurityErrorCode {
    unsafe {
        if let Some(manager) = &mut SECURITY_MANAGER {
            info!("[Rust-Security] Destroying security module...");
            manager.initialized = false;
        }
        
        FFSecurityErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_security_is_initialized() -> bool {
    unsafe {
        SECURITY_MANAGER.as_ref().map_or(false, |m| m.initialized)
    }
}

// ====================================================================
// FFI - 输入验证
// ====================================================================

#[no_mangle]
pub extern "C" fn rust_validate_email(email: *const c_char) -> FFSecurityErrorCode {
    unsafe {
        if email.is_null() {
            return FFSecurityErrorCode::InvalidInput;
        }

        let manager = match SECURITY_MANAGER.as_ref() {
            Some(m) if m.initialized => m,
            _ => return FFSecurityErrorCode::InvalidInput,
        };

        let email_str = CStr::from_ptr(email).to_str().unwrap_or("");
        
        if email_str.len() > 254 {
            return FFSecurityErrorCode::TooLong;
        }

        if manager.email_regex.is_match(email_str) {
            debug!("[Rust-Security] Email validation passed: {}", email_str);
            FFSecurityErrorCode::Ok
        } else {
            warn!("[Rust-Security] Email validation failed: {}", email_str);
            FFSecurityErrorCode::InvalidEmail
        }
    }
}

#[no_mangle]
pub extern "C" fn rust_validate_username(username: *const c_char) -> FFSecurityErrorCode {
    unsafe {
        if username.is_null() {
            return FFSecurityErrorCode::InvalidInput;
        }

        let manager = match SECURITY_MANAGER.as_ref() {
            Some(m) if m.initialized => m,
            _ => return FFSecurityErrorCode::InvalidInput,
        };

        let user_str = CStr::from_ptr(username).to_str().unwrap_or("");
        
        if user_str.len() > 32 {
            return FFSecurityErrorCode::TooLong;
        }

        // 检查黑名单用户名
        let blacklist = ["admin", "root", "administrator", "sys", "system", "moderator"];
        let lower = user_str.to_lowercase();
        
        if blacklist.contains(&lower.as_str()) {
            warn!("[Rust-Security] Username blacklisted: {}", user_str);
            return FFSecurityErrorCode::Blacklisted;
        }

        if manager.username_regex.is_match(user_str) {
            FFSecurityErrorCode::Ok
        } else {
            FFSecurityErrorCode::InvalidUsername
        }
    }
}

#[no_mangle]
pub extern "C" fn rust_validate_password(password: *const c_char) -> FFSecurityErrorCode {
    unsafe {
        if password.is_null() {
            return FFSecurityErrorCode::InvalidInput;
        }

        let pwd_str = CStr::from_ptr(password).to_str().unwrap_or("");
        
        if pwd_str.len() < 8 {
            return FFSecurityErrorCode::InvalidPassword;
        }
        
        if pwd_str.len() > 128 {
            return FFSecurityErrorCode::TooLong;
        }

        // 简单密码策略
        let has_upper = pwd_str.chars().any(|c| c.is_ascii_uppercase());
        let has_lower = pwd_str.chars().any(|c| c.is_ascii_lowercase());
        let has_digit = pwd_str.chars().any(|c| c.is_ascii_digit());

        if has_upper && has_lower && has_digit {
            FFSecurityErrorCode::Ok
        } else {
            FFSecurityErrorCode::InvalidPassword
        }
    }
}

// ====================================================================
// FFI - 防注入检测
// ====================================================================

#[no_mangle]
pub extern "C" fn rust_check_sql_injection(input: *const c_char) -> FFSecurityErrorCode {
    unsafe {
        if input.is_null() {
            return FFSecurityErrorCode::InvalidInput;
        }

        let input_str = CStr::from_ptr(input).to_str().unwrap_or("").to_uppercase();
        
        for (pattern, name) in &SQL_INJECTION_PATTERNS {
            if input_str.contains(pattern) {
                warn!("[Rust-Security] SQL injection pattern detected: {} ({})", pattern, name);
                return FFSecurityErrorCode::SqlInjection;
            }
        }
        
        FFSecurityErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_check_xss_injection(input: *const c_char) -> FFSecurityErrorCode {
    unsafe {
        if input.is_null() {
            return FFSecurityErrorCode::InvalidInput;
        }

        let input_str = CStr::from_ptr(input).to_str().unwrap_or("").to_lowercase();
        
        for (pattern, name) in &XSS_INJECTION_PATTERNS {
            if input_str.contains(pattern) {
                warn!("[Rust-Security] XSS pattern detected: {} ({})", pattern, name);
                return FFSecurityErrorCode::XssInjection;
            }
        }
        
        FFSecurityErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_sanitize_string(
    input: *const c_char,
    output: *mut c_char,
    output_size: size_t
) -> FFSecurityErrorCode {
    unsafe {
        if input.is_null() || output.is_null() {
            return FFSecurityErrorCode::InvalidInput;
        }

        let input_str = CStr::from_ptr(input).to_str().unwrap_or("");
        
        // 基础清理：替换危险字符
        let sanitized = input_str
            .replace('<', "&lt;")
            .replace('>', "&gt;")
            .replace('"', "&quot;")
            .replace('\'', "&#x27;")
            .replace('&', "&amp;")
            .replace('/', "&#x2F;")
            .replace('\\', "&#x5C;");

        // 写入输出
        let sanitized_cstr = CString::new(sanitized).unwrap();
        let sanitized_bytes = sanitized_cstr.as_bytes_with_nul();
        
        if sanitized_bytes.len() > output_size {
            return FFSecurityErrorCode::TooLong;
        }
        
        let output_slice = std::slice::from_raw_parts_mut(output as *mut u8, sanitized_bytes.len());
        output_slice.copy_from_slice(sanitized_bytes);
        
        debug!("[Rust-Security] Sanitized input successfully");
        
        FFSecurityErrorCode::Ok
    }
}

// ====================================================================
// FFI - JSON 验证
// ====================================================================

#[no_mangle]
pub extern "C" fn rust_validate_json(json_str: *const c_char) -> FFSecurityErrorCode {
    unsafe {
        if json_str.is_null() {
            return FFSecurityErrorCode::InvalidInput;
        }

        let json_str = CStr::from_ptr(json_str).to_str().unwrap_or("");
        
        // 简单的长度检查
        if json_str.len() > 1024 * 1024 {  // 1MB 限制
            return FFSecurityErrorCode::TooLong;
        }

        match serde_json::from_str::<serde_json::Value>(json_str) {
            Ok(_) => {
                debug!("[Rust-Security] JSON validation passed");
                FFSecurityErrorCode::Ok
            },
            Err(e) => {
                warn!("[Rust-Security] JSON validation failed: {}", e);
                FFSecurityErrorCode::InvalidJson
            }
        }
    }
}

// ====================================================================
// FFI - XML 验证
// ====================================================================

#[no_mangle]
pub extern "C" fn rust_validate_xml(xml_str: *const c_char) -> FFSecurityErrorCode {
    unsafe {
        if xml_str.is_null() {
            return FFSecurityErrorCode::InvalidInput;
        }

        let xml_str = CStr::from_ptr(xml_str).to_str().unwrap_or("");
        
        // 简单的 XML 检查（实际项目应该用 proper 库）
        let bad_patterns = ["<!ENTITY", "SYSTEM", "PUBLIC", "<!DOCTYPE", "<![CDATA[", "<?xml"];
        
        for pattern in &bad_patterns {
            if xml_str.contains(pattern) {
                warn!("[Rust-Security] XML dangerous pattern: {}", pattern);
                return FFSecurityErrorCode::XmlInjection;
            }
        }
        
        // 基本格式检查
        let tag_open = xml_str.matches('<').count();
        let tag_close = xml_str.matches('>').count();
        
        if tag_open != tag_close {
            return FFSecurityErrorCode::InvalidXml;
        }
        
        FFSecurityErrorCode::Ok
    }
}

// ====================================================================
// FFI - 错误消息
// ====================================================================

#[no_mangle]
pub extern "C" fn rust_get_security_error_message(code: FFSecurityErrorCode) -> *const c_char {
    static MSG_OK: &str = "Security check passed";
    static MSG_INVALID_INPUT: &str = "Invalid input";
    static MSG_SQL_INJ: &str = "SQL injection detected";
    static MSG_XSS_INJ: &str = "XSS injection detected";
    static MSG_JSON_INJ: &str = "JSON injection detected";
    static MSG_XML_INJ: &str = "XML injection detected";
    static MSG_RATE_LIMIT: &str = "Rate limit exceeded";
    static MSG_INVALID_JSON: &str = "Invalid JSON";
    static MSG_INVALID_XML: &str = "Invalid XML";
    static MSG_INVALID_EMAIL: &str = "Invalid email";
    static MSG_INVALID_PHONE: &str = "Invalid phone";
    static MSG_INVALID_USERNAME: &str = "Invalid username";
    static MSG_INVALID_PWD: &str = "Invalid password";
    static MSG_TOO_LONG: &str = "Input too long";
    static MSG_BLACKLISTED: &str = "Blacklisted input";
    static MSG_UNKNOWN: &str = "Unknown security error";

    let msg = match code {
        FFSecurityErrorCode::Ok => MSG_OK,
        FFSecurityErrorCode::InvalidInput => MSG_INVALID_INPUT,
        FFSecurityErrorCode::SqlInjection => MSG_SQL_INJ,
        FFSecurityErrorCode::XssInjection => MSG_XSS_INJ,
        FFSecurityErrorCode::JsonInjection => MSG_JSON_INJ,
        FFSecurityErrorCode::XmlInjection => MSG_XML_INJ,
        FFSecurityErrorCode::RateLimit => MSG_RATE_LIMIT,
        FFSecurityErrorCode::InvalidJson => MSG_INVALID_JSON,
        FFSecurityErrorCode::InvalidXml => MSG_INVALID_XML,
        FFSecurityErrorCode::InvalidEmail => MSG_INVALID_EMAIL,
        FFSecurityErrorCode::InvalidPhone => MSG_INVALID_PHONE,
        FFSecurityErrorCode::InvalidUsername => MSG_INVALID_USERNAME,
        FFSecurityErrorCode::InvalidPassword => MSG_INVALID_PWD,
        FFSecurityErrorCode::TooLong => MSG_TOO_LONG,
        FFSecurityErrorCode::Blacklisted => MSG_BLACKLISTED,
        FFSecurityErrorCode::UnknownError => MSG_UNKNOWN,
    };
    
    CString::new(msg).unwrap().into_raw()
}
