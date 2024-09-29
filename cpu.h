#ifndef CPU_H
#define CPU_H

double get_cpu_usage();
void get_per_core_usage(double *core_usages, int num_cores);
double get_cpu_frequency();
double get_cpu_temperature();
void get_top_cpu_process(char *process_name, int *pid);

#endif // CPU_H
