#ifndef UUTILS_COMMON_H_
#define UUTILS_COMMON_H_

#ifndef FAKE_HARDWARE
#define FAKE_HARDWARE 0
#endif

#if FAKE_HARDWARE
#define REAL_HARDWARE 0
#else
#define REAL_HARDWARE 1
#endif

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#if REAL_HARDWARE
#endif

#if FAKE_HARDWARE
#include <stdio.h>
#endif

void debug_assert(char* value);
void debug_crash_shutdown(void);

#ifdef __cplusplus
#define __EXTERN_C_BEGIN extern "C" {
#define __EXTERN_C_END }
#else
#define __EXTERN_C_BEGIN
#define __EXTERN_C_END
#endif

#endif  // UUTILS_COMMON_H_
