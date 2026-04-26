// FindFriend 内存池管理模块

#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_POOL_SIZE 1024 * 1024 // 1MB
#define MIN_BLOCK_SIZE 16
#define MAX_BLOCK_SIZE 1024 * 1024 // 1MB

// 内存块结构
typedef struct MemoryBlock {
    struct MemoryBlock* next;
    size_t size;
    bool free;
    uint8_t data[];
} MemoryBlock;

// 内存池结构
typedef struct {
    MemoryBlock* head;
    size_t total_size;
    size_t used_size;
    size_t block_count;
    size_t free_blocks;
} MemoryPool;

typedef struct {
    size_t total_size;
    size_t used_size;
    size_t free_size;
    size_t block_count;
    size_t free_blocks;
    float usage_percentage;
} FFMemoryPoolStats;

static MemoryPool g_memory_pool = {
    .head = NULL,
    .total_size = 0,
    .used_size = 0,
    .block_count = 0,
    .free_blocks = 0
};

static bool g_pool_initialized = false;

FFResult ff_memory_pool_init(size_t pool_size) {
    if (pool_size == 0) {
        pool_size = DEFAULT_POOL_SIZE;
    }
    
    // 分配初始内存池
    void* memory = malloc(pool_size + sizeof(MemoryBlock));
    if (!memory) {
        FF_LOG_ERROR("Failed to allocate memory pool");
        return FF_ERROR_NO_MEMORY;
    }
    
    MemoryBlock* block = (MemoryBlock*)memory;
    block->next = NULL;
    block->size = pool_size;
    block->free = true;
    
    g_memory_pool.head = block;
    g_memory_pool.total_size = pool_size;
    g_memory_pool.used_size = 0;
    g_memory_pool.block_count = 1;
    g_memory_pool.free_blocks = 1;
    g_pool_initialized = true;
    
    FF_LOG_INFO("Memory pool initialized with size: %zu bytes", pool_size);
    return FF_OK;
}

void ff_memory_pool_cleanup(void) {
    MemoryBlock* block = g_memory_pool.head;
    while (block) {
        MemoryBlock* next = block->next;
        free(block);
        block = next;
    }
    
    g_memory_pool.head = NULL;
    g_memory_pool.total_size = 0;
    g_memory_pool.used_size = 0;
    g_memory_pool.block_count = 0;
    g_memory_pool.free_blocks = 0;
    g_pool_initialized = false;
    FF_LOG_INFO("Memory pool cleanup");
}

void* ff_memory_pool_alloc(size_t size) {
    if (!g_pool_initialized) {
        // 自动初始化
        if (ff_memory_pool_init(DEFAULT_POOL_SIZE) != FF_OK) {
            return NULL;
        }
    }
    
    if (size == 0) {
        return NULL;
    }
    
    // 对齐到最小块大小
    size = (size + MIN_BLOCK_SIZE - 1) & ~(MIN_BLOCK_SIZE - 1);
    if (size > MAX_BLOCK_SIZE) {
        // 对于大内存，直接使用 malloc
        void* memory = malloc(size);
        if (memory) {
            g_memory_pool.used_size += size;
        }
        return memory;
    }
    
    // 查找合适的空闲块
    MemoryBlock* block = g_memory_pool.head;
    MemoryBlock* prev = NULL;
    
    while (block) {
        if (block->free && block->size >= size) {
            // 检查是否需要分割块
            if (block->size >= size + sizeof(MemoryBlock) + MIN_BLOCK_SIZE) {
                // 分割块
                MemoryBlock* new_block = (MemoryBlock*)((uint8_t*)block + sizeof(MemoryBlock) + size);
                new_block->next = block->next;
                new_block->size = block->size - size - sizeof(MemoryBlock);
                new_block->free = true;
                
                block->next = new_block;
                block->size = size;
                g_memory_pool.block_count++;
                g_memory_pool.free_blocks++;
            }
            
            // 标记块为使用中
            block->free = false;
            g_memory_pool.free_blocks--;
            g_memory_pool.used_size += block->size;
            
            return block->data;
        }
        
        prev = block;
        block = block->next;
    }
    
    // 没有合适的块，分配新的内存
    size_t alloc_size = size > DEFAULT_POOL_SIZE ? size : DEFAULT_POOL_SIZE;
    void* memory = malloc(alloc_size + sizeof(MemoryBlock));
    if (!memory) {
        return NULL;
    }
    
    MemoryBlock* new_block = (MemoryBlock*)memory;
    new_block->next = NULL;
    new_block->size = alloc_size;
    new_block->free = false;
    
    if (prev) {
        prev->next = new_block;
    } else {
        g_memory_pool.head = new_block;
    }
    
    g_memory_pool.total_size += alloc_size;
    g_memory_pool.used_size += alloc_size;
    g_memory_pool.block_count++;
    
    return new_block->data;
}

void ff_memory_pool_free(void* ptr) {
    if (!ptr || !g_pool_initialized) {
        return;
    }
    
    // 找到对应的内存块
    MemoryBlock* block = (MemoryBlock*)((uint8_t*)ptr - sizeof(MemoryBlock));
    
    // 标记块为空闲
    if (!block->free) {
        block->free = true;
        g_memory_pool.free_blocks++;
        g_memory_pool.used_size -= block->size;
    }
    
    // 尝试合并相邻的空闲块
    MemoryBlock* current = g_memory_pool.head;
    while (current) {
        if (current->free) {
            MemoryBlock* next = current->next;
            while (next && next->free) {
                // 合并块
                current->size += sizeof(MemoryBlock) + next->size;
                current->next = next->next;
                
                g_memory_pool.block_count--;
                g_memory_pool.free_blocks--;
                
                free(next);
                next = current->next;
            }
        }
        current = current->next;
    }
}

void* ff_memory_pool_realloc(void* ptr, size_t new_size) {
    if (!ptr) {
        return ff_memory_pool_alloc(new_size);
    }
    
    if (new_size == 0) {
        ff_memory_pool_free(ptr);
        return NULL;
    }
    
    // 找到对应的内存块
    MemoryBlock* block = (MemoryBlock*)((uint8_t*)ptr - sizeof(MemoryBlock));
    
    // 对齐到最小块大小
    new_size = (new_size + MIN_BLOCK_SIZE - 1) & ~(MIN_BLOCK_SIZE - 1);
    
    if (block->size >= new_size) {
        // 不需要重新分配
        return ptr;
    }
    
    // 分配新内存
    void* new_ptr = ff_memory_pool_alloc(new_size);
    if (!new_ptr) {
        return NULL;
    }
    
    // 复制数据
    memcpy(new_ptr, ptr, block->size);
    
    // 释放旧内存
    ff_memory_pool_free(ptr);
    
    return new_ptr;
}

FFResult ff_memory_pool_get_stats(FFMemoryPoolStats* stats) {
    if (!stats) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    stats->total_size = g_memory_pool.total_size;
    stats->used_size = g_memory_pool.used_size;
    stats->free_size = g_memory_pool.total_size - g_memory_pool.used_size;
    stats->block_count = g_memory_pool.block_count;
    stats->free_blocks = g_memory_pool.free_blocks;
    stats->usage_percentage = g_memory_pool.total_size > 0 ? 
        (float)g_memory_pool.used_size / g_memory_pool.total_size * 100.0 : 0.0;
    
    return FF_OK;
}

void ff_memory_pool_dump(void) {
    MemoryBlock* block = g_memory_pool.head;
    size_t index = 0;
    
    FF_LOG_INFO("Memory pool dump:");
    FF_LOG_INFO("Total size: %zu bytes", g_memory_pool.total_size);
    FF_LOG_INFO("Used size: %zu bytes", g_memory_pool.used_size);
    FF_LOG_INFO("Free size: %zu bytes", g_memory_pool.total_size - g_memory_pool.used_size);
    FF_LOG_INFO("Block count: %zu", g_memory_pool.block_count);
    FF_LOG_INFO("Free blocks: %zu", g_memory_pool.free_blocks);
    FF_LOG_INFO("Usage: %.2f%%", g_memory_pool.total_size > 0 ? 
        (float)g_memory_pool.used_size / g_memory_pool.total_size * 100.0 : 0.0);
    
    while (block) {
        FF_LOG_INFO("Block %zu: size=%zu, free=%s, address=%p", 
                index, block->size, block->free ? "yes" : "no", block);
        block = block->next;
        index++;
    }
}

// 替代标准内存分配函数
void* ff_malloc(size_t size) {
    return ff_memory_pool_alloc(size);
}

void ff_free(void* ptr) {
    ff_memory_pool_free(ptr);
}

void* ff_realloc(void* ptr, size_t size) {
    return ff_memory_pool_realloc(ptr, size);
}

void* ff_calloc(size_t count, size_t size) {
    void* ptr = ff_memory_pool_alloc(count * size);
    if (ptr) {
        memset(ptr, 0, count * size);
    }
    return ptr;
}
