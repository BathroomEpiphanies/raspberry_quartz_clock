CFLAGS += -DVERSION_DATE="\"$(shell date +'%Y-%m-%d')\""

all: main.hex


%.o: %.c %.h
	avr-gcc $(CFLAGS) -std=c99 -g -Os -mmcu=attiny1614 -c -o $@ $<


main.elf: main.o control.o uart.o
	avr-gcc -g -mmcu=attiny1614 -o $@ $^


main.hex: main.elf
	avr-objcopy -O ihex $< $@
	avr-size --target ihex $@


clean:
	rm -rf *.eep
	rm -rf *.elf
	rm -rf *.lss
	rm -rf *.lst
	rm -rf *.map
	rm -rf *.sym
	rm -rf *.o


distclean: clean
	rm -rf *.hex
