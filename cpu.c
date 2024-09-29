#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static long previous_total = 0, previous_idle = 0;

double get_cpu_usage() {
    FILE *fp;
    char buffer[128];
    long user, nice, system, idle, iowait, irq, softirq, steal;
    long total, total_diff, idle_diff;
    double cpu_usage;

    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fgets(buffer, sizeof(buffer), fp);
    sscanf(buffer, "cpu %ld %ld %ld %ld %ld %ld %ld %ld",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    fclose(fp);

    total = user + nice + system + idle + iowait + irq + softirq + steal;
    total_diff = total - previous_total;
    idle_diff = idle - previous_idle;

    cpu_usage = (100.0 * (total_diff - idle_diff)) / total_diff;

    previous_total = total;
    previous_idle = idle;

    return cpu_usage;
}

void get_per_core_usage(double *core_usages, int num_cores) {
    FILE *fp;
    char buffer[256];
    long user, nice, system, idle, iowait, irq, softirq, steal;
    long total, total_diff, idle_diff;
    static long previous_totals[64] = {0}, previous_idles[64] = {0}; // assuming max 64 cores

    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fgets(buffer, sizeof(buffer), fp); // Skip the first line for overall CPU
    for (int i = 0; i < num_cores; i++) {
        fgets(buffer, sizeof(buffer), fp);
        sscanf(buffer, "cpu%*d %ld %ld %ld %ld %ld %ld %ld %ld",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);

        total = user + nice + system + idle + iowait + irq + softirq + steal;
        total_diff = total - previous_totals[i];
        idle_diff = idle - previous_idles[i];

        core_usages[i] = (100.0 * (total_diff - idle_diff)) / total_diff;

        previous_totals[i] = total;
        previous_idles[i] = idle;
    }
    fclose(fp);
}

double get_cpu_frequency() {
    FILE *fp;
    double frequency = 0.0;

    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp)) {
        if (sscanf(buffer, "cpu MHz : %lf", &frequency) == 1) {
            fclose(fp);
            return frequency;
        }
    }

    fclose(fp);
    return frequency;
}

double get_cpu_temperature() {
    FILE *fp;
    double temperature = 0.0;

    fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (fp == NULL) {
        perror("fopen");
        return temperature;
    }

    fscanf(fp, "%lf", &temperature);
    fclose(fp);

    // Convert millidegrees to degrees
    return temperature / 1000.0;
}

void get_top_cpu_process(char *process_name, int *pid) {
    FILE *fp;

    fp = popen("ps -eo pid,comm,pcpu --sort=-pcpu | head -2 | tail -1", "r");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%d %s", pid, process_name);
    pclose(fp);
}
