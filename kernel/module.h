#define MAXMOD 10 // maximum number of modules per hook

enum hooktype {KEYIN, INBUF, ENC, DEC, PROC, MAXHOOK}; // add new before MAXHOOK

struct module {
    enum hooktype hookID;
    char name[6];
    void (*func)(void*);
};

struct kmodule {
    int pid;
    char name[6];
    void (*func)(void*);
    uint memstart; // first virtual address of kernel mapped module
    uint size; // size of module
};

// structures representing parameters for modules
struct keyin_params {
	ushort *crt;
	int c;
};

// hook position matrix
struct kmodule hook[MAXHOOK][MAXMOD];