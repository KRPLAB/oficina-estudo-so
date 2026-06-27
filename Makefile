CFLAGS = -m32 -nostdlib -fno-builtin -fno-leading-underscore \
         -Iinclude

CXXFLAGS = $(CFLAGS) -fno-use-cxa-atexit -fno-rtti -fno-exceptions
ASPARAMS = --32
LDPARAMS = -melf_i386

OBJDIR = build/obj

OBJECTS = \
	$(OBJDIR)/loader.o \
	$(OBJDIR)/gdt.o \
	$(OBJDIR)/port.o \
	$(OBJDIR)/kernel.o

$(OBJDIR)/kernel.o: src/kernel.c
	mkdir -p $(OBJDIR)
	gcc $(CFLAGS) -c -o $@ $<

$(OBJDIR)/gdt.o: src/arch/x86/gdt.c
	mkdir -p $(OBJDIR)
	gcc $(CFLAGS) -c -o $@ $<

$(OBJDIR)/port.o: src/arch/x86/port.cpp
	mkdir -p $(OBJDIR)
	g++ $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/loader.o: src/arch/x86/loader.s
	mkdir -p $(OBJDIR)
	as $(ASPARAMS) -o $@ $<

build/mykernel.bin: linker.ld $(OBJECTS)
	mkdir -p build
	ld $(LDPARAMS) -T $< -o $@ $(OBJECTS)

install: build/mykernel.bin
	sudo cp $< /boot/mykernel.bin

clean:
	rm -rf build mykernel.iso iso

mykernel.iso: build/mykernel.bin
	mkdir -p iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

run: mykernel.iso
	(killall VirtualBoxVM && sleep 1) || true
	VirtualBoxVM --startvm "myOS" --iso $<