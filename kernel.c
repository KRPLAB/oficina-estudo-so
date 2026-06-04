#include "include/types.h"

/* 
 * From https://osdev.wiki/wiki/Multiboot1_Bare_Bones 2026-06-04 tutorial
 */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
        len++;
	return len;
}

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
static uint16_t *terminal_buffer = (uint16_t*)VGA_MEMORY;


void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if(++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT)
                terminal_row = 0;
        }
    }
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

// void myprintf(char* str) {
//     unsigned short* video_memory = (unsigned short*) 0xB8000;
//     for(int i = 0; str[i] != '\0'; i++)
//         video_memory[i] = (video_memory[i] & 0xFF00) | str[i]; 
// }

typedef void (*constructor)();

extern constructor *start_ctors;
extern constructor *end_ctors;

extern void call_constructors() {
	for (constructor *i = start_ctors; i != end_ctors; i++)
		(*i)();
}

extern void kernel_main(void *multiboot_structure, uint32_t /* magic_number */) {
	(void) multiboot_structure;

	call_constructors();
	// myprintf("Kernel is alive!\n");
	/* Initialize terminal interface */
	terminal_initialize();

	terminal_writestring("Hello, The Kernel is alive!\n");
	terminal_writestring("I implemented a simple terminal!\n");
	terminal_writestring("This is a test of the terminal's ability to handle newlines.\n");
	terminal_writestring("The terminal should correctly move to the next line after each newline character.\n");
	terminal_writestring("If you see this text on separate lines, the terminal is working correctly!\n");
	terminal_writestring("This concludes the terminal test.\n");

	while (1)
		;
}