#include "kernel/types.h"
#include "user/user.h"

/*
 * Write a program that uses UNIX system calls to ''ping-pong'' a byte between
 * two processes over a pair of pipes, one for each direction. The parent should
 * send a byte to the child; the child should print "<pid>: received ping",
 * where <pid> is its process ID, write the byte on the pipe to the parent, and
 * exit; the parent should read the byte from the child, print "<pid>: received
 * pong", and exit. Your solution should be in the file user/pingpong.c.
 */
int
main(int argc, char *argv[])
{
  int p2c_fds[2];
  int c2p_fds[2];

  if (pipe(p2c_fds) < 0 || pipe(c2p_fds) < 0) {
    printf("pingpong: pipe failed\n");
    exit(1);
  }

  if (fork() == 0) { // child
    char c;
    if (read(p2c_fds[0], &c, 1) != 1) {
      printf("pingpong: parent_to_child pipe read failed\n");
      exit(1);
    }
    printf("%d: received ping\n", getpid());

    if (write(c2p_fds[1], &c, 1) != 1) {
      printf("pingpong: child_to_parent pipe write failed\n");
      exit(1);
    }

  } else { // parent

    if (write(p2c_fds[1], "x", 1) != 1) {
      printf("pingpong: parent_to_child pipe write failed\n");
      exit(1);
    }

    char c;
    if (read(c2p_fds[0], &c, 1) != 1) {
      printf("pingpong: child_to_parent pipe read failed\n");
      exit(1);
    }
    printf("%d: received pong\n", getpid());
  }
  close(p2c_fds[0]);
  close(p2c_fds[1]);
  close(c2p_fds[0]);
  close(c2p_fds[1]);
  exit(0);
}
