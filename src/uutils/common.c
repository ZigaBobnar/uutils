#include "uutils/common.h"

__EXTERN_C_BEGIN

void debug_assert(char* value) {
    (void)value;
}

void debug_crash_shutdown() {}

__EXTERN_C_END
