#include "kernel/types.h"
#include "kernel/module.h"
#include "user.h"

ushort crthist[25*80*10];
uint curlocation = 25*80*9 + 80;
int cursor = 25*80*10-80*2+1;

void func(void *x, uint offset) {
    ushort *gcrthist = (ushort*)((char*)crthist + offset);
    struct consout_params *params = (struct consout_params*)x;
    int *in = params->c;
    ushort* crt = params->crt;
    switch(*in) {
        case '\n':
            gcrthist[(*(int*)globl(&cursor, offset))] = '\n';
            *(int*)globl(&cursor, offset) = 25*80*10-80*2+1;
            for(int i = 0; i < 25*80*10 - 80*2; i++) {
                gcrthist[i] = gcrthist[i + 80];
            }
            *((uint*)globl(&curlocation, offset)) = 25*80*9 + 80;
            for(uint i = 0, j = *((uint*)globl(&curlocation, offset)); i < 25*80 - 80 * 3; i++, j++) {
                crt[i] = gcrthist[j];
            }
            break;

        // pgupS
        case 230:
            if (*((uint*)globl(&curlocation, offset)) >= 80) {
                *((uint*)globl(&curlocation, offset)) -= 80;
                for(uint i = 0, j = *((uint*)globl(&curlocation, offset)); i < 25*80 - 80 * 3; i++, j++) {
                    crt[i] = gcrthist[j];
                }
            }
            break;

        // pgdown
        case 231:
            if (*((uint*)globl(&curlocation, offset)) <= 25*80*9) {
                *((uint*)globl(&curlocation, offset)) += 80;
                for(uint i = 0, j = *((uint*)globl(&curlocation, offset)); i < 25*80 - 80 * 3; i++, j++) {
                    crt[i] = gcrthist[j];
                }
            }
            break;
        
        default:
            gcrthist[*((int*)globl(&cursor, offset))] = (ushort)((*in & 0xff) | 0x0700);
            *(int*)globl(&cursor, offset) += 1;
            *((uint*)globl(&curlocation, offset)) = 25*80*9+ 80;
            for(uint i = 0, j = *((uint*)globl(&curlocation, offset)); i < 25*80 - 80 * 3; i++, j++) {
                crt[i] = gcrthist[j];
            }
            break;
    }
}

int
main(void)
{
    for(int i = 0; i < 25*80*10; i++)
        crthist[i] = 0x0000;
    struct module mod[1];
    mod[0].func = &func;
    mod[0].hookID = CONSOUT;
    char name[6] = "schis";
    strcpy(mod[0].name, name);
    int err = addmod(1, mod);
    if (err == -1)
        printf("module already exists\n");
    if (err == -2) 
        printf("hook position filled up\n");
    if (err == -3)
        printf("wrong hook identifier\n");
	exit();
}
