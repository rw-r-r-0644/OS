# OS  
## I did not find a better name for it yet.  

It's pretty bad for now but hopefully it will be fixed sometime.  
heap is currently randomly causing page faults. there must be something weird with it.  
  
(informations were taken from the osdev wiki, the little book about kernel development and jamesm's tutorials)  

### How to build  
#### Requirements:  
- GNU make  
- i386-elf gcc toolchain (you can find one here: http://wiki.osdev.org/GCC_Cross-Compiler#Prebuilt_Toolchains)  
- nasm  
- grub and grub-mkrescue  
#### Build instructions:  
1. Download the main repo  
2. Run "make" in the root folder  
