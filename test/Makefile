CC := gcc

all: minijson.h minijson.c test.c jsonlink
	$(CC) -g minijson.c test.c json.o -o test

clean:
	rm -f json.o test

jsonlink: test.json
	ln -f -s test.json json
	objcopy -I binary -O elf64-x86-64 -B i386 json json.o
