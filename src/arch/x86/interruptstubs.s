.section .text

.extern handle_interrupt

.global interrupt_number
interrupt_number: .long 0

# ------------------------------------------------------------
# Macros para exceções (0..31)
# ------------------------------------------------------------
.macro ISR_NOERR num
.global isr_stub_\num
isr_stub_\num:
	movl $\num, interrupt_number	# salva o número da interrupção
	pushl $0						# código de erro fictício (para uniformidade)
	jmp int_common
.endm

.macro ISR_ERR num
.global isr_stub_\num
isr_stub_\num:
	movl $\num, interrupt_number	# salva o número da interrupção
	jmp int_common
.endm

# ------------------------------------------------------------
# Macros para IRQs (0..15, remapeadas para 0x20+)
# ------------------------------------------------------------
.set IRQ_BASE, 0x20

.macro IRQ num, irq_num
.global irq_stub_\num
irq_stub_\num:
	movl $\irq_num, interrupt_number	# salva o número da interrupção
	pushl $0							# código de erro
	jmp int_common
.endm

# ------------------------------------------------------------
# Geração das funções
# ------------------------------------------------------------
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_ERR   30
ISR_NOERR 31

# IRQs
IRQ  0, IRQ_BASE + 0
IRQ  1, IRQ_BASE + 1
IRQ  2, IRQ_BASE + 2
IRQ  3, IRQ_BASE + 3
IRQ  4, IRQ_BASE + 4
IRQ  5, IRQ_BASE + 5
IRQ  6, IRQ_BASE + 6
IRQ  7, IRQ_BASE + 7
IRQ  8, IRQ_BASE + 8
IRQ  9, IRQ_BASE + 9
IRQ 10, IRQ_BASE + 10
IRQ 11, IRQ_BASE + 11
IRQ 12, IRQ_BASE + 12
IRQ 13, IRQ_BASE + 13
IRQ 14, IRQ_BASE + 14
IRQ 15, IRQ_BASE + 15

# ------------------------------------------------------------
# Handler comum
# ------------------------------------------------------------
int_common:
    # Salvar todos os registradores de propósito geral
    pusha
    pushl %ds           # salvar segmentos de dados
    pushl %es           # Salvar segmentos de dados adicionais
    pushl %fs
    pushl %gs

    # Carregar segmentos de kernel
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    # Chamar handle_interrupt(uint8_t interrupt, uint32_t esp)
    pushl %esp                # ponteiro atual da pilha
    pushl (interrupt_number)  # número da interrupção (32 bits)
    call handle_interrupt
    addl $8, %esp

    movl %eax, %esp           # restaurar ponteiro da pilha (retorno de handle_interrupt)

    # Restaurar registradores
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

    # Remover código de erro (real ou dummy) antes do iret
    addl $4, %esp
    iret

# ------------------------------------------------------------
# Handler "ignore" - simplesmente retorna
# ------------------------------------------------------------
.global ignore_int
ignore_int:
    iret

# ------------------------------------------------------------
# Tabela de stubs (para as exceções)
# ------------------------------------------------------------
.section .data
.global isr_stub_table
isr_stub_table:
	.long isr_stub_0
	.long isr_stub_1
	.long isr_stub_2
    .long isr_stub_3
    .long isr_stub_4
    .long isr_stub_5
    .long isr_stub_6
    .long isr_stub_7
    .long isr_stub_8
    .long isr_stub_9
    .long isr_stub_10
    .long isr_stub_11
    .long isr_stub_12
    .long isr_stub_13
    .long isr_stub_14
    .long isr_stub_15
    .long isr_stub_16
    .long isr_stub_17
    .long isr_stub_18
    .long isr_stub_19
    .long isr_stub_20
    .long isr_stub_21
    .long isr_stub_22
    .long isr_stub_23
    .long isr_stub_24
    .long isr_stub_25
    .long isr_stub_26
    .long isr_stub_27
    .long isr_stub_28
    .long isr_stub_29
    .long isr_stub_30
    .long isr_stub_31