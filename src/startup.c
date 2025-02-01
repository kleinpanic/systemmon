#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>

// Define the framebuffer device path
#define FRAMEBUFFER_DEVICE "/dev/fb0"

static int fb_fd;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
static char *fb_ptr;

// A basic 5x7 font for ASCII characters 32-126
const uint8_t font[96][7] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // Space (32)
    {0x04, 0x04, 0x04, 0x04, 0x00, 0x04, 0x00}, // !
    {0x0A, 0x0A, 0x0A, 0x00, 0x00, 0x00, 0x00}, // "
    {0x0A, 0x1F, 0x0A, 0x1F, 0x0A, 0x00, 0x00}, // #
    {0x04, 0x0E, 0x05, 0x0E, 0x14, 0x0E, 0x04}, // $
    {0x18, 0x19, 0x02, 0x04, 0x08, 0x13, 0x03}, // %
    {0x0C, 0x12, 0x0C, 0x12, 0x11, 0x0E, 0x00}, // &
    {0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00}, // '
    {0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02}, // (
    {0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08}, // )
    {0x00, 0x04, 0x15, 0x0E, 0x15, 0x04, 0x00}, // *
    {0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00}, // +
    {0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x08}, // ,
    {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00}, // -
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00}, // .
    {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x00}, // /
    {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E}, // 0
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x1F}, // 1
    {0x0E, 0x11, 0x01, 0x0E, 0x10, 0x10, 0x1F}, // 2
    {0x1F, 0x01, 0x01, 0x0E, 0x01, 0x11, 0x0E}, // 3
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}, // 4
    {0x1F, 0x10, 0x10, 0x1E, 0x01, 0x11, 0x0E}, // 5
    {0x0E, 0x11, 0x10, 0x1E, 0x11, 0x11, 0x0E}, // 6
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}, // 7
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}, // 8
    {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x11, 0x0E}, // 9
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00}, // :
    // Add all remaining printable ASCII characters here
};

// Initialize the framebuffer
void initialize_framebuffer() {
    fb_fd = open(FRAMEBUFFER_DEVICE, O_RDWR);
    if (fb_fd < 0) {
        perror("Error opening framebuffer device");
        exit(EXIT_FAILURE);
    }

    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("Error reading variable screen info");
        close(fb_fd);
        exit(EXIT_FAILURE);
    }

    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo)) {
        perror("Error reading fixed screen info");
        close(fb_fd);
        exit(EXIT_FAILURE);
    }

    size_t screensize = vinfo.yres_virtual * finfo.line_length;
    fb_ptr = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fb_ptr == MAP_FAILED) {
        perror("Error mapping framebuffer to memory");
        close(fb_fd);
        exit(EXIT_FAILURE);
    }
}

// Draw a single pixel
void draw_pixel(unsigned int x, unsigned int y, uint32_t color) {
    if (x >= vinfo.xres || y >= vinfo.yres) {
        printf("Pixel out of bounds: (%u, %u)\n", x, y);
        return; // Skip pixels outside the framebuffer
    }

    size_t location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                      (y + vinfo.yoffset) * finfo.line_length;
    if (vinfo.bits_per_pixel == 32) {
        *((uint32_t *)(fb_ptr + location)) = color;
    }
}

// Draw a box
void draw_box(unsigned int x_start, unsigned int y_start, unsigned int width, unsigned int height, uint32_t color) {
    for (unsigned int y = y_start; y < y_start + height; y++) {
        for (unsigned int x = x_start; x < x_start + width; x++) {
            if (x == x_start || x == x_start + width - 1 || y == y_start || y == y_start + height - 1) {
                draw_pixel(x, y, color);
            }
        }
    }
}

// Draw a character using the font
void draw_character(unsigned int x, unsigned int y, char c, uint32_t color) {
    if (c < 32 || c > 126) {
        printf("Unsupported character: '%c'\n", c);
        return; // Skip unsupported characters
    }

    const uint8_t *glyph = font[c - 32];
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 5; col++) {
            if (glyph[row] & (1 << (4 - col))) { // Check if the bit is set
                draw_pixel(x + col, y + row, color);
            }
        }
    }
    printf("Rendered character '%c' at (%u, %u)\n", c, x, y);
}


// Draw a string of characters
void draw_string(unsigned int x, unsigned int y, const char *str, uint32_t color) {
    printf("Rendering string '%s' at (%u, %u)\n", str, x, y); // Debug the string and position
    while (*str) {
        draw_character(x, y, *str++, color);
        x += 6; // Move right for the next character
    }
}

// Draw the main blocks
void draw_main_blocks() {
    unsigned int border = 50; // Outer buffer
    unsigned int main_width = vinfo.xres - 2 * border;
    unsigned int main_height = vinfo.yres - 2 * border;

    draw_box(border, border, main_width, main_height, 0xFFFFFF); // Main window

    unsigned int inner_padding = 20; // Inner buffer
    unsigned int inner_width = main_width - 2 * inner_padding;
    unsigned int inner_height = main_height - 2 * inner_padding;

    unsigned int block_width = (inner_width - inner_padding) / 2;
    unsigned int block_height = (inner_height - inner_padding) / 3;

    const char *labels[6] = {
        "CPU Information",
        "Memory Usage",
        "Disk Activity",
        "Network Stats",
        "Power Status",
        "I/O Devices"
    };

    for (int i = 0; i < 6; i++) {
        unsigned int x_start = border + inner_padding + (i % 2) * (block_width + inner_padding);
        unsigned int y_start = border + inner_padding + (i / 2) * (block_height + inner_padding);

        draw_box(x_start, y_start, block_width, block_height, 0xFFFFFF); // Block border

        // Adjusted label placement
        unsigned int label_x = x_start + (block_width / 2) - (strlen(labels[i]) * 3); // Center horizontally
        unsigned int label_y = y_start + 10; // Slightly below top border

        printf("Placing label '%s' at (%u, %u)\n", labels[i], label_x, label_y);
        draw_string(label_x, label_y, labels[i], 0xFFFFFF); // Draw label
    }
}

// Function to clear the framebuffer
void clear_screen(uint32_t color) {
    for (unsigned int y = 0; y < vinfo.yres; y++) {
        for (unsigned int x = 0; x < vinfo.xres; x++) {
            size_t location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                              (y + vinfo.yoffset) * finfo.line_length;
            if (vinfo.bits_per_pixel == 32) {
                *((uint32_t *)(fb_ptr + location)) = color;
            }
        }
    }
}

// Persistent display loop
void run_persistent_display() {
    while (1) {
        clear_screen(0x000000); // Clear to black
        draw_main_blocks();
        sleep(1); // Refresh every second
    }
}

int main() {
    initialize_framebuffer();
    run_persistent_display();

    munmap(fb_ptr, vinfo.yres_virtual * finfo.line_length);
    close(fb_fd);
    return 0;
}

