CFLAGS = -c -std=gnu99 -ffreestanding -Wno-unused-parameter -Wno-unused-variable -O0 -Wall -Wextra -g
ASFLAGS = -f elf32
LDFLAGS = -r -O0 -s -nostdlib
# -fPIC -shared ======> shared lib
.PHONY: clean
build: testmod
clean:
	rm -rf test/module.o
testmod: test/module.o
	$(LINKER) -T test/link.ld $(LDFLAGS) -o testmod.so test/module.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<