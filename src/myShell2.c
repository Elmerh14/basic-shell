#include "apue.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

static void sig_int(int);

int main(void) {
  char buf[MAXLINE];
  pid_t pid;
  int status;

  if (signal(SIGINT, sig_int) == SIG_ERR)
    err_sys("signal error");

  printf("%% ");
  while (fgets(buf, MAXLINE, stdin) != NULL) {
    if (buf[strlen(buf) - 1] == '\n')
      buf[strlen(buf) - 1] = 0;

    /* get the exact size of array to allocate */
    int count = 0;
    bool in_word = false;

    for (char *p = buf; *p != '\0'; p++) {
      if (strchr(" \t\n", *p) != NULL) {
        in_word = false;
      } else if (!in_word) {
        in_word = true;
        count++;
      }
    }
    /* -------------------------------------*/

    /* iterate through the buffer and fill in our array */
    char **cmd_strs = malloc((count + 1) * sizeof(char *));
    char *token = strtok(buf, " \t\n");
    int i = 0;

    while (token != NULL) {
      cmd_strs[i] = token;
      i++;
      token = strtok(NULL, " \t\n");
    }
    cmd_strs[i] = NULL;
    /* -------------------------------------------*/

    if ((pid = fork()) < 0) { /* chile proccess */
      err_sys("fork error");
    } else if (pid == 0) {
      execvp(cmd_strs[0], cmd_strs);
      err_ret("couldn't execute: %s", buf);
      free(cmd_strs);
      exit(127);
    }

    if ((pid = waitpid(pid, &status, 0)) < 0)
      err_sys("waitpid error");
    free(cmd_strs);
    printf("%% ");
  }
  exit(0);
}

void sig_int(int signo) { printf("interrupt\n%% "); }
