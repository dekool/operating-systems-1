#include <linux/kernel.h>
#include <linux/sched.h>

int restrict(int syscall_number) {
	if(current->restrictions_counter > 0){
		int syscall_threshold = get_syscall_threshold(syscall_number);
		if(syscall_threshold >= 0 && current->restriction_level < syscall_threshold){
			//saving in forbidden log 
			int log_index = (current->log_counter)%RESTRICT_LOG_SIZE;
			current->forbidden_log[log_index].syscall_num = syscall_number;
			current->forbidden_log[log_index].syscall_restriction_threshold = syscall_threshold;
			current->forbidden_log[log_index].proc_restriction_level = current->restriction_level;
			current->forbidden_log[log_index].time = (int)jiffies;
			current->log_counter++;
			/* next check is to keep the counter between 0 to 200, if we are above 100 we know there were
				at least 100 restrictions and we can return the log for any size requested*/
			if(current->log_counter > (2*RESTRICT_LOG_SIZE)){
				current->log_counter -= RESTRICT_LOG_SIZE;
			}
			
			return -ENOSYS;
		}
	}
	return 0;
}

static int get_syscall_threshold(int syscall_number){
		scr* restrictions_list = current->restrictions_list;
		int size = current->restrictions_counter;
		int i;
		//finds the syscall in the restrictions_list
		for(i = 0; i < size; i++){
			if(restrictions_list[i].syscall_num == syscall_number){
				return restrictions_list[i].restriction_threshold;
			}
		}
		return -1; //in case the syscall is not restricted
}

