#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"

#define IDT_ENTRIES 256

/*
 * @brief Estrutura de uma entrada da IDT (Interrupt Descriptor Table)
 */
struct idt_entry {
	uint16_t isr_low;		// bits 0-15 do endereço do handler
	uint16_t kernel_cs;		// seletor do segmento de código (GDT)
	uint8_t reserved;		// reservado, sempre 0
	uint8_t attributes;		// flags de atributos
	uint16_t isr_high;		// bits 16-31 do endereço do handler
} __attribute__((packed));

/*
 * @brief Estrutura do ponteiro da IDT
 */
struct idt_ptr {
	uint16_t limit;		// tamanho da IDT
	uint32_t base;		// endereço da IDT
} __attribute__((packed));

// Funções principais

/*
 * @brief Inicializa o tratamento de interrupções
 */
void init_interrupts(void);

/*
 * @brief Habilita interrupções
 */
void enable_interrupts(void);

/*
 * @brief Desabilita interrupções
 */
void disable_interrupts(void);

/*
 * @brief Handler C chamado pelo Assembly para lidar com interrupções 
 * @param interrupt Número da interrupção
 * @param esp Ponteiro para a pilha no momento da interrupção
 * @return Novo valor do ponteiro da pilha após o tratamento da interrupção
 */
uint32_t handle_interrupt(uint32_t interrupt, uint32_t esp);

/*
 * @brief Tabela de stubs gerada no assembly (endereços de isr_stub_0..isr_stub_31)
 */
extern uint32_t isr_stub_table[IDT_ENTRIES];

#endif /* __INTERRUPTS_H__ */