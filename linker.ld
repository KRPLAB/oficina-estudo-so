/* Ponto de entrada do kernel. O bootloader/linker pulará para este símbolo.
   NOTA: Nesta versão manterei "loader" em vez de "_start" (tutorial osdev.wiki usa "_start").
   Ambos funcionam, mas "loader" é mais descritivo. */
ENTRY(loader)

/* Define o formato de saída como ELF 32-bit para arquitetura i386.
   NOTA: Tutorial osdev.wiki 2026 não especifica isso explicitamente,
   pois deixa o compilador (gcc) decidir. Nesta versão opta-se por ser explícito
   para evitar confusões, já que o formato e arquitetura são cruciais para um kernel.
   então precisa ser explícito. */
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
    /* Endereço base do kernel: opta-se por 2 MB (0x200000) para garantir compatibilidade
       com UEFI e espaço suficiente.
       HISTÓRICO:
       - Tradicionalmente: 1 MB (0x100000) - garantidamente disponível em BIOS
       - Tutorial osdev.wiki 2026: recomenda 2 MB (0x200000) para UEFI
       - Código 2016 Canal "Write your own Operating System" de Victor Angelman: 1 MB (compatível com BIOS/VirtualBox)
       
        NOTA: O comment no tutorial 2026 alerta que UEFI é mais complicado.
              Por esta razão opta-se seguir recomendação de 2026 e mudar para `. = 2M;`*/
    . = 2M;

    /* Seção .text: código executável e dados read-only
       CONTEÚDO:
       - *(.multiboot)  : header multiboot (DEVE ser no início!)
       - *(.text*)      : código do programa
       - *(.rodata)     : constantes read-only (ex: strings)
       
       DIFERENÇA DO TUTORIAL 2026:
       - Tutorial separa .text e .rodata com BLOCK(4K) : ALIGN(4K)
       
        NOTA: Alinhamento é importante para desempenho e compatibilidade, especialmente em UEFI.
              O tutorial 2026 alinha ambas as seções com BLOCK(4K) : ALIGN(4K), o que é uma prática comum.
       */
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

    /* Seção .data: dados inicializados (read-write)
       CONTEÚDO ESPECIAL:
       - start_ctors : símbolo marcando início dos construtores C++
       - .init_array : array de ponteiros para construtores globais C++
       - SORT_BY_INIT_PRIORITY : ordena construtores por prioridade
       - end_ctors : símbolo marcando fim dos construtores
       
       NOTA: Essa seção é ESPECÍFICA para suporte a C++ com construtores globais!
       O tutorial osdev.wiki 2026 (apenas C) não menciona isso.
       Fica como decisão de design incluir suporte a C++ desde o início, pois é 
       comum em kernels modernos.
       
       O loader.s chama call_constructors() antes de kernel_main(),
       que itera sobre [start_ctors, end_ctors) e chama cada construtor. */
	.data BLOCK(4K) : ALIGN(4K)
    {
        start_ctors = .;
        KEEP(*( .init_array ));
        KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
        end_ctors = .;

        *(.data)
    }

    /* Seção .bss: dados não inicializados (read-write)
       CONTEÚDO:
       - Variáveis globais não inicializadas
       - Stack do kernel (veja loader.s: kernel_stack)
       
       DIFERENÇA:
       - Tutorial 2026 alinha com BLOCK(4K) : ALIGN(4K)
       - Você não alinha explicitamente (funciona, mas menos seguro)
       
       NOTA: Em teoria .bss não é alocado no arquivo (só marcado como espaço),
       o que torna a imagem do kernel menor. */
	.bss BLOCK(4K) : ALIGN(4K)
    {
        *(.bss)
    }

    /* Seção /DISCARD/: descarta seções desnecessárias
       CONTEÚDO DESCARTADO:
       - *(.fini_array*) : finalizadores (não usados neste kernel)
       - *(.comment)     : comentários do compilador (lixo) */
    /DISCARD/ :
    {
        *(.fini_array*)
        *(.comment)
    }
}