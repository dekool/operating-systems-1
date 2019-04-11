#include "hw1_syscalls.h"
#include <sys/types.h>
#include <stdlib.h>

int main(){

    int test_res = 0;
	pid_t pid = getpid();
	scr r = {20,2};
	sc_restrict(pid,0,&r,1);
	test_res = getpid(); //should fail
	printf("result: %d, ", test_res);
	if (test_res == -1) {
	    printf("errno: %d\n", errno);
	}
	set_proc_restriction(pid,1);
    test_res = getpid(); //should fail
    printf("result: %d, ", test_res);
    if (test_res == -1) {
        printf("errno: %d\n", errno);
    }
	set_proc_restriction(pid,0);
    test_res = getpid(); //should fail
    printf("result: %d, ", test_res);
    if (test_res == -1) {
        printf("errno: %d\n", errno);
    }
	set_proc_restriction(pid,2);
    test_res = getpid(); //should succeed
    printf("result: %d, ", test_res);
    if (test_res == -1) {
        printf("errno: %d\n", errno);
    }
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

	// test fork
    set_proc_restriction(pid,0);
	int pid2 = fork();
	mem = (fai*)malloc(sizeof(fai)*3);
    if(mem == NULL){
        return -1;
    }
	if (pid2 == 0) {
        test_res = getpid(); //child should succeed
        printf("I am child, and the result is: %d\n", test_res);
	} else {
        test_res = getpid(); //parent should fail
        printf("I am parent, and the result is: %d\n", test_res);
	}
    free(mem);

	return 0;
}
