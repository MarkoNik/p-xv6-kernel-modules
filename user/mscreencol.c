#include "kernel/types.h"
#include "kernel/module.h"
#include "user.h"

struct module mod[1];
char *name = "sccol";
char c;
ushort color = 0x0700;

// set screen color
void func(void *x) {
    // switch(c) {
    //     case 'r':
    //         color = 0x4f00;
    //         break;
    //     case 'g':
    //         color = 0x2e00;
    //         break;
    //     case 'b':
    //         color = 0x1c00;
    //         break;
    //     default:
    //         printf("color not supported\n");
    // }
    struct keyin_params *params = (struct keyin_params*)x;
    int in = params->c;
    if(in == '\n') {
        ushort *crt = params->crt;
        for(int i = 0; i < 25; i++) {
            for(int j = 0; j < 80; j++) {
                crt[i*80 + j] &= 0x00ff;
                crt[i*80 + j] |= 0x2500;
            }
        }
    }
}

int
main(void)
{
    int fd;
    if((fd = open("a", 0)) < 0){
        printf("cannot open color config file\n");
        exit();
    }
    read(fd, &c, sizeof(c));
    close(fd);
   

    mod[0].func = &func;
    mod[0].hookID = KEYIN;
    strcpy(mod[0].name, name);
    addmod(1, mod);
	exit();
}
