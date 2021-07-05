#include "kernel/types.h"
#include "user/user.h"

/*
 * Write a concurrent version of prime sieve using pipes. This idea is due to
 * Doug McIlroy, inventor of Unix pipes. The picture halfway down this page
 * (https://swtch.com/~rsc/thread/) and the surrounding text explain how to do
 * it. Your solution should be in the file user/primes.c.
 */

void child_pipe(int[]);

int
main(int argc, char *argv[])
{
  int p2c_fds[2];

  if (pipe(p2c_fds) < 0) {
    fprintf(2, "primes: pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) { // error
    fprintf(2, "primes: fork failed\n");
    exit(1);
  }
  else if (pid == 0) { // child
    close(p2c_fds[1]);
    child_pipe(p2c_fds);
    close(p2c_fds[0]);
  }
  else { // parent
    close(p2c_fds[0]);

    for (int i = 2; i <= 35; i++) {
      write(p2c_fds[1], &i, sizeof(i));
    }
    close(p2c_fds[1]);
    wait(0);
  }
  exit(0);
}

void
child_pipe(int p2c_fds[])
{
  int divisor;
  int num;

  int bytes_read = read(p2c_fds[0], &divisor, sizeof(num));
  if (bytes_read == 0) {
    return;
  }

  printf("prime: %d\n", divisor);

  int c2gc_fds[2];
  if (pipe(c2gc_fds) < 0) {
    fprintf(2, "primes: child_to_grandchild pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) { // error
    fprintf(2, "primes: child_pipe fork failed\n");
    exit(1);
  }
  else if (pid == 0) { // grand child
    close(c2gc_fds[1]);
    child_pipe(c2gc_fds);
    close(c2gc_fds[0]);
  }
  else { // child
    close(c2gc_fds[0]);

    while (read(p2c_fds[0], &num, sizeof(num)) > 0) {
      if (num % divisor != 0) {
        write(c2gc_fds[1], &num, sizeof(num));
      }
    }
    close(c2gc_fds[1]);
    wait(0);
  }
}
