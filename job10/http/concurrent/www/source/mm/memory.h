#ifndef _MM_MEMORY_H
#define _MM_MEMORY_H

#include <utils/list.h>
#include "page.h"
struct pool;
typedef struct pool pool_t;

typedef struct object {
    chain_t chain;
    long long_array[0];
} object_t;

#define BUCKET_MAGIC 0xab2cd61c
typedef struct bucket {
    chain_t chain;
    int magic;
    pool_t *pool;
    int object_count;
    list_t object_list;
    object_t object_array[0];
} bucket_t;

extern void *bucket_alloc_object(bucket_t *this);
extern void bucket_free_object(bucket_t *this, object_t *object);

#define POOL_MAGIC 0x32ac76a3
struct pool {
    int magic;
    int object_size;
    list_t free_bucket_list; 
    list_t busy_bucket_list;
};

extern void pool_init(pool_t *this, int object_size);
extern void *pool_alloc_object(pool_t *this);
extern void pool_free_object(pool_t *this, void *object);

extern void alloc_memory_init();
extern void alloc_memory_test();
extern void *alloc_memory(int size);
extern void free_memory(void *pointer);
extern void out_of_memory(int user);

#endif
