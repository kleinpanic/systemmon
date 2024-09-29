#include "battery.h"
#include <stdio.h>
#include <stdlib.h>

double get_battery_percentage() {
    FILE *fp;
    double percentage = 0.0;

    fp = fopen("/sys/class/power_supply/BAT0/capacity", "r");
    if (fp == NULL) {
        perror("fopen");
        return percentage;
    }

    fscanf(fp, "%lf", &percentage);
    fclose(fp);

    return percentage;
}

double get_battery_wattage() {
    FILE *fp;
    double wattage = 0.0;

    fp = fopen("/sys/class/power_supply/BAT0/power_now", "r");
    if (fp == NULL) {
        perror("fopen");
        return wattage;
    }

    fscanf(fp, "%lf", &wattage);
    fclose(fp);

    // Convert from microwatts to watts
    return wattage / 1e6;
}

double get_battery_temperature() {
    FILE *fp;
    double temperature = 0.0;

    fp = fopen("/sys/class/power_supply/BAT0/temp", "r");
    if (fp == NULL) {
        perror("fopen");
        return temperature;
    }

    fscanf(fp, "%lf", &temperature);
    fclose(fp);

    // Convert from millidegrees to degrees
    return temperature / 10.0;
}

const char* get_battery_status() {
    FILE *fp;
    static char status[16];

    fp = fopen("/sys/class/power_supply/BAT0/status", "r");
    if (fp == NULL) {
        perror("fopen");
        return "Unknown";
    }

    fscanf(fp, "%15s", status);
    fclose(fp);

    return status;
}
