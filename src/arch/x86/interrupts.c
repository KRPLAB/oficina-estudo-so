#include <kernel/gdt.h>
#include <kernel/interrupts.h>
#include <kernel/port.h> // <--- Usando o seu port.h nativo
#include <kernel/types.h>

void myprintf(char *str);

// ------------------ Portas do PIC ------------------
#define PIC1_COMMAND 0x20
#define PIC2_COMMAND 0xA0
#define PIC1_DATA 0x21
#define PIC2_DATA 0xA1

// ------------------ IDT ------------------
static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtp;

// Forward declarations (as funções vêm do assembly)
extern void ignore_int();
extern void irq_stub_0();
extern void irq_stub_1();
extern void irq_stub_2();
extern void irq_stub_3();
extern void irq_stub_4();
extern void irq_stub_5();
extern void irq_stub_6();
extern void irq_stub_7();
extern void irq_stub_8();
extern void irq_stub_9();
extern void irq_stub_10();
extern void irq_stub_11();
extern void irq_stub_12();
extern void irq_stub_13();
extern void irq_stub_14();
extern void irq_stub_15();

// Função auxiliar para preencher uma entrada
static void idt_set_entry(uint8_t num, uint32_t handler, uint16_t sel, uint8_t flags) {
	idt[num].isr_low = handler & 0xFFFF;
	idt[num].isr_high = (handler >> 16) & 0xFFFF;
	idt[num].kernel_cs = sel;
	idt[num].reserved = 0;
	idt[num].attributes = flags;
}

// Inicialização completa
void init_interrupts(void) {
	// 1. Preencher todas as 256 entradas com o handler "ignore"
	for (int i = 0; i < IDT_ENTRIES; i++) {
		idt_set_entry(i, (uint32_t)ignore_int, 0x08, 0x8E);
	}

	// 2. Instalar exceções usando a tabela de stubs do assembler
	for (int i = 0; i < 32; i++) {
		idt_set_entry(i, isr_stub_table[i], 0x08, 0x8E);
	}

	// 3. Remapear os PICs usando suas funções estáveis e io_wait()
	uint8_t mask1 = inb(PIC1_DATA);
	uint8_t mask2 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, 0x11);
	io_wait(); // inicialização em cascata
	outb(PIC2_COMMAND, 0x11);
	io_wait();

	outb(PIC1_DATA, 0x20);
	io_wait(); // mestre: 0x20 a 0x27
	outb(PIC2_DATA, 0x28);
	io_wait(); // escravo: 0x28 a 0x2F

	outb(PIC1_DATA, 0x04);
	io_wait(); // mestre aponta slave na IRQ2
	outb(PIC2_DATA, 0x02);
	io_wait(); // escravo responde como cascata

	outb(PIC1_DATA, 0x01);
	io_wait(); // modo 8086 x86
	outb(PIC2_DATA, 0x01);
	io_wait();

	outb(PIC1_DATA, mask1); // devolve mascaras anteriores
	outb(PIC2_DATA, mask2);

	// 4. Instalar handlers de IRQ (0x20..0x2F)
	idt_set_entry(0x20, (uint32_t)irq_stub_0, 0x08, 0x8E);
	idt_set_entry(0x21, (uint32_t)irq_stub_1, 0x08, 0x8E);
	idt_set_entry(0x22, (uint32_t)irq_stub_2, 0x08, 0x8E);
	idt_set_entry(0x23, (uint32_t)irq_stub_3, 0x08, 0x8E);
	idt_set_entry(0x24, (uint32_t)irq_stub_4, 0x08, 0x8E);
	idt_set_entry(0x25, (uint32_t)irq_stub_5, 0x08, 0x8E);
	idt_set_entry(0x26, (uint32_t)irq_stub_6, 0x08, 0x8E);
	idt_set_entry(0x27, (uint32_t)irq_stub_7, 0x08, 0x8E);
	idt_set_entry(0x28, (uint32_t)irq_stub_8, 0x08, 0x8E);
	idt_set_entry(0x29, (uint32_t)irq_stub_9, 0x08, 0x8E);
	idt_set_entry(0x2A, (uint32_t)irq_stub_10, 0x08, 0x8E);
	idt_set_entry(0x2B, (uint32_t)irq_stub_11, 0x08, 0x8E);
	idt_set_entry(0x2C, (uint32_t)irq_stub_12, 0x08, 0x8E);
	idt_set_entry(0x2D, (uint32_t)irq_stub_13, 0x08, 0x8E);
	idt_set_entry(0x2E, (uint32_t)irq_stub_14, 0x08, 0x8E);
	idt_set_entry(0x2F, (uint32_t)irq_stub_15, 0x08, 0x8E);

	// 5. Carregar IDTR
	idtp.limit = sizeof(idt) - 1;
	idtp.base = (uint32_t)idt;
	__asm__ volatile(
		"lidt %0"
		:
		: "m"(idtp));
}

void enable_interrupts() {
	__asm__ volatile("sti");
}

void disable_interrupts() {
	__asm__ volatile("cli");
}

uint32_t handle_interrupt(uint32_t interrupt, uint32_t esp) {
	// Declarado como array na Stack para ser mutável e evitar GPF
	char msg[] = "INTERRUPT 0x00\n";
	char *hex = "0123456789ABCDEF";

	msg[12] = hex[(interrupt >> 4) & 0xF];
	msg[13] = hex[interrupt & 0xF];
	myprintf(msg);

	return esp;
}
