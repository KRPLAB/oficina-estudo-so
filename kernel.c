// Funções inline para I/O de porta
static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    asm volatile("inb %1, %0" : "=a" (result) : "dN" (port));
    return result;
}

static inline void outb(unsigned short port, unsigned char data) {
    asm volatile("outb %0, %1" : : "a" (data), "dN" (port));
}

// Inicializa porta serial COM1
void serial_init(void) {
    outb(0x3F8 + 1, 0x00);    // Disable interrupts
    outb(0x3F8 + 3, 0x80);    // Set DLAB
    outb(0x3F8 + 0, 0x03);    // Divisor low byte (115200 baud)
    outb(0x3F8 + 1, 0x00);    // Divisor high byte
    outb(0x3F8 + 3, 0x03);    // Set 8 bits, 1 stop, no parity
    outb(0x3F8 + 2, 0xC7);    // Enable FIFO
}

// Escreve um caractere na porta serial
void serial_putchar(char c) {
    while ((inb(0x3F8 + 5) & 0x20) == 0);  // Wait for transmit buffer empty
    outb(0x3F8, c);
}

// Print string via serial
void myprintf(char* str) {
    for(int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            serial_putchar('\r');
        }
        serial_putchar(str[i]);
    }
}

void kernel_main(void* multiboot_structure, unsigned int magicnumber) {
    serial_init();
    myprintf("Hello, World!\n");
    myprintf("Kernel running...\n");
    while(1);
}