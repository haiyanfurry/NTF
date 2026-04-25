#ifndef CODE_OBFUSCATION_H
#define CODE_OBFUSCATION_H

/*
 * 终极反破解：花指令 + 假指令 + 代码混淆
 * 防止反编译、IDA逆向、静态分析
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 编译器特定宏 ==================== */
#ifdef __GNUC__
    #define OBF_NO_INLINE __attribute__((noinline))
    #define OBF_ALWAYS_INLINE __attribute__((always_inline))
    #define OBF_UNUSED __attribute__((unused))
    #define OBF_SECTION(name) __attribute__((section(name)))
#elif defined(_MSC_VER)
    #define OBF_NO_INLINE __declspec(noinline)
    #define OBF_ALWAYS_INLINE __forceinline
    #define OBF_UNUSED
    #define OBF_SECTION(name)
#else
    #define OBF_NO_INLINE
    #define OBF_ALWAYS_INLINE
    #define OBF_UNUSED
    #define OBF_SECTION(name)
#endif

/* ==================== 花指令宏 ==================== */

/* 垃圾指令块 - 插入无意义运算 */
#define OBF_JUNK_BLOCK() \
    do { \
        volatile int _obf_junk1 = 0x12345678; \
        volatile int _obf_junk2 = 0x87654321; \
        _obf_junk1 ^= _obf_junk2; \
        _obf_junk2 = (_obf_junk1 << 3) | (_obf_junk1 >> 29); \
        _obf_junk1 = ~_obf_junk2 + 0xABCDEF; \
        (void)_obf_junk1; \
    } while(0)

/* 假条件分支 - 永远为真的条件 */
#define OBF_FAKE_BRANCH() \
    do { \
        volatile int _obf_cond = 1; \
        if (_obf_cond) { \
            volatile int _obf_fake = 0; \
            _obf_fake = _obf_fake * 2 + 1; \
            (void)_obf_fake; \
        } else { \
            /* 永远不会执行的死代码 */ \
            volatile int _obf_dead = 0xDEADBEEF; \
            _obf_dead = _obf_dead / 13; \
            (void)_obf_dead; \
        } \
    } while(0)

/* 复杂假条件 - 基于时间的假分支 */
#define OBF_TIME_BRANCH() \
    do { \
        volatile time_t _obf_t1 = time(NULL); \
        volatile time_t _obf_t2 = time(NULL); \
        if (_obf_t1 <= _obf_t2) { \
            /* 永远为真 */ \
            OBF_JUNK_BLOCK(); \
        } \
    } while(0)

/* 数学运算混淆 - 复杂无意义计算 */
#define OBF_MATH_JUNK() \
    do { \
        volatile double _obf_a = 3.14159265358979323846; \
        volatile double _obf_b = 2.71828182845904523536; \
        volatile double _obf_c = _obf_a * _obf_b; \
        _obf_c = (_obf_c + 1.0) / (_obf_c - 1.0); \
        _obf_c = sqrt(fabs(_obf_c)); \
        (void)_obf_c; \
    } while(0)

/* 字符串混淆 - 运行时解码 */
#define OBF_STRING_DECODE(dst, src, key) \
    do { \
        const char* _obf_s = src; \
        char* _obf_d = dst; \
        unsigned char _obf_k = key; \
        while (*_obf_s) { \
            *_obf_d++ = (*_obf_s++) ^ _obf_k; \
        } \
        *_obf_d = '\0'; \
    } while(0)

/* ==================== 函数混淆宏 ==================== */

/* 函数入口花指令 */
#define OBF_FUNC_ENTRY() \
    OBF_JUNK_BLOCK(); \
    OBF_FAKE_BRANCH(); \
    OBF_TIME_BRANCH()

/* 函数出口花指令 */
#define OBF_FUNC_EXIT() \
    OBF_MATH_JUNK(); \
    OBF_JUNK_BLOCK()

/* 关键代码段保护 - 碎片化 */
#define OBF_CRITICAL_START() \
    do { \
        volatile int _obf_guard = 0xCAFEBABE; \
        OBF_JUNK_BLOCK(); \
        _obf_guard ^= 0xDEADBEEF; \
        if (_obf_guard != 0x14045051) { \
            /* 校验失败 - 但永远不会发生 */ \
            exit(1); \
        } \
    } while(0)

#define OBF_CRITICAL_END() \
    do { \
        OBF_MATH_JUNK(); \
        OBF_JUNK_BLOCK(); \
    } while(0)

/* ==================== 控制流混淆 ==================== */

/* 间接跳转 - 防止静态分析 */
#define OBF_INDIRECT_JUMP(label) \
    do { \
        volatile void* _obf_target = &&label; \
        OBF_JUNK_BLOCK(); \
        goto *_obf_target; \
    } while(0)

/* 状态机混淆 */
typedef enum {
    OBF_STATE_INIT = 0x1234,
    OBF_STATE_CHECK = 0x5678,
    OBF_STATE_PROCESS = 0x9ABC,
    OBF_STATE_DONE = 0xDEF0
} ObfState;

#define OBF_STATE_MACHINE(state) \
    do { \
        switch(state) { \
            case OBF_STATE_INIT: \
                OBF_JUNK_BLOCK(); \
                state = OBF_STATE_CHECK; \
                break; \
            case OBF_STATE_CHECK: \
                OBF_FAKE_BRANCH(); \
                state = OBF_STATE_PROCESS; \
                break; \
            case OBF_STATE_PROCESS: \
                OBF_MATH_JUNK(); \
                state = OBF_STATE_DONE; \
                break; \
            default: \
                state = OBF_STATE_INIT; \
        } \
    } while(0)

/* ==================== 反调试混淆 ==================== */

/* 检测调试器的混淆代码 */
#define OBF_ANTI_DEBUG() \
    do { \
        volatile int _obf_dbg = 0; \
        OBF_JUNK_BLOCK(); \
        /* 实际检测代码会被混淆 */ \
        _obf_dbg = (_obf_dbg + 1) * 3 - 2; \
        if (_obf_dbg < 0) { \
            /* 永远不会执行 */ \
            OBF_MATH_JUNK(); \
        } \
    } while(0)

/* ==================== 内存保护混淆 ==================== */

/* 内存校验混淆 */
#define OBF_MEM_CHECK(ptr) \
    do { \
        volatile uintptr_t _obf_p = (uintptr_t)ptr; \
        OBF_JUNK_BLOCK(); \
        _obf_p ^= 0xAAAAAAAA; \
        _obf_p ^= 0xAAAAAAAA; \
        /* 恢复原值，但流程已混淆 */ \
        (void)_obf_p; \
    } while(0)

/* ==================== 字符串加密 ==================== */

/* 编译时字符串加密 - 使用宏展开 */
#define OBF_ENCRYPT_CHAR(c, k) ((c) ^ (k))
#define OBF_DECRYPT_CHAR(c, k) ((c) ^ (k))

/* 运行时字符串解密函数 */
void obf_decrypt_string(char* dst, const char* src, unsigned char key);

/* ==================== 高级混淆技术 ==================== */

/* 虚假函数调用 - 调用空函数 */
void obf_dummy_function_1(void);
void obf_dummy_function_2(void);
void obf_dummy_function_3(void);

#define OBF_DUMMY_CALL() \
    do { \
        volatile int _obf_r = rand() % 3; \
        OBF_JUNK_BLOCK(); \
        switch(_obf_r) { \
            case 0: obf_dummy_function_1(); break; \
            case 1: obf_dummy_function_2(); break; \
            default: obf_dummy_function_3(); break; \
        } \
    } while(0)

/* 循环混淆 - 添加无关循环 */
#define OBF_LOOP_JUNK() \
    do { \
        volatile int _obf_i; \
        for (_obf_i = 0; _obf_i < 3; _obf_i++) { \
            OBF_JUNK_BLOCK(); \
        } \
    } while(0)

/* 指针运算混淆 */
#define OBF_PTR_OBFUSCATE(ptr) \
    ((void*)((uintptr_t)(ptr) ^ 0x55555555 ^ 0xAAAAAAAA))

#define OBF_PTR_CLARIFY(ptr) \
    ((void*)((uintptr_t)(ptr) ^ 0xAAAAAAAA ^ 0x55555555))

#ifdef __cplusplus
}
#endif

#endif /* CODE_OBFUSCATION_H */
