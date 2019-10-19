#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void handler(int sig) {
  fprintf(stdout,"I don't want to die!\n");
  return;
}

int main(int argc, char **argv)
{
	if (signal(SIGINT, handler) == SIG_ERR) {
		perror("signal()");
	}
  while (1) ; // infinite loop
}

