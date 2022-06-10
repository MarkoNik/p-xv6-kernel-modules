#include "kernel/types.h"
#include "kernel/module.h"
#include "user.h"

char cmhist[128*100];
uint rhist[100], whist[100], ehist[100];
int pos = 99;

void func(void *x, uint offset) {
    char *gcmhist = (char*)((char*)cmhist + offset);
    uint *grhist = (uint*)((char*)rhist + offset);
    uint *gwhist = (uint*)((char*)whist + offset);
    uint *gehist = (uint*)((char*)ehist + offset);
    struct keyin_params *params = (struct keyin_params*)x;
    int delcount;

    switch(params->key) {
        // key up
        case 0xE2:
            if(*(int*)(globl(&pos, offset)) == 0) break;
            if(grhist[*(int*)(globl(&pos, offset))] <= gehist[*(int*)(globl(&pos, offset))]) {
                delcount = gehist[*(int*)(globl(&pos, offset))] - grhist[*(int*)(globl(&pos, offset))];
            } else {
                delcount = 128 - grhist[*(int*)(globl(&pos, offset))] + gehist[*(int*)(globl(&pos, offset))];
            }

            // backspace
            for(int i = 0; i < delcount; i++) {
                params->consputc(0x100);
            } 

            *(int*)(globl(&pos, offset)) -= 1;
            for(int i = 0; i < 128; i++) {
                params->buf[i] = gcmhist[128 * (*(int*)(globl(&pos, offset))) + i];
            }
            *params->r = grhist[*(int*)(globl(&pos, offset))];
            *params->w = gwhist[*(int*)(globl(&pos, offset))];
            *params->e = gehist[*(int*)(globl(&pos, offset))];

            for(int i = *params->r; i !=  *params->e; i = (i + 1) % 128) {
                params->consputc(params->buf[i]);
            }
            break;

        //key down
        case 0xE3:
            if(*(int*)(globl(&pos, offset)) == 99) break;

            if(grhist[*(int*)(globl(&pos, offset))] <= gehist[*(int*)(globl(&pos, offset))]) {
                delcount = gehist[*(int*)(globl(&pos, offset))] - grhist[*(int*)(globl(&pos, offset))];
            } else {
                delcount = 128 - grhist[*(int*)(globl(&pos, offset))] + gehist[*(int*)(globl(&pos, offset))];
            }
            
            // backspace
            for(int i = 0; i < delcount; i++) {
                params->consputc(0x100);
            } 

            *(int*)(globl(&pos, offset)) += 1;
            for(int i = 0; i < 128; i++) {
                params->buf[i] = gcmhist[128 * (*(int*)(globl(&pos, offset))) + i];
            }
            *params->r = grhist[*(int*)(globl(&pos, offset))];
            *params->w = gwhist[*(int*)(globl(&pos, offset))];
            *params->e = gehist[*(int*)(globl(&pos, offset))];

            for(int i = *params->r; i !=  *params->e; i = (i + 1) % 128) {
                params->consputc(params->buf[i]);
            }
            break;

        case '\n':
            break;

        default:
            for(int i = 0; i < 128; i++) {
                gcmhist[128 * (*(int*)(globl(&pos, offset))) + i] = params->buf[i];
            }
            grhist[*(int*)(globl(&pos, offset))] = *params->r;
            gwhist[*(int*)(globl(&pos, offset))] = *params->w;
            gehist[*(int*)(globl(&pos, offset))] = *params->e;
            break;
    }
}

void enter(void *x, uint offset) {
    char *gcmhist = (char*)((char*)cmhist + offset);
    uint *grhist = (uint*)((char*)rhist + offset);
    uint *gwhist = (uint*)((char*)whist + offset);
    uint *gehist = (uint*)((char*)ehist + offset);

    for(int i = 0; i < 128*99; i++) {
        gcmhist[i] = gcmhist[i+128];
    }
    for(int i = 0; i < 99; i++) {
        grhist[i] = grhist[i + 1];
        gwhist[i] = gwhist[i + 1];
        gehist[i] = gehist[i + 1];
    }
}

int
main(void)
{
    struct module mod[2];
    mod[0].func = &func;
    mod[0].hookID = KEYIN;
    char name[6] = "cmhis";
    strcpy(mod[0].name, name);

    mod[1].func = &enter;
    mod[1].hookID = ENTER;
    char name2[6] = "enter";
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
