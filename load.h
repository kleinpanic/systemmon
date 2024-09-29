#ifndef LOAD_H
#define LOAD_H

typedef struct {
    double load1;
    double load5;
    double load15;
} LoadInfo;

LoadInfo get_load_info();

#endif // LOAD_H
