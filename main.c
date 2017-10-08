#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>

#include IMPL

#ifdef OPT
#define OUT_FILE "opt.txt"
#else
#define OUT_FILE "orig.txt"
#endif

#define DICT_FILE "./dictionary/words.txt"

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

static unsigned int count_file_line(FILE* fp)
{
    unsigned int num_line = 0;
    char line[MAX_LAST_NAME_SIZE];
    while (fgets(line, sizeof(line), fp)) num_line++;
    rewind(fp);
    return num_line;
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int i = 0;
    char line[MAX_LAST_NAME_SIZE];
    struct timespec start, end;
    double cpu_time1, cpu_time2;

    /* check file opening */
    fp = fopen(DICT_FILE, "r");
    if (fp == NULL) {
        printf("cannot open the file\n");
        return -1;
    }

#ifdef POOL
    unsigned int num_line = count_file_line(fp);
    printf("Number of line: %u\n", num_line);
    /* allocate memory for the memory pool */
    unsigned int redundancy = 500;
    pool *pPool = pool_create(sizeof(entry) * num_line + redundancy);
#endif

#ifdef HASH
    unsigned int num_line = count_file_line(fp);
    printf("Number of line: %u\n", num_line);
    /* Emperically, loading factor <= 0.75 would give a better performance */
    unsigned int size = (int)(num_line / 0.75);
    entry *ht[size];
#endif

    /* build the entry */
    entry *pHead, *e;
#ifdef POOL
    pHead = pool_alloc(pPool, sizeof(entry));
#else
    pHead = (entry *) malloc(sizeof(entry));
#endif
    printf("size of entry : %lu bytes\n", sizeof(entry));
    e = pHead;
    e->pNext = NULL;


#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
    printf("PID = %d\n", getpid());
    // sleep(5);
    clock_gettime(CLOCK_REALTIME, &start);
    while (fgets(line, sizeof(line), fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
#ifdef POOL
        e = append_with_mempool(line, e, pPool);
#elif defined HASH
        append_hash(line, ht, size);
#else
        e = append(line, e);
#endif
    }
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);

    /* close file as soon as possible */
    fclose(fp);

    /* the givn last name to find */
    char input[MAX_LAST_NAME_SIZE] = "zyxel";

    /* point e back to pHead */
    e = pHead;
#ifdef HASH
    assert(findName_hash(input, ht, size) &&
           "Did you implement findName_hash() in " IMPL "?");
#else
    assert(findName(input, e) &&
           "Did you implement findName() in " IMPL "?");
#endif
#ifdef HASH
    assert(0 == strcmp(findName_hash(input, ht, size)->lastName, "zyxel"));
#else
    assert(0 == strcmp(findName(input, e)->lastName, "zyxel"));
#endif

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
    /* compute the execution time */
    clock_gettime(CLOCK_REALTIME, &start);
#ifdef HASH
    findName_hash(input, ht, size);
#else
    findName(input, e);
#endif
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time2 = diff_in_second(start, end);

    FILE *output = fopen(OUT_FILE, "a");
    fprintf(output, "append() findName() %lf %lf\n", cpu_time1, cpu_time2);
    fclose(output);

    printf("execution time of append() : %lf sec\n", cpu_time1);
    printf("execution time of findName() : %lf sec\n", cpu_time2);

#ifdef POOL
    pool_destroy(pPool);
#else
    /* free all memory in linked list */
    while (pHead) {
        e = pHead;
        pHead = pHead->pNext;
        free(e);
    }
    free(pHead);
#endif

    return 0;
}
