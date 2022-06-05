#include "kernel/types.h"
#include "kernel/module.h"
#include "user.h"

void f(void *x) {
    printf("BUNAR");
}

void g(void *x) {
    printf("BETON");
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
    printf("%x", (uint*)&f);
    addmod(2, a);
	exit();
}
