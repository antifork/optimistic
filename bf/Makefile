# bf makefile - just type make 

CC=cc
CFLAGS=-O -Wall -DNOPROTO -c -o
AR=             ar r


all:
	$(CC) $(CFLAGS) bf_skey.o bf_skey.c
	$(CC) $(CFLAGS) bf_ecb.o bf_ecb.c
	$(CC) $(CFLAGS) bf_enc.o bf_enc.c
	$(CC) $(CFLAGS) bf_cfb64.o bf_cfb64.c
	$(CC) $(CFLAGS) bf_ofb64.o bf_ofb64.c
	rm -f blowfish.a
	$(AR) blowfish.a bf_skey.o bf_ecb.o bf_enc.o bf_cfb64.o bf_ofb64.o 

clean:
	rm -f *.o *.a
