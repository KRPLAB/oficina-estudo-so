#ifndef __GDT_H
#define __GDT_H

#include "types.h"

/*
 * Cada entrada da GDT (A "SegmentDescriptor" do C++)
 * Tem 8 bytes de tamanho e é composta por:
 * - Limit (20 bits): Define o tamanho do segmento
 * - Base (32 bits): Define o endereço base do segmento
 * - Access Byte (8 bits): Define as permissões e o tipo do segmento
 * - Granularity (8 bits): Define a granularidade do segmento e os 4 bits mais
 * altos do Limit
 */
struct gdt_entry {
	uint16_t limit_low;  // Os 16 bits mais baixos do Limit
	uint16_t base_low;   // Os 16 bits mais baixos da Base
	uint8_t base_middle; // Os próximos 8 bits da Base
	uint8_t access;      // Byte de permissões (Ring 0/3, Executável, etc)
	uint8_t granularity; // 4 bits mais altos do Limit + 4 bits de Flags
	uint8_t base_high;   // Os últimos 8 bits da Base
} __attribute__((packed));

// O ponteiro especial de 6 bytes que a CPU exige para o comando LGDT
struct gdt_ptr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

// Protótipos das funções que criarão a tabela
void init_gdt();
void set_gdt_entry(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

#endif