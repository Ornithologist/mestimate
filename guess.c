#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_PAGES 20
#define PAGE_SIZE 4096
#define TRIGGER_RATIO 2
// #define TRIGGER_BUFFER 100

int triggered = 0;
int page_size = 0;
int expected_cpu_ms = 0;
void *start_addr;

typedef struct link_node_t {
    void *next;
} link_node_t;

int main(int argc, char **argv)
{
    int cur_cpu_ms;
    unsigned long long count = 0;

    if ((page_size = sysconf(_SC_PAGESIZE)) == -1) page_size = PAGE_SIZE;

    while (!triggered) {
        ++count;
        cur_cpu_ms = time_memset();
        
        if ((cur_cpu_ms / expected_cpu_ms) >= TRIGGER_RATIO)
            triggered = 1;
    }
    
    unsigned long long ram_size = count * page_size * NUM_PAGES;
    printf("Count is %d, memory size is approximately %llu MB\n", count, ram_size / 1000000);
    
    return 0;
}

int time_memset(void)
{
    link_node_t *allocated;
    int avg_ms, total_ms = 0, count = 0;

    if ((allocated = (link_node_t *) malloc(NUM_PAGES * page_size)) == NULL)
        return 0;

    // ini first addr, or link_node_t the new addr to the list
    if (start_addr == NULL) {
        start_addr = (void *) allocated;
        allocated->next = NULL;
    } else {
        link_node_t *last = (link_node_t *) start_addr;
        while(last->next != NULL)
            last = last->next;
        last->next = (void *) allocated;
    }

    // iterate through the list and memset each page in each node, find max
    link_node_t *itr = (link_node_t *) start_addr;
    while(itr != NULL) {
        int i;

        for (i = 1; i < NUM_PAGES; i++) {
            void *set_addr = (char *) itr + i * page_size;
            clock_t start = clock(), diff;
            memset(set_addr, 0, page_size);
            diff = clock() - start;
            ++count;
            total_ms += diff;
        }

        itr = (link_node_t *)itr->next;
    }
    
    // find mean
    avg_ms = total_ms / count;
    if (expected_cpu_ms == 0) {
        expected_cpu_ms = avg_ms;
    }

    printf("expected %d ms, cur %d ms\n", expected_cpu_ms, avg_ms);
    return avg_ms;
}
