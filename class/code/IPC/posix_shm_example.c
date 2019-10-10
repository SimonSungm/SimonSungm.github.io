#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char **argv) 
{
	pid_t pid;

	/* Create a shared memory segment */
	int segment_id;
	segment_id = shmget(IPC_PRIVATE, 10*sizeof(char), SHM_R | SHM_W);
	if (segment_id == -1) {
		fprintf(stderr,"Can't create a new shared memory segment\n");
		perror("shmget()");
		exit(1);
	}
	fprintf(stdout,"Created a shared memory segment with id %d\n",
			segment_id);

	/* Create a child process */
	pid = fork();
	if (pid == -1) {
		fprintf(stderr,"Can't create a new process\n");
		perror("fork()");
		exit(1);
	}

	if (pid) { // parent

		fprintf(stdout,"Created a child\n");
		/* Attach the memory segment */
		char *shared_memory = (char *)shmat(segment_id, NULL, 0);
		if (!shared_memory) {
			fprintf(stderr,"Can't attach new memory segment\n");
			perror("shmat()");
			exit(1);
		}

		/* Writing to the shared memory segment */
		sprintf(shared_memory, "hello");
		fprintf(stdout,"Parent: wrote 'hello' in shared memory region\n");

		/* Waiting for my child to exit */
		fprintf(stdout,"Parent: waiting for child\n");
		if (waitpid(pid, NULL, 0) != pid) {
			fprintf(stderr,"Can't wait for child\n");
			perror("waitpid()");
			exit(1); 
		}

		/* Detaching the shared memory segment */
		if (shmdt(shared_memory) == -1) {
			fprintf(stderr,"Can't detach memory segment\n");
			perror("shmdt()");
			exit(1);
		}
		fprintf(stdout,"Parent: detached memory segment\n");

		/* Removing the shared memory segment */
		if (shmctl(segment_id, IPC_RMID, NULL)) {
			fprintf(stderr,"Can't remove memory segment\n");
			perror("shmctl()");
			exit(1);
		}
		fprintf(stdout,"Parent: removed memory segment\n");

	} else { // child

		/* Sleeping */
		sleep(2);

		/* Attaching the shared memory segment */
		char *shared_memory = (char *)shmat(segment_id, NULL, 0);
		if (!shared_memory) {
			fprintf(stderr,"Can't attach new memory segment\n");
			perror("shmat()");
			exit(1);
		}
		fprintf(stdout,"Child: attached memory segment\n");

		/* Reading the content of the shared memory */
		fprintf(stdout,"Child: read '%s' in shared memory region\n", 
				shared_memory);
		/* Detaching the shared memory segment */
		if (shmdt(shared_memory) == -1) {
			fprintf(stderr,"Can't detach memory segment\n");
			perror("shmdt()");
			exit(1);
		}
		fprintf(stdout,"Child: detached memory segment\n");

		fprintf(stdout,"Child: exiting\n");
		exit(0);
	}

	exit(0);
}
