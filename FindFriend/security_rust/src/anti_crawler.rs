//! 反爬虫模块
//! 检测和防止爬虫行为

use std::collections::HashMap;
use std::sync::{RwLock, Once};
use std::time::{Duration, SystemTime};
use log::{debug, warn};

struct CrawlerDetector {
    user_queries: RwLock<HashMap<u32, (u32, SystemTime)>>, // (uid, (query_count, last_query_time))
    max_queries: u32,
    time_window: Duration,
}

static mut DETECTOR: Option<CrawlerDetector> = None;
static INIT: Once = Once::new();

pub fn init() -> Result<(), Box<dyn std::error::Error>> {
    INIT.call_once(|| {
        unsafe {
            DETECTOR = Some(CrawlerDetector {
                user_queries: RwLock::new(HashMap::new()),
                max_queries: 100, // 100 queries per 60 seconds
                time_window: Duration::from_secs(60),
            });
        }
    });
    Ok(())
}

pub fn shutdown() {
    // 清理资源
}

pub fn detect_crawler(uid: u32, query_count: u32, time_window: u32) -> bool {
    unsafe {
        if let Some(detector) = &DETECTOR {
            let mut queries = detector.user_queries.write().unwrap();
            
            let now = SystemTime::now();
            let window = Duration::from_secs(time_window as u64);
            
            if let Some((count, last_time)) = queries.get(&uid) {
                if let Ok(elapsed) = now.duration_since(*last_time) {
                    if elapsed < window {
                        let new_count = *count + query_count;
                        if new_count > detector.max_queries {
                            warn!("[FF-Security] Crawler detected: uid={}, queries={}", uid, new_count);
                            queries.insert(uid, (new_count, now));
                            return true;
                        } else {
                            queries.insert(uid, (new_count, now));
                        }
                    } else {
                        // 时间窗口重置
                        queries.insert(uid, (query_count, now));
                    }
                }
            } else {
                queries.insert(uid, (query_count, now));
            }
        }
    }
    
    false
}

pub fn add_query(uid: u32) {
    unsafe {
        if let Some(detector) = &DETECTOR {
            let mut queries = detector.user_queries.write().unwrap();
            let now = SystemTime::now();
            
            if let Some((count, _)) = queries.get(&uid) {
                queries.insert(uid, (*count + 1, now));
            } else {
                queries.insert(uid, (1, now));
            }
        }
    }
}

pub fn reset_user(uid: u32) {
    unsafe {
        if let Some(detector) = &DETECTOR {
            let mut queries = detector.user_queries.write().unwrap();
            queries.remove(&uid);
        }
    }
}

pub fn get_user_queries(uid: u32) -> Option<(u32, SystemTime)> {
    unsafe {
        if let Some(detector) = &DETECTOR {
            let queries = detector.user_queries.read().unwrap();
            queries.get(&uid).cloned()
        } else {
            None
        }
    }
}
