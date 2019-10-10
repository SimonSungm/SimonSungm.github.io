#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char **argv)
{
  pid_t pid;

  pid = fork();

  if (pid < 0) {
    fprintf(stderr,"Error: can't fork a process\n");
    perror("fork()");
    exit(1);
  } else if (pid) { // I am the parent and I die right away
    printf("parent %d exits after forked child %d\n", getpid(), pid);
    exit(0); 
  } else {  // I am the child and I exit after a second
    sleep(1);
    fprintf(stderr,"I am the child %d and my parent's pid is: %d\n", getpid(), getppid());
    exit(0);
  }
}

