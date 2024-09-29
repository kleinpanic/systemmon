#ifndef DISK_H
#define DISK_H

typedef struct {
    unsigned long total;
    unsigned long used;
    unsigned long free;
    double used_percent;
} DiskInfo;

DiskInfo get_disk_info(const char *path);

#endif // DISK_H
