# System Monitoring Tool

This is a simple system monitoring tool written in C for Linux systems. It monitors and displays key system metrics such as CPU usage, memory usage, disk usage, and network activity.

## Features

- Display current CPU usage
- Display current memory usage
- Display current disk usage
- Display network activity (upload and download rates)

## Usage

### Compilation

To compile the tool, use the following commands:
make clean
make 
make install 

### Runing the tool
./sysmon [-i interval] [-n interface]
-i interval: Set the update interval in seconds (default is 1 second)
-n interface: Specify the network interface to monitor (default is wlp0s20f3)

