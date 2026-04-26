//! 反虚假 GPS 模块
//! 检测和防止虚假定位

use std::collections::HashMap;
use std::sync::{RwLock, Once};
use std::time::SystemTime;
use log::{debug, warn};

struct LocationPoint {
    latitude: f64,
    longitude: f64,
    timestamp: SystemTime,
    speed: f64,
}

struct FakeGPSDetector {
    user_locations: RwLock<HashMap<u32, Vec<LocationPoint>>>, // (uid, locations)
    max_speed: f64, // 最大速度（米/秒）
    min_time_between_updates: u32, // 最小更新间隔（秒）
    max_location_history: usize, // 最大位置历史记录数
}

static mut DETECTOR: Option<FakeGPSDetector> = None;
static INIT: Once = Once::new();

pub fn init() -> Result<(), Box<dyn std::error::Error>> {
    INIT.call_once(|| {
        unsafe {
            DETECTOR = Some(FakeGPSDetector {
                user_locations: RwLock::new(HashMap::new()),
                max_speed: 100.0, // 360 km/h，超过这个速度视为虚假
                min_time_between_updates: 5, // 最小 5 秒更新一次
                max_location_history: 10, // 保留最近 10 个位置
            });
        }
    });
    Ok(())
}

pub fn shutdown() {
    // 清理资源
}

pub fn detect_fake_location(lat: f64, lon: f64, speed: f64, last_lat: f64, last_lon: f64, time_diff: u32) -> bool {
    // 检查速度是否合理
    if speed > 300.0 { // 1080 km/h，不可能的速度
        warn!("[FF-Security] Fake GPS detected: unrealistic speed: {} km/h", speed);
        return true;
    }
    
    // 检查位置变化是否合理
    let distance = calculate_distance(last_lat, last_lon, lat, lon);
    let max_distance = speed * time_diff as f64 * 1000.0 / 3600.0; // 转换为米
    
    if distance > max_distance * 1.5 { // 允许 50% 的误差
        warn!("[FF-Security] Fake GPS detected: unrealistic distance: {} meters in {} seconds", distance, time_diff);
        return true;
    }
    
    // 检查坐标是否有效
    if !is_valid_coordinates(lat, lon) {
        warn!("[FF-Security] Fake GPS detected: invalid coordinates: {}, {}", lat, lon);
        return true;
    }
    
    false
}

fn calculate_distance(lat1: f64, lon1: f64, lat2: f64, lon2: f64) -> f64 {
    const R: f64 = 6371000.0; // 地球半径（米）
    
    let lat1_rad = lat1.to_radians();
    let lon1_rad = lon1.to_radians();
    let lat2_rad = lat2.to_radians();
    let lon2_rad = lon2.to_radians();
    
    let dlat = lat2_rad - lat1_rad;
    let dlon = lon2_rad - lon1_rad;
    
    let a = (dlat / 2.0).sin().powi(2) + 
             lat1_rad.cos() * lat2_rad.cos() * 
             (dlon / 2.0).sin().powi(2);
    let c = 2.0 * a.sqrt().asin();
    
    R * c
}

fn is_valid_coordinates(lat: f64, lon: f64) -> bool {
    lat >= -90.0 && lat <= 90.0 && lon >= -180.0 && lon <= 180.0
}

pub fn add_location(uid: u32, lat: f64, lon: f64, speed: f64) -> bool {
    unsafe {
        if let Some(detector) = &DETECTOR {
            let mut locations = detector.user_locations.write().unwrap();
            
            let now = SystemTime::now();
            let new_point = LocationPoint {
                latitude: lat,
                longitude: lon,
                timestamp: now,
                speed: speed,
            };
            
            if let Some(points) = locations.get_mut(&uid) {
                // 检查时间间隔
                if let Some(last_point) = points.last() {
                    if let Ok(seconds) = now.duration_since(last_point.timestamp).as_secs() {
                        if seconds < detector.min_time_between_updates as u64 {
                            warn!("[FF-Security] Fake GPS detected: too frequent updates");
                            return true;
                        }
                    }
                }
                
                // 检查速度
                if let Some(last_point) = points.last() {
                    if let Ok(seconds) = now.duration_since(last_point.timestamp).as_secs() {
                        if seconds > 0 {
                            let distance = calculate_distance(
                                last_point.latitude,
                                last_point.longitude,
                                lat,
                                lon
                            );
                            let speed_mps = distance / seconds as f64;
                            if speed_mps > detector.max_speed {
                                warn!("[FF-Security] Fake GPS detected: speed too high: {} m/s", speed_mps);
                                return true;
                            }
                        }
                    }
                }
                
                points.push(new_point);
                // 保持历史记录数量
                if points.len() > detector.max_location_history {
                    points.drain(0..(points.len() - detector.max_location_history));
                }
            } else {
                locations.insert(uid, vec![new_point]);
            }
        }
    }
    
    false
}

pub fn get_user_locations(uid: u32) -> Option<Vec<LocationPoint>> {
    unsafe {
        if let Some(detector) = &DETECTOR {
            let locations = detector.user_locations.read().unwrap();
            locations.get(&uid).cloned()
        } else {
            None
        }
    }
}

pub fn clear_user_locations(uid: u32) {
    unsafe {
        if let Some(detector) = &DETECTOR {
            let mut locations = detector.user_locations.write().unwrap();
            locations.remove(&uid);
        }
    }
}
