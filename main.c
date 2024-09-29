#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cpu.h"
#include "memory.h"
#include "disk.h"
#include "network.h"
#include "load.h"
#include "user.h"
#include "battery.h"
#include "firewall.h"

#define DEFAULT_INTERVAL 1
#define DEFAULT_INTERFACE "wlp0s20f3" // Default network interface

void print_usage() {
    printf("Usage: sysmon [-i interval] [-n interface]\n");
}

int main(int argc, char *argv[]) {
    int interval = DEFAULT_INTERVAL;
    const char *interface = DEFAULT_INTERFACE;
    int opt;

    // Parse command-line arguments
    while ((opt = getopt(argc, argv, "i:n:")) != -1) {
        switch (opt) {
            case 'i':
                interval = atoi(optarg);
                break;
            case 'n':
                interface = optarg;
                break;
            default:
                print_usage();
                exit(EXIT_FAILURE);
        }
    }

    // Determine number of CPU cores
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    double *core_usages = malloc(num_cores * sizeof(double));

    while (1) {
        system("clear");

        // CPU usage
        double cpu_usage = get_cpu_usage();
        printf("CPU Usage: %.2f%%\n", cpu_usage);

        // Per-core CPU usage
        get_per_core_usage(core_usages, num_cores);
        for (int i = 0; i < num_cores; i++) {
            printf("Core %d Usage: %.2f%%\n", i, core_usages[i]);
        }

        // CPU frequency
        double cpu_frequency = get_cpu_frequency();
        printf("CPU Frequency: %.2f MHz\n", cpu_frequency);

        // CPU temperature
        double cpu_temperature = get_cpu_temperature();
        printf("CPU Temperature: %.2f°C\n", cpu_temperature);

        // Top CPU consuming process
        char process_name[256];
        int pid;
        get_top_cpu_process(process_name, &pid);
        printf("Top CPU Process: %s (PID: %d)\n", process_name, pid);

        // Memory usage
        MemoryInfo mem_info = get_memory_info();
        printf("Memory Usage: %.2f%% (Used: %lu MB, Free: %lu MB)\n", 
            mem_info.used_percent, mem_info.used / 1024, mem_info.free / 1024);

        // Disk usage
        DiskInfo disk_info = get_disk_info("/");
        printf("Disk Usage: %.2f%% (Used: %lu GB, Free: %lu GB)\n", 
            disk_info.used_percent, disk_info.used / (1024 * 1024), disk_info.free / (1024 * 1024));

        // Network activity
        NetworkInfo net_info = get_network_info(interface);
        if (net_info.upload_rate == (unsigned long)-1 || net_info.download_rate == (unsigned long)-1) {
            printf("Network (%s): Error reading network statistics\n", interface);
        } else {
            printf("Network (%s): Upload: %lu B/s, Download: %lu B/s\n", interface, net_info.upload_rate, net_info.download_rate);
        }

        // Load average
        LoadInfo load_info = get_load_info();
        printf("Load Average: 1 min: %.2f, 5 min: %.2f, 15 min: %.2f\n", 
            load_info.load1, load_info.load5, load_info.load15);

        // Firewall info
        check_ufw_status();
        get_open_ports();
        get_connected_ports();

        // User info
        int user_count = get_user_count();
        printf("Users Signed In: %d\n", user_count);

        // Battery info
        double battery_percentage = get_battery_percentage();
        double battery_wattage = get_battery_wattage();
        double battery_temperature = get_battery_temperature();
        const char* battery_status = get_battery_status();
        printf("Battery Percentage: %.2f%%\n", battery_percentage);
        printf("Battery Wattage: %.2f W\n", battery_wattage);
        printf("Battery Temperature: %.2f°C\n", battery_temperature);
        printf("Battery Status: %s\n", battery_status);

        // WiFi signal strength
        get_wifi_signal_strength(interface);

        // Wait for the specified interval
        sleep(interval);
    }

    free(core_usages);
    return 0;
}
