/* Ponto de entrada do kernel. */
ENTRY(loader)

/* Define explicitamente o formato de saída binária e a arquitetura alvo */
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
    /* Endereço base de carregamento em 2 MB para garantir compatibilidade com UEFI */
    . = 2M;

    /* Seção .text: código executável e dados read-only */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text*)
    }

	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

    /* Dados inicializados de leitura e escrita */
	.data BLOCK(4K) : ALIGN(4K)
    {
        /* Mapeamento de ponteiros de construtores para inicialização POO em C++ */
        start_ctors = .;
        KEEP(*( .init_array ));
        KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
        KEEP(*(.ctors)) /* Garante retrocompatibilidade com geradores de código antigos */
        end_ctors = .;

        *(.data)
    }

    /* Seção .bss: dados não inicializados (read-write)
       CONTEÚDO:
       - Variáveis globais não inicializadas
       - Stack do kernel   
     */
	.bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }

    /* Seção /DISCARD/: descarta seções desnecessárias
       CONTEÚDO DESCARTADO:
       - *(.fini_array*) : finalizadores (não usados neste kernel)
       - *(.comment)     : comentários do compilador (lixo) */
    /DISCARD/ :
    {
        *(.fini_array*)
        *(.dtors*)
        *(.comment)
    }
}