// The contents of this file are up to you, but they should be related to
// running separate processes. It is recommended that you have functions
// for:
//   - performing a $PATH lookup
//   - determining if a command is a built-in or executable
//   - running a single command in a second process
//   - running a pair of commands that are connected with a pipe
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *path_lookup (char *cmd);
int is_builtin (char *cmd);
int run_builtin (char *cmd, char *argv[]);
int run_single (char *cmd, char *argv[]);
int run_piped (char *cmd1, char *cmd2, char *argv1[], char *argv2[]);

char *
path_lookup (char *cmd)
{
  char *path = getenv ("PATH");
  char *token = strtok (path, ":");

  while (token != NULL)
    {
      size_t full_len = strlen (token) + 1 + strlen (cmd) + 1;
      char *full = malloc (strlen (token) + strlen (cmd) + 2);

      if (full == NULL)
        {
          exit (EXIT_FAILURE);
        }

      snprintf (full, full_len, "%s/%s", token, cmd);

      if (access (full, X_OK) == 0)
        {
          return full;
        }

      free (full);
      token = strtok (NULL, ":");
    }
  return NULL;
}

int
is_builtin (char *cmd)
{
  if (strcmp (cmd, "cd") == 0 || strcmp (cmd, "echo") == 0)
    {
      return 1;
    }
  if (path_lookup (cmd) != NULL)
    {
      return 2;
    }
  return 0;
}

int
run_builtin (char *cmd, char *argv[])
{
  if (strcmp (cmd, "cd") == 0)
    {
      if (argv[1] != NULL)
        {
          if (chdir (argv[1]) != 0)
            {
              return 1;
            }
        }
      else
        {
          fprintf (stderr, "cd: missing arg\n");
          return 1;
        }
    }
  else if (strcmp (cmd, "echo") == 0)
    {
      for (int i = 1; argv[i] != NULL; i++)
        {
          if (i > 1)
            {
              printf (" ");
            }
          printf ("%s", argv[i]);
        }
      printf ("\n");
    }
  return 0;
}

int
run_single (char *cmd, char *argv[])
{
  pid_t pid = fork ();
  if (pid == 0)
    {
      if (is_builtin (cmd) == 1)
        {
          exit (run_builtin (cmd, argv));
        }
      else
        {
          if (cmd != NULL)
            {
              execv (cmd, argv);
              exit (EXIT_FAILURE);
            }
          else
            {
              fprintf (stderr, "Invalid Command");
              free (cmd);
              exit (EXIT_FAILURE);
            }
        }
    }
  else if (pid > 0)
    {
      int stat;

      while (waitpid (pid, &stat, 0) == -1)
        {
          if (errno != EINTR)
            {
              perror ("waitpid");
              break;
            }
        }
      if (WIFEXITED (stat))
        {
          return WEXITSTATUS (stat);
        }
      else
        {
          return -1;
        }
    }
  else
    {
      return -1;
    }
}

int
run_piped (char *cmd1, char *cmd2, char *argv1[], char *argv2[])
{
  int stat1, stat2;
  int pipe_fd[2];

  if (pipe (pipe_fd) == -1)
    {
      return 1;
    }
  pid_t child1 = fork ();

  if (child1 == 0)
    {
      close (pipe_fd[0]);
      dup2 (pipe_fd[1], STDOUT_FILENO);
      close (pipe_fd[1]);
      exit (run_single (cmd1, argv1));
    }
  pid_t child2 = fork ();

  if (child2 == 0)
    {
      close (pipe_fd[1]);
      dup2 (pipe_fd[0], STDIN_FILENO);
      close (pipe_fd[0]);
      exit (run_single (cmd2, argv2));
    }

  close (pipe_fd[0]);
  close (pipe_fd[1]);

  waitpid (child1, &stat1, 0);
  waitpid (child2, &stat2, 0);

  if (stat1 == 0 && stat2 == 0)
    {
      return 0;
    }
  else
    {
      return 1;
    }
}