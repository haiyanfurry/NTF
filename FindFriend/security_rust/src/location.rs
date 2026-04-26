//! 位置模糊化模块
//! 提供位置隐私保护功能

use std::f64::consts::PI;
use rand::RngCore;
use rand_chacha::ChaCha20Rng;
use thiserror::Error;

#[derive(Error, Debug)]
pub enum LocationError {
    #[error("Invalid coordinates")]
    InvalidCoordinates,
    #[error("Random generation failed")]
    RandomError,
}

static mut RNG: Option<ChaCha20Rng> = None;

const EARTH_RADIUS_METERS: f64 = 6371000.0;

pub fn init() {
    unsafe {
        if RNG.is_none() {
            RNG = Some(ChaCha20Rng::from_entropy());
        }
    }
}

pub fn obfuscate(lat: f64, lon: f64, radius_meters: f64) -> Result<(f64, f64), LocationError> {
    if lat < -90.0 || lat > 90.0 || lon < -180.0 || lon > 180.0 {
        return Err(LocationError::InvalidCoordinates);
    }
    
    if radius_meters <= 0.0 || radius_meters > 50000.0 {
        return Err(LocationError::InvalidCoordinates);
    }
    
    let mut angle = 0.0;
    let mut distance = 0.0;
    
    unsafe {
        if let Some(ref mut rng) = RNG {
            let mut bytes = [0u8; 16];
            rng.fill_bytes(&mut bytes);
            
            angle = ((bytes[0] as f64) / 256.0) * 2.0 * PI;
            distance = radius_meters * (((bytes[1] as f64) / 256.0) * 0.6 + 0.4);
        }
    }
    
    let lat_rad = lat * PI / 180.0;
    let lon_rad = lon * PI / 180.0;
    
    let delta_lat = (distance * f64::cos(angle)) / EARTH_RADIUS_METERS;
    let delta_lon = (distance * f64::sin(angle)) / (EARTH_RADIUS_METERS * f64::cos(lat_rad));
    
    let new_lat = lat + (delta_lat * 180.0 / PI);
    let new_lon = lon + (delta_lon * 180.0 / PI);
    
    let final_lat = new_lat.max(-90.0).min(90.0);
    let final_lon = if new_lon > 180.0 {
        new_lon - 360.0
    } else if new_lon < -180.0 {
        new_lon + 360.0
    } else {
        new_lon
    };
    
    Ok((final_lat, final_lon))
}

pub fn fuzzy_distance(exact_meters: f64) -> f64 {
    if exact_meters < 100.0 {
        return 0.0;
    } else if exact_meters < 500.0 {
        return 100.0 + ((exact_meters - 100.0) * 0.2);
    } else if exact_meters < 1000.0 {
        return 200.0 + ((exact_meters - 500.0) * 0.3);
    } else if exact_meters < 5000.0 {
        return 350.0 + ((exact_meters - 1000.0) * 0.1);
    } else {
        return 750.0 + ((exact_meters - 5000.0) * 0.05);
    }
}

pub fn validate_plausibility(lat: f64, lon: f64, speed: f64, heading: f64) -> bool {
    if lat < -90.0 || lat > 90.0 || lon < -180.0 || lon > 180.0 {
        return false;
    }
    
    if speed < 0.0 || speed > 500.0 {
        return false;
    }
    
    if heading < 0.0 || heading > 360.0 {
        return false;
    }
    
    if speed > 200.0 {
        return false;
    }
    
    true
}
