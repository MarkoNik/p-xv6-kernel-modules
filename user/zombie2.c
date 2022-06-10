// Create a zombie process that
// must be reparented at exit.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int
main(void)
{
    nice(3, 101);
	int f = fork();
    sleep(1);
    if (f > 0) {
        printf("child finished\n");
        exit();
    }
    printf("parent finished\n");
    exit();
}
