#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/errno.h>

int sys_sc_restrict (pid_t pid ,int proc_restriction_level, scr* restrictions_list, int list_size) {
    /* do some input checks */
    if (pid < 0) {
        return ESRCH;
    }
    task_struct* task = find_task_by_pid(pid);
    if (task == 0) {
        return ESRCH;
    }
    if (proc_restriction_level < 0 || proc_restriction_level > 2 || list_size < 0) {
        return EINVAL;
    }
    task.restriction_level = proc_restriction_level;
    if (task.restriction_list != NULL) {
        kfree(task.restrictions_list);
    }
    task.restrictions_list = (scr*)kmalloc(sizeof(scr)*list_size, GFP_KERNEL); /* advised to use GFP_KERNEL flag for kernel memory use */
    if (task.restrictions_list == NULL) {
        return ENOMEM;
    }
    int succ_copying = copy_from_user(&task.restriction_list, &restriction_list, ksize(restriction_list));
    if (succ_copying != 0) {
        return ENOMEM;
    }
    /* also allocate memory for the log */
    task.forbidden_log = (fai*)kmalloc(sizeof(fai)*100, GFP_KERNEL);
    if (task.forbidden_log == NULL) {
        return ENOMEM;
    }
    return 0;
}

int sys_set_proc_restriction (pid_t pid ,int proc_restriction_level) {
    /* do some input checks */
    if (pid < 0) {
        return ESRCH;
    }
    task_struct* task = find_task_by_pid(pid);
    if (task == 0) {
        return ESRCH;
    }
    if (proc_restriction_level < 0 || proc_restriction_level > 2 || list_size < 0) {
        return EINVAL;
    }
    task.restriction_level = proc_restriction_level;

    return 0;
}

int sys_get_process_log(pid_t pid, int size, fai* user_mem) {
    /* do some input checks */
    if (pid < 0) {
        return ESRCH;
    }
    task_struct* task = find_task_by_pid(pid);
    if (task == 0) {
        return ESRCH;
    }
    if (size > task.log_counter || size < 0) {
        return EINVAL;
    }
    if (task.log_counter < 100) {
        /* to calculate the pointer to start copying from do: #start_of_array + (#counter - #size_to_copy) */
        int succ_copying = copy_to_user(&user_mem, &task.forbidden_log + (task.counter - size),\
        ksize(task.forbidden_log + (task.counter - size)));
        if (succ_copying != 0) {
            return ENOMEM;
        }
    }
    return 0;
    /* enter here logic in case we passed 100 logs */

}
