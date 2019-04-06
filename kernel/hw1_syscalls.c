#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>



int sys_sc_restrict (pid_t pid ,int proc_restriction_level, scr* restrictions_list, int list_size) {
	/* do some input checks */
    if (pid < 0) {
        return -ESRCH;
    }
    task_t* task = find_task_by_pid(pid);
    if (task == 0) {
        return -ESRCH;
    }
    if (proc_restriction_level < 0 || proc_restriction_level > 2 || list_size < 0) {
        return -EINVAL;
    }
    /* accroding to piazza if the syscall fail it should have no effect on the data
		so we first check if it can fail and only after it we change the data */
    scr* new_restriction_list = (scr*)kmalloc(sizeof(scr)*list_size, GFP_KERNEL); /* advised to use GFP_KERNEL flag for kernel memory use */
    if (new_restriction_list == NULL) {
        return -ENOMEM;
    }
	int succ_copying = copy_from_user(new_restriction_list, restrictions_list, sizeof(scr)*list_size);
    if (succ_copying != 0) {
		kfree(new_restriction_list);
		return -ENOMEM;
    }
	
	// changes to our data starts here
	if (task->restrictions_list != NULL) {
        kfree(task->restrictions_list);
    }
	task->restriction_level = proc_restriction_level;
	task->restrictions_list = new_restriction_list;
	task->restrictions_counter = list_size;
    
    return 0;
}

int sys_set_proc_restriction (pid_t pid ,int proc_restriction_level) {
	/* do some input checks */
    if (pid < 0) {
        return -ESRCH;
    }
    task_t* task = find_task_by_pid(pid);
    if (task == 0) {
        return -ESRCH;
    }
    if (proc_restriction_level < 0 || proc_restriction_level > 2 ) {
        return -EINVAL;
    }
    task->restriction_level = proc_restriction_level;

    return 0;
}

int sys_get_process_log(pid_t pid, int size, fai* user_mem) {
    /* do some input checks */
    if (pid < 0) {
        return -ESRCH;
    }
    task_t* task = find_task_by_pid(pid);
    if (task == 0) {
        return -ESRCH;
    }
	//TODO need to check what to do in case log_counter == 0 , should we return failure ?, in the documnets it says no.
    if (size > task->log_counter || size < 0 || size > 100) { 
        return -EINVAL;
    }
	// our log in oraganized from oldest to newest and we need to return from newest to oldest
    
    fai* return_log = (fai*)kmalloc(sizeof(fai)*size, GFP_KERNEL); //allocating mem for organizing the output
	int i;
	for(i = 0; i < size; i++){
		/*our data should be in indexes (log_counter - size)mod100 to (log_counter - 1) mod100
			we copy the data in opposite direction */
		return_log[i] = task->forbidden_log[(task->log_counter - i - 1) % 100]; 
	}
		
    int succ_copying = copy_to_user(user_mem, return_log , sizeof(scr)*size);
	kfree(return_log);
    if (succ_copying != 0) {
        return -ENOMEM;
    }
    return 0;

}
