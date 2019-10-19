#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char **argv)
{
	pid_t pid;

	pid = fork();

	if (pid < 0) {
		fprintf(stderr,"Error: can't fork a process\n");
		perror("fork()");
		exit(1);

	} else if (pid) { // I am the parent and I do whatever

		while(1) ; // infinite loop

	} else {  // I am the child
		pid_t pid;

		fprintf(stderr,"I am the child and my pid is: %d\n",getpid());

		// forking a grandchild
		pid = fork();

		if (pid < 0) {
			fprintf(stderr,"Error: can't fork a process\n");
			perror("fork()");
			exit(1);
		} 

		if (pid == 0) {  // grandchild
			fprintf(stderr,"I am the grandchild and my parent's pid is: %d\n",
					getppid());
			sleep(2);
			fprintf(stderr,"I am the grandchild and my parent's pid is: %d\n",
					getppid());
			exit(0);
		} else { // child
			sleep(1);
			exit(0);
		}
	}
}

