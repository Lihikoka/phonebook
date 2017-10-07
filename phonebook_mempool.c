#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "phonebook_mempool.h"

entry *findName(char lastName[], entry *pHead)
{
    while (pHead != NULL) {
        if (strcasecmp(lastName, pHead->lastName) == 0)
            return pHead;
        pHead = pHead->pNext;
    }
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