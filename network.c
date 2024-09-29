#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static unsigned long previous_rx_bytes = 0, previous_tx_bytes = 0;

NetworkInfo get_network_info(const char *interface) {
    FILE *fp;
    char path[256];
    unsigned long rx_bytes, tx_bytes;
    NetworkInfo net_info = {0, 0};

    snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/rx_bytes", interface);
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("fopen");
        net_info.upload_rate = (unsigned long)-1;
        net_info.download_rate = (unsigned long)-1;
        return net_info;
    }
    fscanf(fp, "%lu", &rx_bytes);
    fclose(fp);

    snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/tx_bytes", interface);
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("fopen");
        net_info.upload_rate = (unsigned long)-1;
        net_info.download_rate = (unsigned long)-1;
        return net_info;
    }
    fscanf(fp, "%lu", &tx_bytes);
    fclose(fp);

    net_info.download_rate = rx_bytes - previous_rx_bytes;
    net_info.upload_rate = tx_bytes - previous_tx_bytes;

    previous_rx_bytes = rx_bytes;
    previous_tx_bytes = tx_bytes;

    return net_info;
}

void get_open_ports() {
    printf("Open ports:\n");
    system("ss -ltn | tail -n +2");
}

void get_connected_ports() {
    printf("Connected ports:\n");
    system("ss -tun | tail -n +2");
}

void get_wifi_signal_strength(const char *interface) {
    char command[256];
    snprintf(command, sizeof(command), "nmcli dev wifi list ifname %s | grep '*' | awk '{print $8}'", interface);
    printf("WiFi Signal Strength (dBm): ");
    system(command);
}
