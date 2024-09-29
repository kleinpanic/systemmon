#include "user.h"
#include <stdio.h>
#include <stdlib.h>

int get_user_count() {
    FILE *fp;
    int user_count = 0;
    char buffer[256];

    fp = popen("who | wc -l", "r");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        user_count = atoi(buffer);
    }
    pclose(fp);

    return user_count;
}
