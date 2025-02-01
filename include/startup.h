// startup.h
#ifndef STARTUP_H
#define STARTUP_H

#include <stdbool.h>
#include <stdint.h>

// Function to check if running in a graphical environment
bool is_graphical_environment();

// Function to initialize the Linux framebuffer
void initialize_framebuffer();

// Function to draw the main system resource blocks
void draw_main_blocks();

// Function to keep the display persistent
void run_persistent_display();

#endif // STARTUP_H
