// cc sample.c bf/blowfish.a -o sample

#include <stdio.h>
#include <stdlib.h>
#include "bf/blowfish.h"

#define F_LEN 24

void
secret()
{
    printf("Ya, this is my secret!\n");
}


int
main(int argc, char **argv, char **envp)
{
    struct {
	BF_KEY          key;
	int             enc;
	int             num;
	unsigned char   ivec[8];
    } bf;


    BF_set_key(&bf.key, strlen(getenv("KEY")), getenv("KEY"));
    memset(bf.ivec, '\0', 8);
    bf.num = 0;

    BF_cfb64_encrypt((unsigned char *) secret, (unsigned char *) secret,
		     F_LEN, &bf.key, bf.ivec, &bf.num, BF_DECRYPT);

    secret();
    return 0;
}
