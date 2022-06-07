#include "kernel/types.h"
#include "kernel/module.h"
#include "user.h"

void f(void *x) {
    (*(int*)x)++;
}

void g(void *x) {
    (*(int*)x)--;
}

struct module a[2];
struct module m, n;
char *ime1 = "abcde";
char *ime2 = "aaaaa";

int
main(void)
{
    m.func = &f;
    n.func = &g;
    strcpy(m.name, ime1);
    strcpy(n.name, ime2);
    a[0] = m;
    a[1] = n;
    addmod(2, a);
    exit();
}

/*
	cprintf("ddd%x\n", hook[0][0].func);
	//cprintf("-%x\n",myproc()->pgdir[PDX(MODBASE)]);
	if (hook[0][0].func != 0)
		hook[0][0].func(0);
*/