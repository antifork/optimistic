/*
 *
 * $Id$
 *
 * ---------------------------------------------------------------------------
 * No part of this project may be used to break the law, or to cause damage of
 * any kind. And I'm not responsible for anything you do with it.
 * ---------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (by Poul-Henning Kamp, Revision 42):
 * <xenion@acidlife.com> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.
 * xenion ~ Dallachiesa Michele
 * ---------------------------------------------------------------------------
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <unistd.h>
#include "../bf/blowfish.h"


#define VERSION "2.1"
#define MIN(x,y) x<y?x:y
#define BUFLEN 128


void            fatal(char *, ...);
void            init_opt(int, char **);
void            help();


typedef struct {
    BF_KEY          key;
    int             enc;
    int             num;
    unsigned char   ivec[8];
} BF;


typedef struct {
    BF              bf;
    FILE           *f;
    unsigned long   off_s;
    unsigned long   off_e;
} OPT;


OPT             o;


int
main(int argc, char **argv)
{
    unsigned long   i;
    unsigned char   buf[BUFLEN];
    int             bytes;


    init_opt(argc, argv);

    printf("%scrypting file from <%#lx> to <%#lx+%#lx>..\n",
	   o.bf.enc == BF_ENCRYPT ? "En" : "De", o.off_s, o.off_s,
	   o.off_e);

    o.off_e += o.off_s;

    if ((fseek(o.f, o.off_s, SEEK_SET)) < 0)
	fatal("seek()");

    memset(o.bf.ivec, '\0', 8);
    o.bf.num = 0;

    for (i = o.off_s; i < o.off_e; i += bytes) {
	bytes = MIN(o.off_e - i, BUFLEN);

	if (fread(buf, 1, bytes, o.f) != bytes)
	    fatal("unexected end of file");

	BF_cfb64_encrypt(buf, buf, bytes, &o.bf.key, o.bf.ivec, &o.bf.num,
			 o.bf.enc);

	if (fseek(o.f, i, SEEK_SET) < 0)
	    fatal("unable to seek to %#lx", i);
	if (fwrite(buf, 1, bytes, o.f) != bytes)
	    fatal("fwrite()");
    }

    fclose(o.f);
    printf("Done.\n");
    return 0;
}


void
init_opt(int argc, char **argv)
{
    int             c;
    int             key;


    o.f = NULL;
    o.off_s = 0;
    o.off_e = 0;
    o.bf.enc = -1;
    key = -1;

    while ((c = getopt(argc, argv, "f:s:l:k:edh")) != EOF)
	switch (c) {

	case 'f':
	    if ((o.f = fopen(optarg, "r+")) == NULL)
		fatal("unable to open file");
	    break;

	case 's':
	    if ((o.off_s = strtoul(optarg, NULL, 0)) == ULONG_MAX)
		fatal("-s: overflow");
	    break;

	case 'l':
	    if ((o.off_e = strtoul(optarg, NULL, 0)) == ULONG_MAX)
		fatal("-l: overflow");
	    break;

	case 'k':
	    BF_set_key(&o.bf.key, strlen(optarg), optarg);
	    ++key;
	    break;

	case 'e':
	    o.bf.enc = BF_ENCRYPT;
	    break;

	case 'd':
	    o.bf.enc = BF_DECRYPT;
	    break;

	case 'h':
	    help();
	    break;

	default:
	    fatal("try -h");
	}

    if (o.f == NULL)
	fatal("file required");

    if (key < 0)
	fatal("key required");

}


void
help()
{
    printf("Bud v%s(using Blowfish cipher) by xenion@acidlife.com\n\n",
	   VERSION);
    printf("USAGE: [options]\n\n");
    printf("-f filename                         file to (de|en)crypt\n");
    printf
	("-s begin-offset                     start from SEEK_SET+offset\n");
    printf
	("-l length                           to SEEK_SET+offset+length\n");
    printf("-k key                              ..your key :)\n");
    printf("-e                                  encrypt\n");
    printf("-d                                  decrypt\n\n");
    exit(1);

}


void
fatal(char *pattern, ...)
{

    va_list         ap;
    va_start(ap, pattern);

    vfprintf(stderr, pattern, ap);
    fprintf(stderr, "; exit forced.\n");

    va_end(ap);

    exit(-1);

}

/*
 * EOF 
 */
