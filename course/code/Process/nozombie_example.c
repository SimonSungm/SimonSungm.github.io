#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void handler_sigchld(int sig) 
{
  pid_t pid;
  int status;

  fprintf(stdout,"In handler_sigchld()\n");
  pid = wait(&status);
  if (pid == -1) {
    perror("wait()");
  } else {
    fprintf(stdout,"Killing zombie %d (which exited with code %d)\n",
          pid, WEXITSTATUS(status));
  }
  return;
}


int main(int argc, char **argv)
{
  pid_t pid;

  pid = fork();

  signal(SIGCHLD, handler_sigchld);

  if (pid < 0) {
    fprintf(stderr,"Error: can't fork a process\n");
    perror("fork()");
    exit(1);
  } else if (pid) { // I am the parent
    // infinite loop
    while (1);
  } else {  // I am the child and I exit after sleeping 2 seconds
    sleep(2);
    exit(42);
  }
}

