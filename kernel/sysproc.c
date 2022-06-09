#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "module.h"

int
sys_fork(void)
{
	return fork();
}

int
sys_exit(void)
{
	exit();
	return 0;  // not reached
}

int
sys_wait(void)
{
	return wait();
}

int
sys_kill(void)
{
	int pid;

	if(argint(0, &pid) < 0)
		return -1;
	return kill(pid);
}

int
sys_getpid(void)
{
	return myproc()->pid;
}

int
sys_sbrk(void)
{
	int addr;
	int n;

	if(argint(0, &n) < 0)
		return -1;
	addr = myproc()->sz;
	if(growproc(n) < 0)
		return -1;
	return addr;
}

int
sys_sleep(void)
{
	int n;
	uint ticks0;

	if(argint(0, &n) < 0)
		return -1;
	acquire(&tickslock);
	ticks0 = ticks;
	while(ticks - ticks0 < n){
		if(myproc()->killed){
			release(&tickslock);
			return -1;
		}
		sleep(&ticks, &tickslock);
	}
	release(&tickslock);
	return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
	uint xticks;

	acquire(&tickslock);
	xticks = ticks;
	release(&tickslock);
	return xticks;
}

int
sys_addmod(void)
{
	struct proc *curproc = myproc();
	int n;
	argint(0, &n);
	struct module *modarr;
	argptr(1, (char**)&modarr, n * sizeof(struct module));

	if(modarr->hookID < 0 || modarr->hookID > MAXHOOK) {
		return -3;
	}

	// check if name already exists
	for(int i = 0; i < MAXHOOK; i++) {
		for (int j = 0; j < MAXMOD; j++) {
			if(hook[i][j].pid && memcmp(hook[i][j].name, modarr->name, 5) == 0) {
				return -1;
			}
		}
	}

	// do the mapping
	mapmodule();
	// fill in functions in hooks, with recalculated virtual address
	for(int i = 0; i < n; i++) {
		for (int j = 0; j < MAXMOD; j++) {
			if (hook[modarr[i].hookID][j].pid == 0) {
				// offset function to proper kvm adr
				hook[modarr[i].hookID][j].func = (void (*)(void*))((uint)modarr[i].func + curproc->moduletop);
				hook[modarr[i].hookID][j].memstart = curproc->moduletop;
				hook[modarr[i].hookID][j].size = curproc->sz;
				hook[modarr[i].hookID][j].pid = curproc->pid;
				memmove(hook[modarr[i].hookID][j].name, modarr[i].name, 6); // copy module name
				break;
			} else if (j == MAXMOD - 1) {
				// hook filled up
				return -2;
			}
		}
	}
	
	acquire(getptablock());
	// update moduletop in all processes
	struct proc *p;
	struct proc *procarr = getprocarr();
	for(p = procarr; p < &procarr[NPROC]; p++){
		p->moduletop += PGROUNDUP(curproc->sz);
	}

	myproc()->state = RESIDENT;
	return 0;
}

int sys_rmmod(void)
{
	char *name;
	argptr(0, &name, 6 * sizeof(char));
	// delete module and find pid of deleted module
	int pid = -1;
	struct kmodule *kmod = 0;
	for(int i = 0; i < MAXHOOK; i++) {
		for (int j = 0; j < MAXMOD; j++) {
			if(hook[i][j].pid && memcmp(hook[i][j].name, name, 5) == 0) {
				// if module found, mark deleted (pid = 0)
				pid = hook[i][j].pid;
				hook[i][j].pid = 0;
				kmod = &hook[i][j];
				// TODO move back all modules by 1 in this hook
				break;
			}
		}
	}
	if(pid == -1) return -1;

	// if there are no modules with this pid anymore, free process with pid
	{
		// check if there are more modules with this pid
		int dead = 1;
		for(int i = 0; i < MAXHOOK; i++) {
			for (int j = 0; j < MAXMOD; j++) {
				if(hook[i][j].pid == pid) {
					dead = 0;
				}
			}
		}
		// there are no more
		if(dead) {
			// find process to free
			struct proc *p;
			struct proc *procarr = getprocarr();
			acquire(getptablock());
			for(p = procarr; p < &procarr[NPROC]; p++){
				if(p->pid == pid) {
					freemodule(kmod);
					p->state = RUNNABLE;
					break;
				}
			}
			for(p = procarr; p < &procarr[NPROC]; p++){
				p->moduletop -= kmod->size;
			}
			// remap function offset
			for(int i = 0; i < MAXHOOK; i++) {
				for (int j = 0; j < MAXMOD; j++) {
					if(hook[i][j].memstart > kmod->memstart) {
						hook[i][j].func -= kmod->size;
						hook[i][j].memstart -= kmod->size;
					}
				}
			}
			release(getptablock());
		}
	}

	return 0;
}

int sys_nice(void)
{
	return 0;
}