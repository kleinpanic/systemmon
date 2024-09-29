#include "load.h"
#include <stdio.h>
#include <stdlib.h>

LoadInfo get_load_info() {
    LoadInfo load_info;
    if (getloadavg((double *)&load_info, 3) == -1) {
        perror("getloadavg");
        exit(EXIT_FAILURE);
    }
    return load_info;
}
