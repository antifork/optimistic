CC=cc
CFLAGS= -Wall

all: clean mkbindir blowfish bud elfo 

mkbindir:
	mkdir bin

blowfish:
	cd bf && make all

bud:	
	$(CC) $(CFLAGS) src/bud.c bf/blowfish.a -o bin/bud

elfo:
	$(CC) $(CFLAGS) src/elfo.c -o bin/elfo

clean:
	cd bf && make clean
	rm -rf bin 
