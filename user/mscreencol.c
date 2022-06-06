#include "kernel/types.h"
#include "kernel/module.h"
#include "user.h"

void mod(void *x) {
    (*((int*)x))++;
    
 /*    uint *crt = (uint*)x;
    crt[0] = 0; */
   
}

struct module a[1];
struct module m;
char *ime1 = "sccol";

int
main(void)
{
    m.func = &mod;
    m.hookID = KEYIN;
    strcpy(m.name, ime1);
    a[0] = m;
    addmod(1, a);
	exit();
}
