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
	argptr(1, &modarr, n * sizeof(struct module));

	// fill in functions in hooks, with recalculated virtual address
	for(int i = 0; i < n; i++) {
		for (int j = 0; j < MAXMOD; j++) {
			if (hook[modarr[i].hookID][j].func == 0) {
				// offset function to proper kvm adr
				hook[modarr[i].hookID][j].func = (void (*)(void*))((uint)modarr[i].func + curproc->moduletop); 
				hook[modarr[i].hookID][j].memstart = curproc->moduletop;
				hook[modarr[i].hookID][j].size = curproc->sz;
				hook[modarr[i].hookID][j].pid = curproc->pid;
				memmove(hook[modarr[i].hookID][j].name, modarr[i].name, 6); // copy module name
				break;
			} else if (j == MAXMOD - 1) {
				// hook filled up
				return -1;
			}
		}
	}

	// do the mapping in all processes
	mapmodule();
	
	curproc->moduletop += PGROUNDUP(curproc->sz);
	myproc()->state = RESIDENT;
	return 0;
}

int sys_rmmod(void)
{
	return 0;
}

int sys_nice(void)
{
	return 0;
}