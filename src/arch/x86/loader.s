/* 
 * Declaração de constantes para o cabeçalho Multiboot (Padrão Multiboot 1)
 */
.set ALIGN,    1<<0             /* Alinha módulos carregados em fronteiras de páginas */
.set MEMINFO,  1<<1             /* Solicita ao bootloader o mapa de memória do sistema */
.set FLAGS,    ALIGN | MEMINFO  /* Combinação de flags do Multiboot */
.set MAGIC,    0x1BADB002       /* Número mágico que permite ao GRUB encontrar o cabeçalho */
.set CHECKSUM, -(MAGIC + FLAGS) /* Checksum para validar a assinatura do cabeçalho */

/*
 * Cabeçalho Multiboot que identifica este programa como um kernel.
 * Deve estar alinhado em 32 bits e localizado nos primeiros 8 KiB
 * do arquivo para que o bootloader consiga encontrá-lo.
 */
.section .multiboot
.align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

/*
 * Seção de Código Executável
 */
.section .text
.extern kernel_main
.extern call_constructors
.global loader
.global gdt_flush

# Ponto de entrada do Kernel
loader:
    /* Configura o ponteiro de pilha (ESP) apontando para o topo seguro da stack */
    mov $kernel_stack, %esp

    /* Executa os construtores globais do C++ antes de entrar no código principal */
    call call_constructors

    push %eax
    push %ebx

    call kernel_main


_stop:
    cli
    hlt
    jmp _stop

gdt_flush:
    # 1. Pega o paâmetro (ponteiro gb) passado pela função C
    mov 4(%esp), %eax

    # 2. Carrega o GDT usando o ponteiro
    lgdt (%eax)

    # 3. Recarrega os registradores de Dados (Segmento de dados = 0x10)
    # 0x10 é o 3º segmento do GDT (Índice 2 * 8 bytes = 16 = 0x10)
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # 4. Recarrega os registradores de Código (Segmento de código = 0x08)
    # O registrador de CS não pode ser alterado com 'mov'. Exige um salto longo (far jump)
    # 0x08 é o 1º segmento do GDT (Índice 1 * 8 bytes = 8 = 0x08)
    jmp $0x08, $flush_cs

flush_cs:
    ret # Volta para o código em C


.section .bss
.align 16
stack_bottom:
    .space 2*1024*1024; # 2MiB
kernel_stack:
