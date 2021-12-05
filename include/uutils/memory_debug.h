#ifndef UUTILS_MEMORY_DEBUG_H_
#define UUTILS_MEMORY_DEBUG_H_

#include <stdlib.h>
#include "uutils/common.h"

#define malloc(size) debug_malloc(size, __FILE__, __LINE__)
#define calloc(count, size) debug_calloc(count, size, __FILE__, __LINE__)
#define free(pointer) debug_free(pointer)

__EXTERN_C_BEGIN

/**
 * Allocate memory.
 */
void *debug_malloc(size_t size, char *filename, size_t line);

/**
 * Allocate memory and set the values to zero memory.
 */
void *debug_calloc(size_t count, size_t size, char *filename, size_t line);

/**
 * Deallocate memory.
 */
void debug_free(void *ptr);

/**
 * Show memory leakage report.
 */
void memory_debug_print_report();

__EXTERN_C_END

#endif  // UUTILS_MEMORY_DEBUG_H_
