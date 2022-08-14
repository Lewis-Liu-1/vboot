

all:
	arm-linux-gcc -mabi=aapcs-linux -mno-thumb-interwork -Os -Wall -c head.S 244x_lib.c nand.c main.c
	arm-linux-ld -T mem.lds -Bstatic head.o 244x_lib.o nand.o main.o 
	arm-linux-objcopy -O binary -S a.out vboot.bin -R .comment -R .stab -R .stabstr
	rm *.o a.out

clean:
	rm vboot.bin
