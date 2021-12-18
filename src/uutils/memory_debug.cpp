#include "uutils/memory_debug.h"
#include <stdio.h>
#include <list>
#include <vector>

// Prevent existing debug macros from being used
#undef malloc
#undef calloc
#undef free

size_t total_allocated = 0;

class memory_node {
public:
    void* ptr;
    size_t size;
    size_t line;
    char* filename;
};

std::vector<memory_node> nodes;

memory_node* head = NULL;

void memory_node_add(void* ptr, size_t size, size_t line, char* filename) {
    memory_node node = {
        ptr, size, line, filename
    };

    nodes.push_back(node);
    total_allocated += size;
}

void memory_node_remove(void* ptr) {
    for (auto it = nodes.begin(); it != nodes.end(); it++) {
        if (it->ptr == ptr) {
            total_allocated -= it->size;

            nodes.erase(it);
            break;
        }
    }
}

void* debug_malloc(size_t size, char *filename, size_t line) {
    void *ptr = malloc(size);
    memory_node_add(ptr, size, line, filename);
    return ptr;
}

void* debug_calloc(size_t count, size_t size, char *filename, size_t line) {
    void *ptr = calloc(count, size);
    memory_node_add(ptr, size * count, line, filename);
    return ptr;
}

void debug_free(void *ptr) {
    free(ptr);
    memory_node_remove(ptr);
}

bool memory_debug_print_report() {
    if (total_allocated == 0 && nodes.size() == 0) {
        return false;
    }

    puts("\n\n******** Memory leakage report ********");
    printf("=> Failed to free %d bytes total.\n", (int)total_allocated);
    printf("=> %d nodes still allocated:\n", (int)nodes.size());

    for (const auto& it : nodes) {
        printf(" %d bytes | location: %s, line %d\n", (int)it.size, it.filename, (int)it.line);
    }

    printf("\n");

    total_allocated = 0;
    nodes.clear();

    return true;
}
