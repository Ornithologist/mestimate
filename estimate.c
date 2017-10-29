#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define NUM_PAGES 20
#define PAGE_SIZE 4096
#define TRIGGER_RATIO 2
#define TRIGGER_BUFFER 20

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
    printf("Memory size is approximately %llu bytes\n", ram_size);
    printf("Expected set time is %d ms, actual set time is %d ms, count is %llu\n",
           expected_cpu_ms, cur_cpu_ms, count);

    return 0;
}

int time_memset(void)
{
    clock_t start = clock(), diff;

    void *mmapped;
    if ((mmapped = mmap(NULL, NUM_PAGES * page_size, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        return 0;
    }
    memset(mmapped, 0, NUM_PAGES * page_size);

    diff = clock() - start;
    return diff;
}
