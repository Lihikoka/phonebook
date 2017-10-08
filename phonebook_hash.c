#   include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "phonebook_hash.h"

entry *findName(char lastName[], entry *pHead)
{
    while (pHead != NULL) {
        if (strcasecmp(lastName, pHead->lastName) == 0)
            return pHead;
        pHead = pHead->pNext;
    }
    return NULL;
}

entry *findName_hash(char lastName[], entry *ht[], int ht_size)
{
    unsigned int hash_value = BKDRhash(lastName) % ht_size;
    if (NULL == ht[hash_value]) return NULL;
    for (int i = 0; i < ht_size; i++)
        if (!strcmp(lastName, ht[(hash_value + i * i) % ht_size]->lastName))
            return ht[(hash_value + i * i) % ht_size];

    return NULL;
}

entry *append(char lastName[], entry *e)
{
    e->pNext = (entry *) malloc(sizeof(entry));
    e = e->pNext;
    strcpy(e->lastName, lastName);
    e->pNext = NULL;

    return e;
}

entry *append_with_mempool(char lastName[], entry *e, pool *p)
{
    e->pNext = pool_alloc(p, sizeof(entry));
    if (NULL == e->pNext) return NULL; // memory pool is full
    e = e->pNext;
    strcpy(e->lastName, lastName);
    e->pNext = NULL;

    return e;
}

void append_hash(char lastName[], entry *ht[], int ht_size)
{
    unsigned int hash_value = BKDRhash(lastName) % ht_size;
    for (int i = 0; i < ht_size; i++) {
        if (NULL == ht[(hash_value + i * i) % ht_size]) {
            ht[(hash_value + i * i) % ht_size] = (entry*) malloc(sizeof(entry));
            strcpy(ht[(hash_value + i * i) % ht_size]->lastName, lastName);
            break;
        }
    }
}

pool *pool_create(size_t size)
{
    pool *p = (pool*) malloc(size + sizeof(pool));
    p->next = (entry*) &p[1];
    p->end = p->next + size;
    return p;
}

void pool_destroy(pool *p)
{
    free(p);
}

size_t pool_available(pool *p)
{
    return p->end - p->next;
}

entry *pool_alloc(pool *p, size_t size)
{
    if (pool_available(p) < sizeof(entry)) return NULL;
    entry *mem = (entry*) p->next;
    p->next += size;
    return mem;
}

unsigned int BKDRhash(char *str)
{
    unsigned int seed = 131;
    unsigned int hash = 0;
    while (*str)
        hash = hash * seed + (*str++);
    return (hash & 0x7FFFFFFF);
}
