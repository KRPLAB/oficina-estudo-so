// Funções inline para I/O de porta
static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    asm volatile("inb %1, %0" : "=a" (result) : "dN" (port));
    return result;
}

// Variável global que recebe o valor de ECX do loader
extern unsigned int cpu_ecx_saved;

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

void serial_print_hex(unsigned int value) {
    char hex_digits[] = "0123456789ABCDEF";
    myprintf("0x");
    for (int i = 7; i >= 0; i--) {
        unsigned int digit = (value >> (i * 4)) & 0xF;
        serial_putchar(hex_digits[digit]);
    }
}

// Print number em decimal via serial
void serial_print_dec(unsigned int n) {
    unsigned int divisor = 1000000000;
    int started = 0;
    
    while(divisor > 0) {
        unsigned int digit = n / divisor;
        if (digit > 0 || started) {
            serial_putchar('0' + digit);
            started = 1;
        }
        n = n % divisor;
        divisor = divisor / 10;
    }
    
    if (!started) serial_putchar('0');
}

// Delay simples (contagem de loops)
void delay(unsigned int cycles) {
    for(unsigned int i = 0; i < cycles; i++) {
        asm volatile("nop");  // No operation
    }
}

void kernel_main(void* multiboot_structure, unsigned int magicnumber) {
    serial_init();
    myprintf("Hello, World!\n");
    myprintf("Kernel is alive!\n");
    myprintf("Magic number: ");
    serial_print_hex(magicnumber);
    myprintf("\n");
    myprintf("ECX value: ");
    serial_print_hex(cpu_ecx_saved);
    myprintf("\n");
    
    myprintf("CAFEBABE em decimal: ");
    serial_print_dec(cpu_ecx_saved);
    myprintf("\n");
    
    unsigned int counter = 0;
    while(1) {
        delay(1000000000);  // Pequeno delay
        myprintf("Counter: ");
        serial_print_dec(counter);
        myprintf("\n");
        counter++;
    }
}