#include "kernel/types.h"
#include "kernel/module.h"
#include "user.h"

struct module mod[1];
char *name = "sccol";
ushort color;

// set screen color
void func(void *x, uint offset) {
    struct keyin_params *params = (struct keyin_params*)x;
    int in = params->c;
    if(in == '\n') {
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
    mod[0].func = &func;
    mod[0].hookID = KEYIN;
    strcpy(mod[0].name, name);
    addmod(1, mod);
	exit();
}
