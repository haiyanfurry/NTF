# Building / 编译说明
## Compile Standard / 编译规范
- Main code: C99 + CMake
  核心代码采用 C99 标准，基于 CMake 构建
- Android: Gradle
- iOS: Swift + Podfile

## Platform Rule / 平台编译规则
1. Android / iOS / Windows / macOS / Debian / RedHat / Arch
   Compiled binary output / 提供编译好的可执行程序
2. Debian: with exclusive graphical debug terminal panel
   Debian额外附带宝塔风格图形化调试运维端
3. Gentoo / LFS
   Only source code + Makefile (no CMake)
   仅提供源码与原生Makefile，不依赖CMake

## Test Status / 测试状态
Compile available, function not fully tested.
可正常编译运行，功能未完成全量测试。
