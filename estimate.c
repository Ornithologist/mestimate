#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_PAGES 20
#define PAGE_SIZE 4096
#define TRIGGER_RATIO 2
#define TRIGGER_BUFFER 100

int triggered = 0;
int page_size = 0;
int expected_cpu_ms = 0;

int main(int argc, char **argv)
{
    int cur_cpu_ms;
    int trigger_count = 0;
    unsigned long long count = 0;

    if ((page_size = sysconf(_SC_PAGESIZE)) == -1) page_size = PAGE_SIZE;

    while (!triggered) {
        ++count;
        cur_cpu_ms = time_memset();
        if (expected_cpu_ms == 0) expected_cpu_ms = cur_cpu_ms;

        if ((cur_cpu_ms / expected_cpu_ms) >= TRIGGER_RATIO)
            ++trigger_count;

        if (trigger_count >= TRIGGER_BUFFER)
            triggered = 1;
    }

    unsigned long long ram_size = (count - TRIGGER_BUFFER) * page_size * NUM_PAGES;
    printf("Memory size is approximately %llu MB\n", ram_size / 1000000);

    return 0;
}

int time_memset(void)
{
    clock_t start = clock(), diff;

    void *allocated;
    if ((allocated = malloc(NUM_PAGES * page_size)) == NULL) {
        return 0;
    }
    memset(allocated, 0, NUM_PAGES * page_size);

    diff = clock() - start;
    return diff;
}
