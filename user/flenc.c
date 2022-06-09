#include "kernel/types.h"
#include "kernel/module.h"
#include "user.h"

char key;
uint encrypted_inum[500];

void enc(void *x, uint offset) {
    uint *gencrypted_inum = (uint*)((char*)encrypted_inum + offset);
    struct dat_params *params = (struct dat_params*)x;
    if(gencrypted_inum[params->inum] == 0)
        gencrypted_inum[params->inum] = 1;

    for(int j = 0; j < params->n; j++) {
        params->src[j] ^= *(char*)globl(&key, offset);
    }
}

void dec(void *x, uint offset) {
    uint *gencrypted_inum = (uint*)((char*)encrypted_inum + offset);
    struct dat_params *params = (struct dat_params*)x;
    if(gencrypted_inum[params->inum] == 0) return;
    params->consputc(params->inum);

    for(int j = 0; j < params->n; j++) {
        params->src[j] ^= *(char*)globl(&key, offset);
    }
}

int
main(void)
{
    // read enc key
    int fd;
    if((fd = open("e", 0)) < 0){
        printf("cannot open enc config file\n");
        exit();
    }
    read(fd, &key, sizeof(key));
    close(fd);

    // prep module
    struct module mod[2];

    mod[0].func = &enc;
    mod[0].hookID = ENC;
    char name1[6] = "flenc";
    strcpy(mod[0].name, name1);

    mod[1].func = &dec;
    mod[1].hookID = DEC;
    char name2[6] = "fldec";
    strcpy(mod[1].name, name2);

    int err = addmod(2, mod);
    if (err == -1)
        printf("module already exists\n");
    if (err == -2) 
        printf("hook position filled up\n");
    if (err == -3)
        printf("wrong hook identifier\n");
	exit();
}
