//! 输入验证模块
//! 提供 email、用户名验证，SQL/XSS 注入检测

use regex::Regex;
use std::sync::Once;
use log::{debug, warn};

static mut EMAIL_REGEX: Option<Regex> = None;
static mut USERNAME_REGEX: Option<Regex> = None;
static INIT: Once = Once::new();

#[derive(Debug, thiserror::Error)]
pub enum InputError {
    #[error("Invalid email format")]
    InvalidEmail,
    #[error("Invalid username format")]
    InvalidUsername,
    #[error("SQL injection detected")]
    SqlInjection,
    #[error("XSS injection detected")]
    XssInjection,
}

pub fn init() -> Result<(), InputError> {
    INIT.call_once(|| {
        // 初始化正则表达式
        unsafe {
            EMAIL_REGEX = Some(Regex::new(r"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$").unwrap());
            USERNAME_REGEX = Some(Regex::new(r"^[a-zA-Z0-9_]{3,32}$").unwrap());
        }
    });
    Ok(())
}

pub fn shutdown() {
    // 清理资源
}

pub fn validate_email(email: &str) -> Result<(), InputError> {
    if email.len() > 254 {
        return Err(InputError::InvalidEmail);
    }
    
    unsafe {
        if let Some(regex) = &EMAIL_REGEX {
            if regex.is_match(email) {
                return Ok(());
            }
        }
    }
    
    Err(InputError::InvalidEmail)
}

pub fn validate_username(username: &str) -> Result<(), InputError> {
    if username.len() < 3 || username.len() > 32 {
        return Err(InputError::InvalidUsername);
    }
    
    // 检查黑名单用户名
    let blacklist = ["admin", "root", "administrator", "sys", "system", "guest"];
    let lower = username.to_lowercase();
    
    if blacklist.contains(&lower.as_str()) {
        return Err(InputError::InvalidUsername);
    }
    
    unsafe {
        if let Some(regex) = &USERNAME_REGEX {
            if regex.is_match(username) {
                return Ok(());
            }
        }
    }
    
    Err(InputError::InvalidUsername)
}

pub fn check_sql_injection(input: &str) -> bool {
    let sql_patterns = [
        "union select", "select from", "drop table", "insert into",
        "delete from", "update set", "exec(", "xp_cmdshell",
        "or 1=1", "and 1=1", "--", "/*", "@@", "char(",
        "sleep(", "waitfor", "benchmark("
    ];
    
    let input_lower = input.to_lowercase();
    
    for pattern in &sql_patterns {
        if input_lower.contains(pattern) {
            warn!("[FF-Security] SQL injection pattern detected: {}", pattern);
            return true;
        }
    }
    
    false
}

pub fn check_xss_injection(input: &str) -> bool {
    let xss_patterns = [
        "<script", "javascript:", "onerror=", "onload=",
        "onclick=", "<iframe", "<img", "<svg",
        "<object", "alert(", "eval(", "document.",
        "window.", "prompt(", "confirm("
    ];
    
    let input_lower = input.to_lowercase();
    
    for pattern in &xss_patterns {
        if input_lower.contains(pattern) {
            warn!("[FF-Security] XSS pattern detected: {}", pattern);
            return true;
        }
    }
    
    false
}
