#include "kernel/types.h"
#include "kernel/module.h"
#include "user.h"

uint zipped_inum[500];
char buf[512];
char comp_param;

void enc(void *x, uint offset) {
    uint *gzipped_inum = (uint*)((char*)zipped_inum + offset);
    char *gbuf = (char*)(buf + offset);
    struct dat_params *params = (struct dat_params*)x;
    // TODO POPRAVI COUNT
    char count = '0';
    int j = 0;
    for(int i = 1; i < *params->n; i++) {
        if(params->src[i] != params->src[i - 1] || count == *(char*)globl(comp_param, offset)) {
            gbuf[j++] = params->src[i - 1];
            gbuf[j++] = count + 1;
            count = '0';
        } else {
            count++;
        }
    }
    gbuf[j++] = params->src[*params->n - 1];
    gbuf[j++] = count + 1;
    if (j >= *params->n) return;
    
    for(int i = 0; i < j; i++) {
        params->src[i] = gbuf[i];
    }
    *params->n = j;
    if(gzipped_inum[params->inum] == 0)
        gzipped_inum[params->inum] = *params->n;
}

void dec(void *x, uint offset) {
    uint *gzipped_inum = (uint*)((char*)zipped_inum + offset);
    char *gbuf = (char*)(buf + offset);
    struct dat_params *params = (struct dat_params*)x;
    if(gzipped_inum[params->inum] == 0) return;

    int j = 0;
    for(int i = 0; i < *params->n; i+=2) {
        int x = params->src[i + 1] - '0';
        while(x--) {
            gbuf[j++] = params->src[i];
        } 
    }
    for(int i = 0; i < j; i++) {
        params->src[i] = gbuf[i];
    }
}

void restoresz(void *x, uint offset) {
    uint *gzipped_inum = (uint*)((char*)zipped_inum + offset);
    struct dat_params *params = (struct dat_params*)x;
    char *gbuf = (char*)(buf + offset);
    if(gzipped_inum[params->inum] != 0){
        *params->n = gzipped_inum[params->inum];
    }
}

int
main(void)
{
    // read enc key
    int fd;
    if((fd = open("z", 0)) < 0){
        printf("cannot open enc config file\n");
        exit();
    }
    read(fd, &comp_param, sizeof(comp_param));
    close(fd);

    // prep module
    struct module mod[3];

    mod[0].func = &restoresz;
    mod[0].hookID = RES;
    char name0[6] = "rstsz";
    strcpy(mod[0].name, name0);

    mod[1].func = &enc;
    mod[1].hookID = ENC;
    char name1[6] = "flzip";
    strcpy(mod[1].name, name1);

    mod[2].func = &dec;
    mod[2].hookID = DEC;
    char name2[6] = "flunz";
    strcpy(mod[2].name, name2);

    int err = addmod(3, mod);
    if (err == -1)
        printf("module already exists\n");
    if (err == -2) 
        printf("hook position filled up\n");
    if (err == -3)
        printf("wrong hook identifier\n");
	exit();
}
