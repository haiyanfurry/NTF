# NTF
一个由福瑞组成的小工作室的一个仓库

## 最新信息

## 项目状态 / Project Status
所有功能代码已开发完成，**整体未经过严格测试**
All features coded, overall **not fully tested yet**

## 开发语言 / Development Languages
- C99：核心通用逻辑 / Common core logic
- Java：安卓端 / Android platform
- Swift：苹果iOS端 / iOS platform

## 支持平台 / Supported Platforms
- Android 安卓
- iOS 苹果手机
- Windows
- macOS 苹果电脑
- Debian / RedHat / Arch 主流Linux发行版（提供编译二进制）
- Gentoo：仅源码 + Makefile 编译
- LFS：仅源码 + Makefile 编译

- ## 项目结构 / Project Structure
NTF/
├── CHANGELOG.md # 版本更新记录

├── CMakeLists.txt # 顶层CMake构建配置

├── CODEOWNERS # 代码所有者配置

├── CONTRIBUTING.md # 贡献指南

├── LICENSE # 开源许可证

├── README.md # 项目说明

├── SECURITY.md # 安全政策与漏洞报告

│

├── common_core/ # 核心通用逻辑（C99）

│ ├── bridge/ # 平台桥接层

│ ├── business/ # 业务逻辑模块（用户、消息、交易、展览等）

│ ├── game/ # 游戏模块

│ ├── model/ # 数据模型与数据库操作

│ ├── network/ # 网络通信基础层

│ ├── security/ # 安全模块（Root检测、完整性校验、内存加密等）

│ ├── tls/ # TLS加密与中间人防护

│ ├── util/ # 通用工具函数

│ └── version/ # 版本信息

│

├── common_ui/ # 通用UI组件（C99）

│ ├── chat/ # 聊天窗口组件

│ ├── friend_list/ # 好友列表组件

│ ├── login/ # 登录窗口组件

│ ├── main/ # 主窗口（客户端/商家/管理端）

│ └── map/ # 地图视图组件

│

├── platform_android/ # Android平台（Java）

│ └── app/src/main/java/ # Android原生代码

│

├── platform_ios/ # iOS平台（Swift）

│ └── classes/ # iOS原生代码

│

├── platform_macos/ # macOS平台（Swift/C）

│

├── platform_windows/ # Windows平台（C）

│

├── platform_linux_debian/ # Debian/Ubuntu（C）

├── platform_linux_redhat/ # RedHat/Fedora（C）

├── platform_linux_arch/ # Arch Linux（C）

├── platform_linux_gentoo/ # Gentoo（仅源码+Makefile）

├── platform_linux_lfs/ # LFS（仅源码+Makefile）

│

└── resources/ # 资源文件

├── layout/ # Android布局文件

└── map_template.html # 地图网页模板

## 开发维护 / Developers & Maintainers
个人维护：haiyanfurry
协作团队：霓拓锋team / NTF-ZEYZ

## 漏洞反馈 / Security Vulnerability Report
如有安全漏洞或BUG，请邮件提交：
If you find security bugs, please contact via email:

[2752842448@qq.com]

[Lu19275627399@outlook.com]


## 为什么要做仓库：

    两个初中生半夜无聊创建的仓库然后觉得可以写点代码

    目前我请假在家我初二然后另一个初三在学校所以到现在都是我在拿着ai搓QwQ

    简介就说到这

    2026.3.31

    haiyan

    （那个没本事只能靠ai的我）

## 和朋友的日常：
怎么说呢我和那个合作的他就是我朋友

他天天上学都不理我QwQ...

所以现在又有春假了所以他应该可以在这段时间多和我聊会天吧

那时候他看到那个兽频道所以他也想做但是他一直拉着我让我给她做但是我不会啊所以只能找ai惹这个项目其实就是ai项目

