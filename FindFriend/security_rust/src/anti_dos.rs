//! 反 DoS 模块
//! 检测和防止 DoS 攻击

use std::collections::HashMap;
use std::sync::{RwLock, Once};
use std::time::{Duration, SystemTime};
use log::{debug, warn};

struct DoSDetector {
    ip_requests: RwLock<HashMap<String, (u32, SystemTime)>>, // (ip, (request_count, last_request_time))
    max_requests: u32,
    time_window: Duration,
    blocked_ips: RwLock<HashMap<String, SystemTime>>, // (ip, block_until)
    block_duration: Duration,
}

static mut DETECTOR: Option<DoSDetector> = None;
static INIT: Once = Once::new();

pub fn init() -> Result<(), Box<dyn std::error::Error>> {
    INIT.call_once(|| {
        unsafe {
            DETECTOR = Some(DoSDetector {
                ip_requests: RwLock::new(HashMap::new()),
                max_requests: 1000, // 1000 requests per 60 seconds
                time_window: Duration::from_secs(60),
                blocked_ips: RwLock::new(HashMap::new()),
                block_duration: Duration::from_secs(300), // 5 minutes
            });
        }
    });
    Ok(())
}

pub fn shutdown() {
    // 清理资源
}

pub fn detect_dos(ip: &str, request_count: u32, time_window: u32) -> bool {
    // 检查是否在黑名单中
    if is_ip_blocked(ip) {
        return true;
    }
    
    unsafe {
        if let Some(detector) = &DETECTOR {
            let mut requests = detector.ip_requests.write().unwrap();
            
            let now = SystemTime::now();
            let window = Duration::from_secs(time_window as u64);
            
            if let Some((count, last_time)) = requests.get(ip) {
                if let Ok(elapsed) = now.duration_since(*last_time) {
                    if elapsed < window {
                        let new_count = *count + request_count;
                        if new_count > detector.max_requests {
                            warn!("[FF-Security] DoS attack detected: ip={}, requests={}", ip, new_count);
                            block_ip(ip);
                            requests.insert(ip.to_string(), (new_count, now));
                            return true;
                        } else {
                            requests.insert(ip.to_string(), (new_count, now));
                        }
                    } else {
                        // 时间窗口重置
                        requests.insert(ip.to_string(), (request_count, now));
                    }
                }
            } else {
                requests.insert(ip.to_string(), (request_count, now));
            }
        }
    }
    
    false
}

pub fn is_ip_blocked(ip: &str) -> bool {
    unsafe {
        if let Some(detector) = &DETECTOR {
            let blocked = detector.blocked_ips.read().unwrap();
            if let Some(block_until) = blocked.get(ip) {
                if let Ok(now) = SystemTime::now().duration_since(*block_until) {
                    if now < detector.block_duration {
                        return true;
                    } else {
                        // 解除封锁
                        let mut blocked_write = detector.blocked_ips.write().unwrap();
                        blocked_write.remove(ip);
                    }
                }
            }
        }
    }
    
    false
}

fn block_ip(ip: &str) {
    unsafe {
        if let Some(detector) = &DETECTOR {
            let mut blocked = detector.blocked_ips.write().unwrap();
            blocked.insert(ip.to_string(), SystemTime::now());
            warn!("[FF-Security] IP blocked: {}", ip);
        }
    }
}

pub fn add_request(ip: &str) {
    unsafe {
        if let Some(detector) = &DETECTOR {
            let mut requests = detector.ip_requests.write().unwrap();
            let now = SystemTime::now();
            
            if let Some((count, _)) = requests.get(ip) {
                requests.insert(ip.to_string(), (*count + 1, now));
            } else {
                requests.insert(ip.to_string(), (1, now));
            }
        }
    }
}

pub fn unblock_ip(ip: &str) {
    unsafe {
        if let Some(detector) = &DETECTOR {
            let mut blocked = detector.blocked_ips.write().unwrap();
            blocked.remove(ip);
        }
    }
}

pub fn get_blocked_ips() -> Vec<String> {
    unsafe {
        if let Some(detector) = &DETECTOR {
            let blocked = detector.blocked_ips.read().unwrap();
            blocked.keys().cloned().collect()
        } else {
            vec![]
        }
    }
}
