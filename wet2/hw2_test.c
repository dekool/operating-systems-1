#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <wait.h>
#include <sys/types.h>
#include "hw2_syscalls.h"
#include <stdlib.h>

#define assertTest(expression)\
    do {\
        if (!(expression)) {\
            printf("Assertion failed at %s:%d: "\
                   "in function \"%s\" "\
                   "with assertion \"%s\".\n",\
                   __FILE__, __LINE__, __func__, (#expression));\
        }\
    } while (0)


/*
 * Scheduling policies
 */
#define SCHED_OTHER		0
#define SCHED_FIFO		1
#define SCHED_RR		2
#define SCHED_SHORT     5

static void is_short_fail_pidNegative() {
    assertTest(is_short(-2) == -1);
    assertTest(errno == ESRCH);
}

static void short_remaining_time_fail_pidNegative() {
    assertTest(short_remaining_time(-2) == -1);
    assertTest(errno == ESRCH);
}

static void short_remaining_time_fail_noSHORT() {
    pid_t pid = getpid();
    assertTest(short_remaining_time(pid) == -1);
    assertTest(errno == EINVAL);
}

static void short_place_in_queue_fail_pidNegative() {
    assertTest(short_place_in_queue(-2) == -1);
    assertTest(errno == ESRCH);
}

static void short_place_in_queue_fail_noSHORT() {
    pid_t pid = getpid();
    assertTest(short_place_in_queue(pid) == -1);
    assertTest(errno == EINVAL);
}

static void setscheduler_fail_invalid_parameters() {
    pid_t pid = getpid();
    int test_res = 0;
    struct sched_param param;
    param.requested_time = 0;
    param.sched_priority = 10;
    param.sched_short_prio = 100;

    assertTest(sched_setscheduler(pid, SCHED_SHORT, &param) == -1);
    assertTest(errno == EINVAL);
    param.requested_time = 3001;
    assertTest(sched_setscheduler(pid, SCHED_SHORT, &param) == -1);
    assertTest(errno == EINVAL);
    param.requested_time = 100;
    param.sched_short_prio = -1;
    assertTest(sched_setscheduler(pid, SCHED_SHORT, &param) == -1);
    assertTest(errno == EINVAL);
    param.sched_short_prio = 140;
    assertTest(sched_setscheduler(pid, SCHED_SHORT, &param) == -1);
    assertTest(errno == EINVAL);
    param.sched_short_prio = 100;
    // convert to real time process
    assertTest(sched_setscheduler(pid, SCHED_FIFO, &param) == 0);
    // only OTHER can be converted to SHORT
    assertTest(sched_setscheduler(pid, SCHED_SHORT, &param) == -1);
    assertTest(errno == EPERM);
}

static void setscheduler_fail_already_SHORT() {
    pid_t pid = getpid();
    struct sched_param param;
    param.requested_time = 100;
    param.sched_priority = 10;
    param.sched_short_prio = 100;

    assertTest(sched_setscheduler(pid, SCHED_SHORT, &param) == 0); // first time success
    assertTest(sched_setscheduler(pid, SCHED_SHORT, &param) == -1); // second time fail
    assertTest(errno == EPERM);
}

static void short_process_runs_before_other() {
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
}

int main() {
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        is_short_fail_pidNegative();
        return 0;
    }
    pid = fork();
    if (pid == 0) {
        short_remaining_time_fail_pidNegative();
        return 0;
    }
    pid = fork();
    if (pid == 0) {
        short_remaining_time_fail_noSHORT();
        return 0;
    }
    pid = fork();
    if (pid == 0) {
        short_place_in_queue_fail_pidNegative();
        return 0;
    }
    pid = fork();
    if (pid == 0) {
        short_place_in_queue_fail_noSHORT();
        return 0;
    }
    pid = fork();
    if (pid == 0) {
        setscheduler_fail_invalid_parameters();
        return 0;
    }
    pid = fork();
    if (pid == 0) {
        setscheduler_fail_already_SHORT();
        return 0;
    }

    pid = fork();
    if (pid == 0) {
        short_process_runs_before_other();
        return 0;
    }
    return 0;
}
