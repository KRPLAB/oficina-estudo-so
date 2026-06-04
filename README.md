# Kernel x86 Multiboot - Projeto de Estudo

Um kernel barebones em x86 32-bit desenvolvido como projeto educacional, seguindo tutoriais do [osdev.wiki](https://osdev.wiki/) (2016-2026) e vídeos de referência. O projeto documenta a evolução de um bootloader simples para um kernel com suporte a Multiboot 2 e Linear Framebuffer.

## Status Atual

**Funcional em QEMU com Serial Output**
- Kernel inicializa sem crashes
- Stack corretamente configurada
- Serial port COM1 para debug output
- Multiboot 1 header validado

**Limitações Atuais**
- Acesso a `0xb8000` (VGA text mode) requer paging ou GRUB2 Multiboot2
- Sem suporte gráfico (pixel rendering)
- Hardware antigo vs moderno tem comportamentos diferentes

## Estrutura do Projeto

```
oficina-estudo/
├── loader.s              # Bootloader em Assembly (entry point)
├── kernel.c              # Kernel principal em C
├── linker.ld             # Script de ligação (linker)
├── Makefile              # Automação de build
│
├── README.md             # Este arquivo
├── CHANGELOG.md          # Histórico de mudanças
│
├── docs/study/
│   └── estudo_01.md      # Estudo: Multiboot 1 → Multiboot 2
│   └── estudo_02.md      # (Próximo) Estudo: Paging e Segmentação
│
├── backup/
│   └── kernel.c.bak      # Backup do kernel original
└── .gitignore            # Configuração Git
```

## Como Compilar

### Requisitos

```bash
# Instalar cross-compiler e ferramentas
sudo apt-get install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo
sudo apt-get install gcc-i686-elf binutils-i686-elf i686-elf-tools

# Ou, alternativa mais simples (Debian/Ubuntu):
sudo apt-get install gcc-i686-linux-gnu binutils-i686-linux-gnu

# QEMU para testes
sudo apt-get install qemu-system-x86
```

### Compilação

```bash
# Compilar kernel binário
make

# Compilar e gerar ISO bootável (requer GRUB2)
make mykernel.iso

# Limpar objetos
make clean

# Instalar no /boot (requer sudo)
make install
```

## Como Testar

### No QEMU (Recomendado)

```bash
# Executar kernel no QEMU com redirecionamento serial
qemu-system-i386 -kernel mykernel.bin -serial stdio

# Esperado:
# Kernel inicializa, imprime mensagens na serial, entra em loop infinito
```

### Em VirtualBox

```bash
# Se tiver arquivo ISO:
# 1. Criar máquina virtual (Linux 32-bit)
# 2. Montar mykernel.iso como CD de boot
# 3. Iniciar máquina
```

### Hardware Real (Avançado)

```bash
# Gravar ISO em USB:
sudo dd if=mykernel.iso of=/dev/sdX bs=4M
sudo sync

# Fazer boot a partir do USB
# (BIOS/UEFI → Boot from USB)
```

## Estrutura do Código

### loader.s (Assembly)

Responsabilidades:
- **Multiboot Header**: Padrão Multiboot 1 (header mágico)
- **Inicialização de CPU**: Stack pointer, segmentos básicos
- **Chamada de kernel_main**: Passa argumentos (magic, multiboot_info)

```asm
ENTRY(loader)
  mov $kernel_stack, %esp      # Configura stack
  call call_constructors       # Construtores C++ (se houver)
  push %ebx                    # Argumentos Multiboot
  push %eax
  call kernel_main
  cli
  hlt                          # Halt infinito
```

### kernel.c (C)

Responsabilidades:
- **main()**: Ponto de entrada C
- **Serial I/O**: Funções `serial_init()`, `serial_putchar()`, `myprintf()`
- **Driver de Vídeo**: Futuro (atualmente não funciona sem paging)

```c
void kernel_main(unsigned int magic, multiboot_info_t* mbi) {
    serial_init();
    myprintf("Kernel is alive!\n");
    while(1);
}
```

### linker.ld (Linker Script)

Responsabilidades:
- **ENTRY point**: Define símbolo `loader` como entrada
- **Seções**: Agrupa `.multiboot`, `.text`, `.rodata`, `.data`, `.bss`
- **Símbolos globais**: `start_ctors`, `end_ctors` (construtores C++)
- **Endereço base**: 2M (UEFI-compatível) ou 1M (BIOS-compatível)

```ld
ENTRY(loader)
SECTIONS {
    . = 2M;                    # Endereço de carregamento
    .text { *(.multiboot) *(.text) }
    .data { ... }
    .bss { ... }
}
```

## Problemas Conhecidos e Soluções

### Problema 1: Stack Pointer Não Inicializado

**Sintoma:** Kernel reinicia imediatamente após GRUB.

**Causa:** `mov $kernel_main, %esp` (colocava endereço de função, não da stack).

**Solução:** `mov $kernel_stack, %esp` (aponta para área allocada em `.bss`).

**Status:** Corrigido

---

### Problema 2: Acesso a 0xb8000 em Modo Protegido

**Sintoma:** Kernel funciona (serial output), mas tela fica escura.

**Causa:** Sem paging/segmentação, `0xb8000` não está mapeado no espaço de endereçamento virtual.

**Soluções Possíveis:**
1. Usar GRUB2 + Multiboot2 (bootloader já configura paging)
2. Implementar paging manual no kernel
3. Usar serial output (já implementado)

**Status:** Pendente (Multiboot2 mitiga)

---

### Problema 3: Hardware Antigo vs Moderno

**Sintoma:** Código funciona em máquinas antigas (BIOS), mas não em AMD Ryzen 7 5700X (UEFI).

**Possíveis Causas:**
- UEFI não fornece modo texto VGA
- IOMMU/virtualization moderna interfere
- Bootloader diferente (UEFI vs Legacy)

**Solução:** Implementar Multiboot2 + GOP (Graphics Output Protocol).

**Status:** Em análise

---

### Problema 4: Warnings do Linker

**Sintoma:**
```
ld: aviso: secção .note.GNU-stack em falta implica uma pilha executável
```

**Causa:** Assembly não marca stack como não-executável.

**Solução:** Adicionar ao final de `loader.s`:
```asm
.section .note.GNU-stack,"",@progbits
```

**Status:** Opcional (barebones pode ignorar)

---

## Próximos Passos

### Fase 1: Multiboot2 (Próximo)
- [ ] Atualizar header em `loader.s` para Multiboot2
- [ ] Implementar parser de tags Multiboot2
- [ ] Testar acesso a `0xb8000` via GRUB2

### Fase 2: Paging
- [ ] Implementar tabelas de página (CR3, CR0.PG)
- [ ] Mapear memória física → virtual
- [ ] Saltar para código em higher half

### Fase 3: Gráficos
- [ ] Linear Framebuffer (GOP)
- [ ] Carregamento de fontes bitmap (PSF)
- [ ] Console gráfico

### Fase 4: Drivers
- [ ] Teclado (PS/2)
- [ ] Mouse
- [ ] Disco (ATA/SATA)

## Documentação de Estudos

### Documentos Inclusos

1. **TUTORIAL_LOADER_ADAPTADO.md** - Documentação de estudo comparando loader.s (2016, Victor Angelman) e tutorial osdev.wiki 
2. **ANALISE_KERNEL_C.md** - Análise detalhada de myprintf (2016, Victor Angelman) → terminal driver (2026, osdev.wiki)
3. **ESTUDO_LINKER.md** - Deep dive em linker.ld, BLOCK/ALIGN, C++ construtors support
4. **estudo_01.md** - Multiboot 1 → Multiboot 2, Linear Framebuffer (completo)

### Como Estudar

```
Ordem Recomendada:
1. Leia TUTORIAL_LOADER_ADAPTADO.md (entender loader.s)
2. Leia ANALISE_KERNEL_C.md (entender kernel.c)
3. Leia ESTUDO_LINKER.md (entender linker.ld)
4. Leia docs/study/estudo_01.md (próximo passo: Multiboot2)
```

## 🔗 Referências

### Oficiais
- [osdev.wiki - Barebones](https://osdev.wiki/wiki/Barebones)
- [osdev.wiki - Multiboot](https://osdev.wiki/wiki/Multiboot)
- [Multiboot Specification 2.0](https://www.gnu.org/software/grub/manual/multiboot2/)

### Tutoriais
- [James Molloy's Kernel Development Tutorial](http://www.jamesmolloy.co.uk/tutorial_html/)
- [OSDev Bare Bones (Assembly)](https://osdev.wiki/wiki/Bare_Bones_with_NASM)

### Referência x86
- [Intel x86 Assembly Language Reference](https://chortle.ccsu.edu/AssemblyTutorial/)
- [Intel 64 and IA-32 Architectures Software Developer Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)

## 📝 Licença

Este projeto é para fins educacionais. Use, modifique e distribua livremente.

## 👤 Autor

Desenvolvido como projeto de estudo em 2026, baseado em tutoriais de 2016-2026.

---

**Última atualização:** 2026-06-04

**Status de Build:** ✅ Compilação bem-sucedida (`make`)

**Status de Teste:** ✅ Funcional em QEMU com serial output
