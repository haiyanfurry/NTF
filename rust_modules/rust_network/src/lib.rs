//! FindFriend Rust 安全网络模块
//! 提供安全的 TCP/UDP/P2P 通信，带 FFI 接口给 C 调用

use std::sync::{Arc, Mutex, RwLock};
use std::sync::atomic::{AtomicBool, Ordering};
use std::collections::HashMap;
use std::ptr;
use std::ffi::{CStr, CString};
use libc::{c_char, c_int, c_void, size_t};
use log::{info, warn, error, debug};

// ====================================================================
// 错误码定义 (与 C 统一)
// ====================================================================
#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FFErrorCode {
    Ok = 0,
    InvalidParam = -1,
    NetworkError = -2,
    Timeout = -3,
    PermissionDenied = -4,
    MemoryError = -5,
    AlreadyInitialized = -6,
    NotInitialized = -7,
    EncryptionError = -8,
    ConnectionClosed = -9,
    BufferTooSmall = -10,
    UnknownError = -99,
}

// ====================================================================
// 网络配置
// ====================================================================
#[repr(C)]
#[derive(Debug, Clone)]
pub struct FFNetConfig {
    pub host: *const c_char,
    pub port: u16,
    pub use_tls: bool,
    pub timeout_ms: u32,
    pub buffer_size: size_t,
    pub enable_encryption: bool,
    pub peer_id: *const c_char,
}

impl Default for FFNetConfig {
    fn default() -> Self {
        FFNetConfig {
            host: ptr::null(),
            port: 8080,
            use_tls: false,
            timeout_ms: 5000,
            buffer_size: 4096,
            enable_encryption: false,
            peer_id: ptr::null(),
        }
    }
}

// ====================================================================
// 网络模块管理器
// ====================================================================
struct NetworkManager {
    initialized: bool,
    connections: RwLock<HashMap<u64, Arc<Connection>>>,
    peer_map: RwLock<HashMap<String, u64>>,
    next_conn_id: Mutex<u64>,
    config: RwLock<FFNetConfig>,
}

impl NetworkManager {
    fn new() -> Self {
        NetworkManager {
            initialized: false,
            connections: RwLock::new(HashMap::new()),
            peer_map: RwLock::new(HashMap::new()),
            next_conn_id: Mutex::new(1),
            config: RwLock::new(FFNetConfig::default()),
        }
    }
}

struct Connection {
    id: u64,
    peer_id: String,
    is_active: AtomicBool,
    buffer: Vec<u8>,
}

impl Connection {
    fn new(id: u64, peer_id: &str) -> Self {
        Connection {
            id,
            peer_id: peer_id.to_string(),
            is_active: AtomicBool::new(true),
            buffer: Vec::with_capacity(4096),
        }
    }
}

// 全局单例
static mut NETWORK_MANAGER: Option<Arc<NetworkManager>> = None;

// ====================================================================
// FFI 接口 - 核心网络功能
// ====================================================================

#[no_mangle]
pub extern "C" fn rust_network_init(config: *const FFNetConfig) -> FFErrorCode {
    unsafe {
        if NETWORK_MANAGER.is_some() {
            return FFErrorCode::AlreadyInitialized;
        }

        let mut manager = NetworkManager::new();

        if !config.is_null() {
            *manager.config.write().unwrap() = (*config).clone();
        }

        info!("[Rust-Network] Initializing network module...");
        
        NETWORK_MANAGER = Some(Arc::new(manager));
        
        info!("[Rust-Network] Network module initialized successfully");
        FFErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_network_destroy() -> FFErrorCode {
    unsafe {
        if NETWORK_MANAGER.is_none() {
            return FFErrorCode::NotInitialized;
        }

        info!("[Rust-Network] Destroying network module...");

        if let Some(manager) = &NETWORK_MANAGER {
            let conns = manager.connections.read().unwrap();
            for (_, conn) in conns.iter() {
                conn.is_active.store(false, Ordering::SeqCst);
            }
            drop(conns);
            
            let mut conns = manager.connections.write().unwrap();
            conns.clear();
            
            manager.peer_map.write().unwrap().clear();
        }

        NETWORK_MANAGER = None;
        info!("[Rust-Network] Network module destroyed");
        
        FFErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_send_secure(
    conn_id: u64,
    data: *const u8,
    data_len: size_t,
    sent: *mut size_t
) -> FFErrorCode {
    unsafe {
        if NETWORK_MANAGER.is_none() {
            return FFErrorCode::NotInitialized;
        }
        
        if data.is_null() || sent.is_null() {
            return FFErrorCode::InvalidParam;
        }

        let manager = NETWORK_MANAGER.as_ref().unwrap();
        let conns = manager.connections.read().unwrap();

        if let Some(conn) = conns.get(&conn_id) {
            if !conn.is_active.load(Ordering::SeqCst) {
                return FFErrorCode::ConnectionClosed;
            }

            // 在实际实现中这里会有加密和真实网络发送
            // 现在是模拟实现
            debug!("[Rust-Network] Sending {} bytes securely to conn {}", data_len, conn_id);
            
            *sent = data_len;
            FFErrorCode::Ok
        } else {
            FFErrorCode::InvalidParam
        }
    }
}

#[no_mangle]
pub extern "C" fn rust_receive_secure(
    conn_id: u64,
    buffer: *mut u8,
    buffer_size: size_t,
    received: *mut size_t
) -> FFErrorCode {
    unsafe {
        if NETWORK_MANAGER.is_none() {
            return FFErrorCode::NotInitialized;
        }

        if buffer.is_null() || received.is_null() {
            return FFErrorCode::InvalidParam;
        }

        // 模拟接收
        *received = 0;
        
        debug!("[Rust-Network] Receiving from conn {}...", conn_id);
        FFErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_connect_peer(
    peer_id: *const c_char,
    host: *const c_char,
    port: u16,
    conn_id: *mut u64
) -> FFErrorCode {
    unsafe {
        if NETWORK_MANAGER.is_none() {
            return FFErrorCode::NotInitialized;
        }

        if peer_id.is_null() || host.is_null() || conn_id.is_null() {
            return FFErrorCode::InvalidParam;
        }

        let peer_id_str = CStr::from_ptr(peer_id).to_str().unwrap_or("unknown");
        let host_str = CStr::from_ptr(host).to_str().unwrap_or("localhost");
        
        info!("[Rust-Network] Connecting to peer {} at {}:{}", peer_id_str, host_str, port);

        let manager = NETWORK_MANAGER.as_ref().unwrap();
        let mut next_id = manager.next_conn_id.lock().unwrap();
        let new_id = *next_id;
        *next_id += 1;

        let new_conn = Arc::new(Connection::new(new_id, peer_id_str));
        
        let mut conns = manager.connections.write().unwrap();
        conns.insert(new_id, new_conn);

        let mut peer_map = manager.peer_map.write().unwrap();
        peer_map.insert(peer_id_str.to_string(), new_id);
        
        *conn_id = new_id;
        
        info!("[Rust-Network] Connected to peer {}, conn_id = {}", peer_id_str, new_id);

        FFErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_disconnect_peer(conn_id: u64) -> FFErrorCode {
    unsafe {
        if NETWORK_MANAGER.is_none() {
            return FFErrorCode::NotInitialized;
        }

        info!("[Rust-Network] Disconnecting conn_id {}", conn_id);
        
        let manager = NETWORK_MANAGER.as_ref().unwrap();
        let conns = manager.connections.read().unwrap();
        
        if let Some(conn) = conns.get(&conn_id) {
            conn.is_active.store(false, Ordering::SeqCst);
        }
        
        FFErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_udp_send(
    dest_addr: *const c_char,
    dest_port: u16,
    data: *const u8,
    data_len: size_t
) -> FFErrorCode {
    unsafe {
        if data.is_null() || dest_addr.is_null() {
            return FFErrorCode::InvalidParam;
        }
        
        let addr = CStr::from_ptr(dest_addr).to_str().unwrap_or("");
        debug!("[Rust-Network] UDP sending {} bytes to {}:{}", data_len, addr, dest_port);
        
        FFErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_p2p_discover() -> FFErrorCode {
    unsafe {
        if NETWORK_MANAGER.is_none() {
            return FFErrorCode::NotInitialized;
        }

        info!("[Rust-Network] Starting P2P discovery...");
        
        // 这里实现实际的 P2P 节点发现
        // 使用 mDNS / UDP 广播 / DHT
        
        FFErrorCode::Ok
    }
}

#[no_mangle]
pub extern "C" fn rust_network_is_initialized() -> bool {
    unsafe {
        NETWORK_MANAGER.is_some()
    }
}

#[no_mangle]
pub extern "C" fn rust_get_error_message(error_code: FFErrorCode) -> *const c_char {
    static MSG_OK: &str = "Operation successful";
    static MSG_INVALID_PARAM: &str = "Invalid parameter";
    static MSG_NET_ERROR: &str = "Network error occurred";
    static MSG_TIMEOUT: &str = "Operation timed out";
    static MSG_PERMISSION: &str = "Permission denied";
    static MSG_MEMORY: &str = "Memory allocation error";
    static MSG_ALREADY_INIT: &str = "Already initialized";
    static MSG_NOT_INIT: &str = "Not initialized";
    static MSG_ENCRYPTION: &str = "Encryption error";
    static MSG_CONN_CLOSED: &str = "Connection closed";
    static MSG_BUFFER_SMALL: &str = "Buffer too small";
    static MSG_UNKNOWN: &str = "Unknown error";
    
    static mut C_MSG_OK: *mut c_char = ptr::null_mut();
    static mut C_MSG_INVALID: *mut c_char = ptr::null_mut();
    static mut C_MSG_NET: *mut c_char = ptr::null_mut();
    static mut C_MSG_TIMEOUT: *mut c_char = ptr::null_mut();
    static mut C_MSG_PERM: *mut c_char = ptr::null_mut();
    static mut C_MSG_MEM: *mut c_char = ptr::null_mut();
    static mut C_MSG_ALREADY: *mut c_char = ptr::null_mut();
    static mut C_MSG_NOT_INIT: *mut c_char = ptr::null_mut();
    static mut C_MSG_ENCRYPT: *mut c_char = ptr::null_mut();
    static mut C_MSG_CONN_CLOSED: *mut c_char = ptr::null_mut();
    static mut C_MSG_BUFFER_SMALL: *mut c_char = ptr::null_mut();
    static mut C_MSG_UNKNOWN: *mut c_char = ptr::null_mut();
    
    unsafe {
        match error_code {
            FFErrorCode::Ok => {
                if C_MSG_OK.is_null() {
                    C_MSG_OK = CString::new(MSG_OK).unwrap().into_raw();
                }
                C_MSG_OK
            },
            FFErrorCode::InvalidParam => {
                if C_MSG_INVALID.is_null() {
                    C_MSG_INVALID = CString::new(MSG_INVALID_PARAM).unwrap().into_raw();
                }
                C_MSG_INVALID
            },
            FFErrorCode::NetworkError => {
                if C_MSG_NET.is_null() {
                    C_MSG_NET = CString::new(MSG_NET_ERROR).unwrap().into_raw();
                }
                C_MSG_NET
            },
            FFErrorCode::Timeout => {
                if C_MSG_TIMEOUT.is_null() {
                    C_MSG_TIMEOUT = CString::new(MSG_TIMEOUT).unwrap().into_raw();
                }
                C_MSG_TIMEOUT
            },
            FFErrorCode::PermissionDenied => {
                if C_MSG_PERM.is_null() {
                    C_MSG_PERM = CString::new(MSG_PERMISSION).unwrap().into_raw();
                }
                C_MSG_PERM
            },
            FFErrorCode::MemoryError => {
                if C_MSG_MEM.is_null() {
                    C_MSG_MEM = CString::new(MSG_MEMORY).unwrap().into_raw();
                }
                C_MSG_MEM
            },
            FFErrorCode::AlreadyInitialized => {
                if C_MSG_ALREADY.is_null() {
                    C_MSG_ALREADY = CString::new(MSG_ALREADY_INIT).unwrap().into_raw();
                }
                C_MSG_ALREADY
            },
            FFErrorCode::NotInitialized => {
                if C_MSG_NOT_INIT.is_null() {
                    C_MSG_NOT_INIT = CString::new(MSG_NOT_INIT).unwrap().into_raw();
                }
                C_MSG_NOT_INIT
            },
            FFErrorCode::EncryptionError => {
                if C_MSG_ENCRYPT.is_null() {
                    C_MSG_ENCRYPT = CString::new(MSG_ENCRYPTION).unwrap().into_raw();
                }
                C_MSG_ENCRYPT
            },
            FFErrorCode::ConnectionClosed => {
                if C_MSG_CONN_CLOSED.is_null() {
                    C_MSG_CONN_CLOSED = CString::new(MSG_CONN_CLOSED).unwrap().into_raw();
                }
                C_MSG_CONN_CLOSED
            },
            FFErrorCode::BufferTooSmall => {
                if C_MSG_BUFFER_SMALL.is_null() {
                    C_MSG_BUFFER_SMALL = CString::new(MSG_BUFFER_SMALL).unwrap().into_raw();
                }
                C_MSG_BUFFER_SMALL
            },
            FFErrorCode::UnknownError => {
                if C_MSG_UNKNOWN.is_null() {
                    C_MSG_UNKNOWN = CString::new(MSG_UNKNOWN).unwrap().into_raw();
                }
                C_MSG_UNKNOWN
            },
        }
    }
}
