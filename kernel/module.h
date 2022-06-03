#define MAXMOD 10 // maximum number of modules per hook

enum hooktype {KEYIN, INBUF, ENC, DEC, PROC, MAXHOOK}; // add new before MAXHOOK

struct module {
    enum hooktype hookID;
    char name[5];
    void (*func)(void*);
};

struct kmodule {
    int pid;
    char name[5];
    void (*func)(void*);
};

kmodule hook[MAXHOOK][MAXMOD];