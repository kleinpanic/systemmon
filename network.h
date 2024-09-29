#ifndef NETWORK_H
#define NETWORK_H

typedef struct {
    unsigned long upload_rate;
    unsigned long download_rate;
} NetworkInfo;

NetworkInfo get_network_info(const char *interface);
void get_open_ports();
void get_connected_ports();
void get_wifi_signal_strength(const char *interface);

#endif // NETWORK_H
