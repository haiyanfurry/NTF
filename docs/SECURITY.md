# Security / 安全说明

## v5.0 安全架构

### 安全模块组成

| 模块 | 语言 | 说明 | 状态 |
|------|------|------|------|
| 安全验证 | C + Rust | 输入验证、防注入、数据校验 | ✅ 已启用 |
| 加密解密 | Rust | AES-256-GCM、HMAC-SHA256 | ✅ 已启用 |
| 网络安全 | Rust | 安全网络通信 | ✅ 已启用 |
| 反黑产检测 | C | Root 检测、MITM 防护、DoS 防护 | ✅ 已启用 |

### Rust 安全模块

```
rust_modules/
├── rust_security/        # 安全验证模块
│   ├── 输入验证 (邮箱、用户名、密码)
│   ├── 防注入检测 (SQL、XSS、JSON、XML)
│   └── 字符串清理
│
├── rust_crypto/          # 加密模块
│   ├── AES-256-GCM 加密/解密
│   ├── HMAC-SHA256
│   ├── SHA-256/SHA-512
│   └── 安全随机数生成
│
└── rust_network/         # 网络安全模块
    └── 安全网络通信
```

### C 安全模块

```
common_core/security/
├── boot_check/          # 启动检查
├── inject_filter.c      # 注入过滤
├── mitm_protect/        # MITM 防护
├── dos_protect.c        # DoS 防护
├── root_detect.c        # Root 检测
├── device_fingerprint.c  # 设备指纹
├── memory_encrypt.c     # 内存加密
└── xml_protect.c        # XML 保护
```

---

## Vulnerability Report / 漏洞提交

If you find security vulnerabilities or bugs:
如有发现安全漏洞、恶意代码隐患、攻击风险，请邮件反馈：

Contact Email / 联系邮箱：
[填写你的Outlook邮箱 / Fill your Outlook email here]

Maintainers / 维护人员：
haiyanfurry / 霓拓锋team (NTF-ZEYZ)

No bug bounty / 暂无漏洞赏金计划。
Fix will be updated in free time.
相关修复会在开发空闲时间迭代更新。
