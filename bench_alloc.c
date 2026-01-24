#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define JOB_BODY_MAX 8192
#define ITERATIONS 10000000

int main() {
    size_t block_blob_size = 2048;
    unsigned char *block_blob = malloc(block_blob_size);
    memset(block_blob, 0xAA, block_blob_size);

    clock_t start, end;
    double cpu_time_used;

    // Baseline: Heap allocation
    start = clock();
    for (int i = 0; i < ITERATIONS; i++) {
        unsigned char *block = calloc(block_blob_size, sizeof(char));
        memcpy(block, block_blob, block_blob_size);

        // Simulating usage (prevent optimization)
        if (block[0] != 0xAA) printf("Error\n");
        block[0] = 0xBB;

        free(block);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Heap allocation time: %f seconds\n", cpu_time_used);

    // Optimization: Stack allocation
    start = clock();
    for (int i = 0; i < ITERATIONS; i++) {
        unsigned char block_stack[JOB_BODY_MAX];
        unsigned char *block = NULL;

        if (block_blob_size <= JOB_BODY_MAX) {
            block = block_stack;
        } else {
            block = calloc(block_blob_size, sizeof(char));
        }

        // Note: calloc zeros memory, stack doesn't.
        // But memcpy overwrites it anyway.
        // If the original code relied on zero initialization for parts NOT covered by memcpy
        // (which is impossible if we copy block_blob_size), then we are fine.
        // However, if we only copy partial, then stack needs memset.
        // In the target code: memcpy(block, bt->block_blob, bt->block_blob_size);
        // So it is fully overwritten.

        memcpy(block, block_blob, block_blob_size);

        // Simulating usage
        if (block[0] != 0xAA) printf("Error\n");
        block[0] = 0xBB;

        if (block != block_stack) {
            free(block);
        }
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Stack allocation time: %f seconds\n", cpu_time_used);

    free(block_blob);
    return 0;
}
