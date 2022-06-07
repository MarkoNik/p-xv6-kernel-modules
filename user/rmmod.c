#include "user.h"

int
main(int argc, char *argv[])
{
	if(argc != 2){
		fprintf(2, "Usage: rmmod module...\n");
		exit();
	}

	if(rmmod(argv[1]) < 0){
		fprintf(2, "rmmod: %s failed to remove module\n", argv[1]);
        exit();
	}

    fprintf(2, "module removed successfully\n");
	exit();
}