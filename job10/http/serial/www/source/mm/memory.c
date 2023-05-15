#include <utils/root.h>
#include "memory.h" 
#include "page.h"

#define POOL_NUMBER 9
pool_t pool_table[POOL_NUMBER];

object_t *object_next(object_t *this, int object_size)
{
    return (object_t *)((ulong)this + object_size);
}

static bucket_t *locate_bucket(void *pointer)
{
    bucket_t *bucket;

    bucket = (bucket_t *) page_down(pointer);
    assert(bucket->magic == BUCKET_MAGIC);
    return bucket;
}

static pool_t *locate_pool(void *pointer)
{
    pool_t *pool;
    bucket_t *bucket;

    bucket = locate_bucket(pointer);
    pool = bucket->pool;
    assert(pool->magic == POOL_MAGIC);
    return pool;
}

int bucket_busy(bucket_t *this)
{
    return list_empty(&this->object_list);
}

void *bucket_alloc_object(bucket_t *this)
{
    if (list_empty(&this->object_list))
        return NULL;
    this->object_count--;
    object_t *object = list_pop_front(&this->object_list);
    return object;
}

void bucket_free_object(bucket_t *this, object_t *object)
{
    this->object_count++;
    chain_init(&object->chain);
    list_push_back(&this->object_list, object);
}

int bucket_contain(bucket_t *this, object_t *object)
{
    return (ulong)object < ((ulong)this + PAGE_SIZE);
}

bucket_t *bucket_new(pool_t *pool)
{
    object_t *object;
    int object_size;
    bucket_t *this;
    
    this = (bucket_t*) alloc_page();
    if (this == NULL)
        return NULL;
    chain_init(&this->chain);
    this->magic = BUCKET_MAGIC;
    this->pool = pool;
    this->object_count = 0;
    list_init(&this->object_list);

    object = this->object_array; 
    object_size = pool->object_size;
    while (bucket_contain(this, object)) {
        bucket_free_object(this, object);
        object = object_next(object, object_size);
    }
    return this;
}

void *pool_alloc_object(pool_t *pool)
{
    void *result;
    bucket_t *bucket;

    list_t *free_bucket_list = &pool->free_bucket_list;
    list_t *busy_bucket_list = &pool->busy_bucket_list;

    if (list_empty(free_bucket_list)) {
        bucket = bucket_new(pool);
        if (!bucket) {
            return NULL;
        }
        list_push_back(free_bucket_list, bucket);
    }

    bucket = list_front(free_bucket_list);
    result = bucket_alloc_object(bucket);
    
    if (bucket_busy(bucket)) {
        chain_remove(bucket);
        list_push_back(busy_bucket_list, bucket);
    }

    return result;
}

void pool_free_object(pool_t *pool, void *object)
{
    bucket_t *bucket;

    bucket = locate_bucket(object);
    bucket_free_object(bucket, object);
    if (!bucket_busy(bucket)) {
        chain_remove(bucket);
        list_push_back(&pool->free_bucket_list, bucket);
    }
}

void pool_init(pool_t *this, int object_size)
{
    this->magic = POOL_MAGIC;
    this->object_size = object_size;
    list_init(&this->busy_bucket_list);
    list_init(&this->free_bucket_list);
}

void *alloc_memory(int size)
{
    int i;
    pool_t *pool;

    for (i = 0; i < POOL_NUMBER; i++) {
        pool = pool_table + i;
        if (size <= pool->object_size)
            return pool_alloc_object(pool);
    }
    panic("requested size(%d) too big\n", size);
    return NULL;
}

void free_memory(void *pointer)
{
    pool_t *pool;

    if (!pointer)
        panic("Try to free null pointer\n");
    pool = locate_pool(pointer);
    pool_free_object(pool, pointer);
}

static void bucket_list_dump(char *title, list_t *list)
{
    chain_t *chain;
    bucket_t *bucket;
    int bucket_count = 0;
    int object_count = 0;

    list_each (list, chain) {
        bucket = (bucket_t *)chain;
        object_count += bucket->object_count;
        bucket_count += 1;
    }

    printf("%s, buckets = %d, objects = %d; ", 
           title, bucket_count, object_count);
}

void pool_dump(pool_t *this)
{
    printf("size-%d: ", this->object_size);
    bucket_list_dump("free", &this->free_bucket_list);
    bucket_list_dump("busy", &this->busy_bucket_list);
    printf("\n");
}

void pool_table_dump()
{
    int i;
    pool_t *pool;

    for (i = 0; i < POOL_NUMBER; i++) {
        pool = pool_table + i;
        pool_dump(pool);
    }
}

void alloc_memory_init()
{
    puts("init memory");
    pool_init(pool_table + 0, 16);
    pool_init(pool_table + 1, 32);
    pool_init(pool_table + 2, 64);
    pool_init(pool_table + 3, 128);
    pool_init(pool_table + 4, 256);
    pool_init(pool_table + 5, 512);
    pool_init(pool_table + 6, 1024);
    pool_init(pool_table + 7, 2000);
    pool_init(pool_table + 8, 4000);
}

#define POINTERS 10
void alloc_memory_test()
{
    int i;
    void *p;
    void *pointers[POINTERS];

    for (i = 0; i < POINTERS; i++) {
        p = alloc_memory(1000);
        pointers[i] = p;
    }

    for (i = 0; i < POINTERS; i++) {
        p = pointers[i];
        free_memory(p);
    }

    pool_table_dump();
    puts("memory test passed");
}
