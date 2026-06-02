void myprintf(char* str) {
    unsigned short* video_memory = (unsigned short*) 0xB8000;
    for(int i = 0; str[i] != '\0'; i++)
        video_memory[i] = (video_memory[i] & 0xFF00) | str[i]; 
}

typedef void (*constructor)();

extern constructor* start_ctors;
extern constructor* end_ctors;

extern void call_constructors() {
    for(constructor* i = start_ctors; i != end_ctors; i++)
        (*i)();
}

void kernel_main(void* multiboot_structure, unsigned int magicnumber) {
    myprintf("Kernel is alive!\n");
    
    while(1);
}