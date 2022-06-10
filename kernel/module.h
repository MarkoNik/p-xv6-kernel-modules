#define MAXMOD 10 // maximum number of modules per hook

enum hooktype {KEYIN, INBUF, ENC, DEC, PROC, CONSOUT, RES, NICE, ENTER, MAXHOOK}; // add new before MAXHOOK

struct module {
    enum hooktype hookID;
    char name[6];
    void (*func)(void*, uint);
};

struct kmodule {
    int pid;
    char name[6];
    void (*func)(void*, uint);
    uint memstart; // first virtual address of kernel mapped module
    uint size; // size of module
};

// hook position matrix
struct kmodule hook[MAXHOOK][MAXMOD];


// structures representing parameters for modules
struct keyin_params {
    ushort *crt;
	char *buf;
	uint *r;  // Read index
	uint *w;  // Write index
	uint *e;  // Edit index
    void (*consputc)(int);
    int key;
};

struct consout_params {
    ushort *crt;
    int *c;
};

struct dat_params {
    uint inum;
    char *src;
    uint off;
    uint *n;
    void (*consputc)(int);
};

struct nice_params {
    int pid;
    int nice;
};

struct proc_params {
    void *procarr;
    void **curproc;
    int *rnbl;
    unsigned long procstructsz;
    void (*consputc)(int);
};