#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>

DiskInfo get_disk_info(const char *path) {
    struct statvfs stat;
    DiskInfo disk_info;

    if (statvfs(path, &stat) != 0) {
        perror("statvfs");
        exit(EXIT_FAILURE);
    }

    disk_info.total = stat.f_blocks * stat.f_frsize;
    disk_info.free = stat.f_bavail * stat.f_frsize;
    disk_info.used = disk_info.total - disk_info.free;
    disk_info.used_percent = (100.0 * disk_info.used) / disk_info.total;

    return disk_info;
}
