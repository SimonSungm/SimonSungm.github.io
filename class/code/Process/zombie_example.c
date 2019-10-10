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
  } else if (pid) { // I am the parent
    // infinite loop
    while (1);
  } else {  // I am the child and I exit right away
    exit(0);
  }
}

