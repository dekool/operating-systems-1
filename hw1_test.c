#include "hw1_syscalls.h"
#include <sys/types.h>
#include <stdlib.h>

int main(){
	
	pid_t pid = getpid();
	scr r = {20,2};
	sc_restrict(pid,0,&r,1);
	getpid(); //should fail
	set_proc_restriction(pid,1);
	getpid(); //should fail
	set_proc_restriction(pid,0);
	getpid(); //should fail
	set_proc_restriction(pid,2);
	getpid(); //should succeed
	fai* mem = (fai*)malloc(sizeof(fai)*3);
	if(mem == NULL){
		return -1;
	}
	
	get_process_log(pid, 3, mem);
	int i;
	for(i = 0 ; i < 3; i++){
		printf("restrict info: \n syscall_num = %d\n restriction threshold = %d\n proc level = %d\n time = %d\n", mem[i].syscall_num,
				mem[i].syscall_restriction_threshold, mem[i].proc_restriction_level, mem[i].time);
	
	}
	
	free(mem);
	return 0;
}
