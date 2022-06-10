
int main(int argc, char *argv[])
{
    if(argc != 3) {
        printf("usage: nice pid prio");
    }
    int err = nice(atoi(argv[1]), atoi(argv[2]));
    if(err < 0) printf("nice: prio<0");
    exit();
}