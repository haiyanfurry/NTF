// FindFriend NetBSD 安全接口实现

#include "platform_specific.h"
#include "../../../common_core/interface/ff_core_interface.h"
#include "../../../security_rust/ffi/ff_security_ffi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

static bool g_security_initialized = false;

FFResult platform_security_init(void) {
    if (g_security_initialized) {
        return FF_OK;
    }

    FFResult result = ff_security_init();
    if (result == FF_OK) {
        g_security_initialized = true;
    }

    return result;
}

void platform_security_cleanup(void) {
    if (g_security_initialized) {
        ff_security_cleanup();
        g_security_initialized = false;
    }
}

bool platform_is_rooted(void) {
    if (!g_security_initialized) {
        return false;
    }
    return ff_security_is_rooted();
}

bool platform_check_integrity(void) {
    if (!g_security_initialized) {
        return false;
    }
    return ff_security_boot_check();
}

bool platform_detect_injection(void) {
    if (!g_security_initialized) {
        return false;
    }
    return ff_security_check_injection();
}

FFResult platform_encrypt_data(const uint8_t* input, size_t input_len,
                               uint8_t* output, size_t* output_len,
                               const uint8_t* key, size_t key_len) {
    if (!g_security_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }

    void* encrypted = ff_security_encrypt_memory(input, input_len, key, key_len);
    if (!encrypted) {
        return FF_ERROR;
    }

    memcpy(output, encrypted, input_len);
    *output_len = input_len;
    free(encrypted);

    return FF_OK;
}

FFResult platform_decrypt_data(const uint8_t* input, size_t input_len,
                               uint8_t* output, size_t* output_len,
                               const uint8_t* key, size_t key_len) {
    if (!g_security_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }

    void* decrypted = ff_security_decrypt_memory(input, input_len, key, key_len);
    if (!decrypted) {
        return FF_ERROR;
    }

    memcpy(output, decrypted, input_len);
    *output_len = input_len;
    free(decrypted);

    return FF_OK;
}

FFResult platform_get_device_fingerprint(uint8_t* fingerprint, size_t* fingerprint_len) {
    if (!g_security_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }

    FFResult result = ff_security_get_device_fingerprint(fingerprint, fingerprint_len);
    return result;
}

bool platform_verify_certificate(const uint8_t* cert_data, size_t cert_len) {
    if (!g_security_initialized) {
        return false;
    }
    return ff_security_verify_certificate(cert_data, cert_len);
}

bool platform_check_mitm(void) {
    if (!g_security_initialized) {
        return false;
    }
    return ff_security_check_mitm();
}
