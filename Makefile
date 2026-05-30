CFLAGS = -m32 -nostdlib -fno-builtin -fno-leading-underscore
CXXFLAGS = $(CFLAGS) -fno-use-cxa-atexit -fno-rtti -fno-exceptions
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = loader.o kernel.o

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

%.o: %.s
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

install: mykernel.bin
	sudo cp $< /boot/mykernel.bin

clean:
	rm -f $(objects) mykernel.bin