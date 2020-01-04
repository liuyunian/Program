#include <sys/signalfd.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  sigset_t mask;
  int sfd;
  struct signalfd_siginfo fdsi;
  ssize_t s;

  sigemptyset(&mask);
  sigaddset(&mask, SIGHUP);
  sigaddset(&mask, SIGQUIT);

  /* Block signals so that they aren't handled
      according to their default dispositions */

  if(sigprocmask(SIG_SETMASK, &mask, NULL) == -1){
    perror("sigprocmask");
  }

  sfd = signalfd(-1, &mask, 0);
  if(sfd == -1){
    perror("signalfd");
  }

  for(;;){
    s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
    if(s != sizeof(struct signalfd_siginfo)){
      perror("read");
    }

    if(fdsi.ssi_signo == SIGHUP) {
      printf("Got SIGHUP\n");
    } else if (fdsi.ssi_signo == SIGQUIT) {
      printf("Got SIGQUIT\n");
      exit(EXIT_SUCCESS);
    } else {
      printf("Read unexpected signal\n");
    }
  }

  return 0;
}