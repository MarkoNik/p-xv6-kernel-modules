#include "kernel/types.h"
#include "kernel/module.h"
#include "user.h"

ushort color;

// set screen color
void func(void *x, uint offset) {
    struct keyin_params *params = (struct keyin_params*)x;
    int *in = params->c;
    if(*in == '\n') {
        ushort *crt = params->crt;
        for(int i = 0; i < 25; i++) {
            for(int j = 0; j < 80; j++) {
                crt[i*80 + j] &= 0x00ff;
                crt[i*80 + j] |= *(ushort*)globl(&color, offset);
            }
        }
    }
}

int
main(void)
{
    // read screen color
    int fd;
    if((fd = open("a", 0)) < 0){
        printf("cannot open color config file\n");
        exit();
    }
    char c;
    read(fd, &c, sizeof(c));
    close(fd);
    switch(c) {
        case 'r':
            color = 0x4f00;
            break;
        case 'g':
            color = 0x2e00;
            break;
        case 'b':
            color = 0x1c00;
            break;
        default:
            printf("color not supported\n");
    }
   

    // prep module
    struct module mod[1];
    mod[0].func = &func;
    mod[0].hookID = CONSOUT;
    char name[6] = "sccol";
    strcpy(mod[0].name, name);
    int err = addmod(1,mod);
    if (err == -1)
        printf("module already exists\n");
    if (err == -2) 
        printf("hook position filled up\n");
    if (err == -3)
        printf("wrong hook identifier\n");
	exit();
}
