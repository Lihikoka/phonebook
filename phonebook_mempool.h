#ifndef _PHONEBOOK_H
#define _PHONEBOOK_H

#define MAX_LAST_NAME_SIZE 16

/* TODO: After modifying the original version, uncomment the following
 * line to set OPT properly */
#define POOL 1
typedef struct __PHONE_BOOK_DETAIL {
    char lastName[MAX_LAST_NAME_SIZE];
    char firstName[16];
    char email[16];
    char phone[10];
    char cell[10];
    char addr1[16];
    char addr2[16];
    char city[16];
    char state[2];
    char zip[5];
    struct __PHONE_BOOK_ENTRY *pNext;
} phonebook_detail;

typedef struct __LAST_NAME_ENTRY {
    char lastName[MAX_LAST_NAME_SIZE];
    phonebook_detail *detail;
    struct __LAST_NAME_ENTRY *pNext;
} entry;

typedef struct __POOL {
    entry *next;
    entry *end;
} pool;

entry *findName(char lastName[], entry *pHead);
entry *append(char lastName[], entry *e);
entry *append_with_mempool(char lastName[], entry *e, pool *p);
pool *pool_create(size_t size);
void pool_destroy(pool *p);
size_t pool_available(pool *p);
entry *pool_alloc(pool *p, size_t size);
#endif
