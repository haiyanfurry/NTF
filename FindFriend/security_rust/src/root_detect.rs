//! Root 检测模块
//! 检测设备是否被 Root 或 Jailbreak

use std::fs;
use std::path::Path;
use log::{debug, warn};

pub fn init() -> Result<(), Box<dyn std::error::Error>> {
    // 初始化
    Ok(())
}

pub fn shutdown() {
    // 清理资源
}

pub fn is_rooted() -> bool {
    // 检查常见的 Root 迹象
    if check_su_binary() || 
       check_superuser_dir() || 
       check_magisk() || 
       check_root_files() || 
       check_env_vars() {
        return true;
    }
    
    false
}

fn check_su_binary() -> bool {
    let su_paths = [
        "/system/bin/su",
        "/system/xbin/su",
        "/sbin/su",
        "/system/su",
        "/su/bin/su",
        "/data/local/bin/su",
        "/data/local/xbin/su"
    ];
    
    for path in &su_paths {
        if Path::new(path).exists() {
            warn!("[FF-Security] Root detected: su binary found at {}", path);
            return true;
        }
    }
    
    false
}

fn check_superuser_dir() -> bool {
    let superuser_dirs = [
        "/system/app/Superuser",
        "/system/priv-app/SuperSU",
        "/system/app/MagiskManager",
        "/data/app/topjohnwu.magisk",
        "/data/app/com.noshufou.android.su"
    ];
    
    for path in &superuser_dirs {
        if Path::new(path).exists() {
            warn!("[FF-Security] Root detected: superuser directory found at {}", path);
            return true;
        }
    }
    
    false
}

fn check_magisk() -> bool {
    let magisk_paths = [
        "/sbin/.magisk",
        "/system/.magisk",
        "/magisk",
        "/cache/magisk",
        "/data/magisk"
    ];
    
    for path in &magisk_paths {
        if Path::new(path).exists() {
            warn!("[FF-Security] Root detected: Magisk found at {}", path);
            return true;
        }
    }
    
    false
}

fn check_root_files() -> bool {
    let root_files = [
        "/etc/.installed_su_daemon",
        "/etc/.has_su_daemon",
        "/etc/su.d",
        "/system/etc/su.d",
        "/system/etc/.installed_su_daemon",
        "/system/etc/.has_su_daemon"
    ];
    
    for path in &root_files {
        if Path::new(path).exists() {
            warn!("[FF-Security] Root detected: root file found at {}", path);
            return true;
        }
    }
    
    false
}

fn check_env_vars() -> bool {
    if let Ok(system_path) = std::env::var("PATH") {
        if system_path.contains("/su/bin") || 
           system_path.contains("/magisk/bin") || 
           system_path.contains("/system/xbin") {
            warn!("[FF-Security] Root detected: suspicious PATH environment variable");
            return true;
        }
    }
    
    false
}

#[cfg(target_os = "ios")]
pub fn is_jailbroken() -> bool {
    let jailbreak_paths = [
        "/Applications/Cydia.app",
        "/Applications/blackra1n.app",
        "/Applications/FakeCarrier.app",
        "/Applications/Icy.app",
        "/Applications/IntelliScreen.app",
        "/Applications/MxTube.app",
        "/Applications/RockApp.app",
        "/Applications/SBSettings.app",
        "/Applications/WinterBoard.app",
        "/Library/MobileSubstrate",
        "/Library/SBSettings",
        "/private/var/lib/cydia",
        "/private/var/lib/apt",
        "/private/var/stash"
    ];
    
    for path in &jailbreak_paths {
        if Path::new(path).exists() {
            warn!("[FF-Security] Jailbreak detected: found at {}", path);
            return true;
        }
    }
    
    false
}

#[cfg(not(target_os = "ios"))]
pub fn is_jailbroken() -> bool {
    false
}
