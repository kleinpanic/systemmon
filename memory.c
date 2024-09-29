#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MemoryInfo get_memory_info() {
    FILE *fp;
    char buffer[256];
    unsigned long mem_total, mem_free, mem_available, buffers, cached;
    MemoryInfo mem_info;

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (sscanf(buffer, "MemTotal: %lu kB", &mem_total) == 1 ||
            sscanf(buffer, "MemFree: %lu kB", &mem_free) == 1 ||
            sscanf(buffer, "MemAvailable: %lu kB", &mem_available) == 1 ||
            sscanf(buffer, "Buffers: %lu kB", &buffers) == 1 ||
            sscanf(buffer, "Cached: %lu kB", &cached) == 1) {
            continue;
        }
    }
    fclose(fp);

    mem_info.total = mem_total;
    mem_info.free = mem_available;
    mem_info.used = mem_total - mem_available;
    mem_info.used_percent = (100.0 * mem_info.used) / mem_info.total;

    return mem_info;
}
