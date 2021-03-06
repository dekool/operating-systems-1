#ifndef _HW1_SYSCALLS_H
#define _HW1_SYSCALLS_H

#include <errno.h>
#include <sys/types.h>

typedef struct sys_call_restriction {
    int syscall_num;
    int restriction_threshold;
} scr;

typedef struct forbidden_activity_info {
    int syscall_num;
    int syscall_restriction_threshold;
    int proc_restriction_level;
    int time;
} fai;

int sc_restrict (pid_t pid ,int proc_restriction_level, scr* restrictions_list, int list_size) {
    int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (243), "b" (pid), "c" (proc_restriction_level), "d" (restrictions_list), "S" (list_size)
		: "memory"
		);
	if((__res) < 0){
		errno = (-__res);
		return -1;
	}
	return __res;
}

int set_proc_restriction (pid_t pid ,int proc_restriction_level) {
    int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (244), "b" (pid), "c" (proc_restriction_level)
		: "memory"
		);
	if((__res) < 0){
		errno = (-__res);
		return -1;
	}
	return __res;
}

int get_process_log(pid_t pid, int size, fai* user_mem) {
    int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (245), "b" (pid), "c" (size), "d" (user_mem)
		: "memory"
		);
	if((__res) < 0){
		errno = (-__res);
		return -1;
	}
	return __res;

}

#endif
