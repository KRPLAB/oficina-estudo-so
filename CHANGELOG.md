# Changelog - Kernel x86 Multiboot

Todas as mudanças notáveis neste projeto serão documentadas neste arquivo.

O formato é baseado em [Keep a Changelog](https://keepachangelog.com/) e este projeto segue [Semantic Versioning](https://semver.org/).

## [Unreleased]

### Planned
- Implementar Multiboot2 header completo
- Suporte a GOP (Graphics Output Protocol)
- Linear Framebuffer rendering
- Paging manual (CR3, CR0.PG)
- Carregamento de fontes bitmap (PSF)
- Parser completo de tags Multiboot2

---

## [0.2.0] - 2026-06-04

### Added
- ✨ **Refactoring para UEFI Compatibility**
  - Linker atualizado para suportar endereço 2M (em vez de 1M)
  - Comentários explicativos adicionados ao linker.ld
  - Preparação para migração Multiboot2
  - Documentação técnica: ESTUDO_LINKER.md

- 📚 **Documentação Educacional Expandida**
  - TUTORIAL_LOADER_ADAPTADO.md: Comparação loader.s vs tutorial osdev.wiki
  - ANALISE_KERNEL_C.md: Deep dive em driver VGA terminal
  - ESTUDO_LINKER.md: Análise completa de linker script
  - docs/study/estudo_01.md: Multiboot 1→2, Linear Framebuffer, GOP
  - Seção 2.5 adicionada ao estudo_01 sobre linker requirements

### Changed
- Endereço base de 1M (0x0100000) para 2M (0x200000) no linker
  - Mais compatível com UEFI moderno
  - Mantém compatibilidade com BIOS via bootloader
  
- Separação de .text e .rodata com alinhamento BLOCK(4K):ALIGN(4K)
  - Preparação para paging futuro
  - Cada seção em limite de página (4 KiB)

- Estrutura de documentação reorganizada
  - README.md criado (este arquivo era RELATORIO.md)
  - CHANGELOG.md para histórico (novo)
  - docs/study/ para documentos de estudo detalhados

### Fixed
- Corrigido linker.ld: sintaxe de comentários multi-linha abertos
  - Linha 24: `. = 2M;` (estava `= 2M;`)
  - Linha 35: Comentário de .text fechado corretamente
  - Linha 88: Comentário de /DISCARD/ fechado corretamente

- Compilação sem warnings (maioria dos warnings suprimidos ou explicados)

### Technical Details

#### Multiboot Header (loader.s)
```asm
; Continua usando Multiboot 1
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)
; Próxima versão: Multiboot 2 (0xE85250D6)
```

#### Linker Script (linker.ld)
```ld
. = 2M;                           ; Endereço 2M (UEFI-safe)
.text BLOCK(4K) : ALIGN(4K)       ; Alinhado a página
.rodata BLOCK(4K) : ALIGN(4K)     ; Separado de .text
.data BLOCK(4K) : ALIGN(4K)       ; Com construtores C++
.bss BLOCK(4K) : ALIGN(4K)        ; Stack alocado aqui
```

#### Kernel (kernel.c)
- Mantém `myprintf()` original comentada (sem acesso a 0xb8000 sem paging)
- Serial port COM1 (já implementado na v0.1)
- Pronto para migração para Multiboot2

### Documentation

#### New Sections Added
- README.md: Visão geral completa do projeto
- CHANGELOG.md: Este arquivo
- ESTUDO_LINKER.md: Análise linker 1MB vs 2MB vs UEFI
- docs/study/estudo_01.md (Seção 2.5): Linker requirements Multiboot2

#### Key Insights Documented
- Por que 1M era padrão em BIOS (2000s-2010s)
- Por que 2M é melhor para UEFI moderno (2020s)
- Requisitos de alinhamento Multiboot2 no linker
- Impacto de BLOCK(4K):ALIGN(4K) em paging futuro
- C++ construtores e symbols start_ctors/end_ctors

---

## [0.1.0] - 2026-06-03

### Added
- ✨ **Kernel Barebones Funcional**
  - Bootloader assembly (loader.s) com Multiboot1 header
  - Kernel C simples (kernel.c) com myprintf()
  - Linker script (linker.ld) com suporte a construtores C++
  - Makefile para compilação completa

- 🔧 **Serial Port Debug Output**
  - Inicialização de COM1 (115200 baud, 8 bits)
  - Funções serial_init(), serial_putchar(), myprintf()
  - Funciona em QEMU com `-serial stdio`
  - Funciona em hardware real com cabo serial

- 🔄 **Process Heartbeat**
  - Loop infinito com incremento de contador
  - Prova de que kernel está vivo e executando
  - Debug via serial output

- 📚 **Documentação Técnica Inicial**
  - Backup do código Heartbeat (kernel.c.bak)

### Fixed
- **Critical Bug: Stack Pointer Não Inicializado**
  ```asm
  # ❌ Antes (causava crash imediato):
  mov $kernel_main, %esp     ; Coloca endereço da função em ESP
  
  # ✅ Depois (funciona):
  mov $kernel_stack, %esp    ; Coloca topo da stack alocada em .bss
  ```
  - Afetava: Qualquer `push` ou `call` causava crash
  - Impacto: Kernel não conseguia inicializar

### Known Issues Documented
- Acesso a 0xb8000 (VGA text mode) não funciona sem paging
  - Modo protegido sem paging não mapeia memória de vídeo
  - Serial output é alternativa funcional
  - Multiboot2 com GRUB2 resolverá na v0.3

- Hardware antigo (BIOS) vs moderno (UEFI) tem comportamentos diferentes
  - Máquinas antigas: kernel funciona com 1M
  - AMD Ryzen 7 5700X: precisa investigação (UEFI?)
  - Planeja-se migração para Multiboot2 para compatibilidade

### Commits
```
1241efc - Kernel funcional com serial port (QEMU)
          Corrige stack pointer, implementa serial output
          
fb98c17 - Kernel com heartbeat - vivo no QEMU
          Prova de conceito: kernel rodando com contador incrementando
```

### Build & Test
```bash
make                    # Compila kernel binário
qemu-system-i386 -kernel mykernel.bin -serial stdio

# Output esperado:
# Hello, The Kernel is alive!
# I implemented a simple terminal!
# This is a test of the terminal's ability to handle newlines.
# The terminal should correctly move to the next line after each newline character
# .
# If you see this text on separate lines, the terminal is working correctly!
# This concludes the terminal test.
```

### Technical Details

#### Stack Allocation (loader.s)
```asm
.section .bss
.space 2*1024*1024  ; 2 MiB de stack
kernel_stack:
```

#### Serial Output (kernel.c.bak)
```c
void serial_init(void) {
    outb(0x3F8 + 1, 0x00);    // Disable interrupts
    outb(0x3F8 + 3, 0x80);    // Set DLAB
    outb(0x3F8 + 0, 0x03);    // Divisor low byte (115200 baud)
    // ...
}
```

---

## Arquitetura de Versioning

- **MAJOR (X.0.0)**: Mudanças arquiteturais (novo bootloader, novo protocolo)
- **MINOR (0.X.0)**: Features novas (novos drivers, novos subsistemas)
- **PATCH (0.0.X)**: Bugfixes e documentação

### Status de Versões

| Versão | Data | Funcionalidade | Hardware |
|--------|------|----------------|----------|
| v0.1.0 | 2026-06-03 | Serial output, heartbeat | QEMU ✅, BIOS ✅ |
| v0.2.0 | 2026-06-04 | Linker 2M-ready, docs | QEMU ✅, BIOS ✅ |
| v0.3.0 | TBD | Multiboot2, GOP, LFB | QEMU ✅, UEFI 🔄 |
| v1.0.0 | TBD | Paging, drivers básicos | QEMU ✅, BIOS ✅, UEFI ✅ |

---

## Notas para Contribuidores

### Conventions
- Commits seguem [Conventional Commits](https://www.conventionalcommits.org/)
- Formato: `type(scope): description`
- Exemplos:
  - `feat(loader): Multiboot2 header support`
  - `fix(linker): Syntax error in comment`
  - `docs(readme): Add compilation instructions`
  - `refactor(kernel): Split myprintf into modules`

### Testing
- Sempre testar com: `qemu-system-i386 -kernel mykernel.bin -serial stdio`
- Esperado: Kernel inicia, imprime via serial, não crashes
- Para hardware real: teste em máquina com BIOS+serial

### Documentation
- Adicionar documentação em `docs/study/` para mudanças grandes
- Atualizar README.md e CHANGELOG.md para cada release
- Manter comentários inline no código (especialmente assembly)

---

## Referências Historicamente Importantes

### 2016 - Video Original
- Tutorial em vídeo mostra bootloader em assembly + kernel simples em C
- Usa Multiboot 1, modo texto VGA
- Funciona em BIOS legado
- Base deste projeto

### 2026 - osdev.wiki (Atual)
- Recomenda Multiboot 2 para compatibilidade moderna
- Enfatiza UEFI + GOP ao invés de BIOS + VGA
- Alinhamento de página (4 KiB) para paging
- Preparação para funcionar em máquinas modernas

### Plano de Atualização
- v0.3.0: Implementar Multiboot2 conforme 2026 wiki
- v0.4.0: Suporte a GOP + Linear Framebuffer
- v1.0.0: Paging implementado, funciona em UEFI nativa

---

**Última Atualização:** 2026-06-04

**Mantedor:** Kauan (projeto educacional)
