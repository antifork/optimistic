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


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <elf.h>


#define VERSION "2.0"
#define PATCH_MODE 0
#define ADDR_MODE 1


void            fatal(char *, ...);
void            init_opt(int, char **);
void            help();


typedef struct {
    struct stat     buf;
    unsigned char  *addr;
    unsigned long   virtual_addr;
    Elf32_Ehdr     *elfHeader;
    Elf32_Phdr     *elfProgram;
    int             fd;
    int             mode;
} OPT;


OPT             o;


int
main(int argc, char **argv)
{
    int             i;
    int found= 0;

    init_opt(argc, argv);

    o.addr =
	mmap(NULL, o.buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, o.fd,
	     0x00);
    if (o.addr == MAP_FAILED)
	fatal("mmap()");

    o.elfHeader = (Elf32_Ehdr *) o.addr;

    for (i = 0; i < o.elfHeader->e_phnum; ++i) {

	o.elfProgram =
	    (Elf32_Phdr *) (o.addr + o.elfHeader->e_phoff +
			    sizeof(Elf32_Phdr) * i);

	switch (o.mode) {

	case ADDR_MODE:
	    if (o.elfProgram->p_vaddr <= o.virtual_addr &&
		o.elfProgram->p_vaddr + o.elfProgram->p_filesz >=
		o.virtual_addr) {
		printf("virtual address %#lx has file offset %#lx\n",
		       o.virtual_addr,
		       o.elfProgram->p_offset + (o.virtual_addr -
						 o.elfProgram->p_vaddr));
                ++found;
                }   
	    break;

	case PATCH_MODE:
	    if (o.elfProgram->p_flags & PF_X) {
		printf("SEGMENT %d is executable, PATCHING!\n", i);
		o.elfProgram->p_flags |= (PF_W | PF_R);
	    }
	    break;

	}
    }

    munmap(o.addr, o.buf.st_size);
    close(o.fd);

    if (!o.mode)
	printf("ELF patched, have fun ;)\n\n");
    else
      if(!found)
        printf("Not found.\n");

    return 0;
}


void
init_opt(int argc, char **argv)
{
    int             c;

    o.fd = -1;
    o.mode = -1;

    while ((c = getopt(argc, argv, "f:a:ph")) != EOF)
	switch (c) {

	case 'f':
	    if (o.fd != -1)
		fatal("file already opened");
	    if ((o.fd = open(optarg, O_RDWR)) < 0)
		fatal("unable to open file with flags O_RDWR");
	    if (stat(optarg, &o.buf) < 0)
		fatal("stat()");
	    break;

	case 'a':
	    if ((o.virtual_addr = strtoul(optarg, NULL, 0)) == ULONG_MAX)
		fatal("address overflow");
	    o.mode = ADDR_MODE;
	    break;

	case 'p':
	    o.mode = PATCH_MODE;
	    break;

	case 'h':
	    help();
	    break;

	default:
	    fatal("try -h");
	}

    if (o.fd < 0)
	fatal("filename required");

    if (o.mode < 0)
	fatal("-a or -p needed");

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


void
help()
{
    printf("elfo v%s by xenion@acidlife.com\n\n", VERSION);
    printf("USAGE: elfo [options]\n\n");
    printf("-f filename                         your input ELF file\n");
    printf("-p                                  PATCH MODE\n");
    printf("-a <virtual addr>                   ADDR MODE\n");
    printf("-h                                  this\n\n");
    printf
	("[PATCH MODE]: Change ELF Segment flags to allow self modifying code\n");
    printf("[ADDR MODE ]: Translate virtual address to file offset\n\n");
    exit(1);

}


/*
 * EOF 
 */
