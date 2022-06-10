#include "kernel/types.h"
#include "kernel/module.h"
#include "kernel/param.h"
#include "user.h"

int nicemap[65];

// sort by priority
void func(void *x, uint offset) {
    struct proc_params *params = (struct proc_params*)x;
    int *gnicemap = (int*)((char*)nicemap + offset);
    int min = 12345;
    for(int i = 0; i < NPROC; i++) {
        if(gnicemap[i] < min && params->rnbl[i]) min = gnicemap[i];
    }
    int minidx = -1;
    for(int i = 0; i < NPROC; i++){
        if(gnicemap[i] == min && params->rnbl[i]) {
            minidx = i;
            break;
        }
    }

    if(minidx == -1) *params->curproc = (void*)((char*)params->procarr + NPROC * params->procstructsz);
    else *params->curproc = (void*)((char*)params->procarr + (minidx) * params->procstructsz);
}

void nicefunc(void *x, uint offset) {
    struct nice_params *params = (struct nice_params*)x;
    int *gnicemap = (int*)((char*)nicemap + offset);
    gnicemap[params->pid] = params->nice;
}

int
main(void)
{
    // all proc start with priority 100
    for(int i = 0; i < 65; i++)
        nicemap[i] = 100;

    // prep module
    struct module mod[2];
    mod[0].func = &func;
    mod[0].hookID = PROC;
    char name1[6] = "prsch";
    strcpy(mod[0].name, name1);

    mod[1].func = &nicefunc;
    mod[1].hookID = NICE;
    char name2[6] = "pnice";
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
