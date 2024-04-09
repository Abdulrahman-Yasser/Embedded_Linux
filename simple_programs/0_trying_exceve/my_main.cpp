#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
	char *command = "/bin/ls";
	char *args[] = {".", "-l", NULL};
	char *envp[] = {NULL};

	pid_t child_pid;

	child_pid = fork();

	if(child_pid == -1){
		perror( "Fork failed");
		exit(EXIT_FAILURE);
	}else if(child_pid == 0){
		execve(command, args, envp);
		perror("Execution failed");
		exit(EXIT_FAILURE);
	}else{
		int status;
		waitpid(child_pid, &status, 0);
		printf("the status is %d\n", status);
	}
	return 0;
}
