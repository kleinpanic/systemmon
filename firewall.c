#include "firewall.h"
#include <stdio.h>
#include <stdlib.h>

void check_ufw_status() {
    printf("UFW Firewall Status: ");
    system("ufw status | grep -i 'Status' | awk '{print $2}'");
    // Replace the above line with your own firewall app status check if needed
}
