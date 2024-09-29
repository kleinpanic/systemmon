#ifndef MEMORY_H
#define MEMORY_H

typedef struct {
    unsigned long total;
    unsigned long used;
    unsigned long free;
    double used_percent;
} MemoryInfo;

MemoryInfo get_memory_info();

#endif // MEMORY_H
