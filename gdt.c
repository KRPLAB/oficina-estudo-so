#include "include/gdt.h"

// Criamos um array de 3 entradas (Null, Código, Dados)
struct gdt_entry gdt[3];
struct gdt_ptr gp;

// Função externa escrita em Assembly (loader.s) para carregar a tabela
extern void gdt_flush(uint32_t);

// Configura uma entrada individual da GDT
void set_gdt_entry(int num, uint32_t base, uint32_t limit, uint8_t access,
                   uint8_t gran) {
	// Configuração da Base
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;

	// Configuração do Limit
	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = (limit >> 16) & 0x0F;
	// Combina as Flags com o Limit
	gdt[num].granularity |= gran & 0xF0;

	// Permissões
	gdt[num].access = access;
}

void init_gdt() {
	// Configura o ponteiro especial
	gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
	gp.base = (uint32_t)&gdt;

	// Segmento NULL (obrigatório, num = 0)
	set_gdt_entry(0, 0, 0, 0, 0);

	// Segmento de Código (num = 1) - Base 0, Limit 4GB, Executável
	set_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

	// Segmento de Dados (num = 2) - Base 0, Limit 4GB, Escrita
	set_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

	// Diz à CPU para aplicar a GDT
	gdt_flush((uint32_t)&gp);
}