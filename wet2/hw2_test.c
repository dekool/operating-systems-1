#include <sys/types.h>
#include "hw2_syscalls.h"

/*
 * Scheduling policies
 */
#define SCHED_OTHER		0
#define SCHED_FIFO		1
#define SCHED_RR		2
#define SCHED_SHORT     5

int main(){

    int test_res = 0;
    int i;
    pid_t pid, pid2;
    pid = getpid();
    test_res = is_short(pid);
    if (test_res != 0) {
        printf("error! process is SHORT\n");
    }
    pid2 = fork(); // must fork before converting to SHORT

    if (pid2 == 0) { // child
        pid = getpid();
        test_res = is_short(pid);
        if (test_res != 0) {
            printf("error! child process is SHORT\n");
        }
        int k = 1;
        // just a long loop so the process will not finish running before the parent become SHORT
        for(i = 0 ; i < 10000; i++) {
            k = k*2;
        }
        printf("I am child, and this should be printed after parent finished\n");
    } else { // parent
        struct sched_param param;
        param.requested_time = 300;
        param.sched_priority = 10;
        param.sched_short_prio = 100;
        test_res = sched_setscheduler(pid, SCHED_SHORT, &param);
        if (test_res != 0) {
            printf("failed convert to SHORT process. errno: %d\n", errno);
        }
        int k = 1;
        // just to make it even (and even bigger loop), so we know that it finished first because of the priority
        for(i = 0 ; i < 99999; i++) {
            k = k*2;
        }

        test_res = is_short(pid);
        if (test_res != 1) {
            printf("error! process is not SHORT after setschedule\n");
        }

        printf("I am parent, and I finished test\n");
    }

    return 0;
}
