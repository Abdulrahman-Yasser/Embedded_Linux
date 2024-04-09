#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	pid_t child_pid;
	child_pid = fork();

	if(child_pid < 0){
		fprintf(stderr, "Fork failed\n");
		return 1;
	}else if(child_pid == 0){
		printf("Child process: PID=%d, parent PID=%d\n", getpid(), getppid());
		while(1);
	}else{
		printf("parent process: PPID = %d, PID=%d, Child PID=%d\n", getppid(), getpid(), child_pid);
		while(1);
	}
	return 0;
}
