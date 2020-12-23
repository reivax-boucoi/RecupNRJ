Compiler avr-gcc :

Install WinAVR https://sourceforge.net/projects/winavr/files/


Upload using AVRdude (command line tool), GUI available here : https://blog.zakkemble.net/avrdudess-a-gui-for-avrdude/ to check programmer and µC connnections


To compile :

avr-gcc -g -Os -mmcu=attiny85 -c main.c
avr-gcc -g -mmcu=attiny85 -o main.elf main.o
avr-objcopy -j .text -j .data -O ihex main.elf main.hex

To upload :

avrdude -P usb -p t85 -c usbtiny -U flash:w:main.hex

Or use the provided makefile