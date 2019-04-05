#include "hw1_syscalls.h"
#include <sys/types.h>
#include <stdlib.h>

int main(){
	/*
	pid_t pid = getpid();
	
	sc_restrict(pid,0,&r,1);
	getpid(); //should fail
	set_proc_restriction(pid,2);
	getpid(); //should succeed
	*/
	scr r = {20,2};
	printf("in main\n");
	sc_restrict(1,0,&r,0);
	set_proc_restriction(2,1000);
	get_process_log(3,1,NULL);
	printf("out main\n");
	return 0;
}
