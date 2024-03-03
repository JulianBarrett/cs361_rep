#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"
#include "hash.h"
#include "process.h"

// No command line can be more than 100 characters
#define MAXLENGTH 100

void
shell (FILE *input)
{
  hash_init (100);
  hash_insert ("?", "0");
  char buffer[MAXLENGTH];
  while (1)
    {
      // Print the cursor and get the next command entered
      printf ("$ ");
      memset (buffer, 0, sizeof (buffer));
      if (fgets (buffer, MAXLENGTH, input) == NULL)
        break;

      if (input != stdin)
        printf ("%s", buffer);

      if (strncmp (buffer, "echo", 4) == 0)
        echo (&buffer[5]);

      if (strncmp (buffer, "cd", 2) == 0)
        {
          char *dir = &buffer[3];
          dir[strlen (dir) - 1] = '\0';
          if (chdir (dir) != 0)
            perror ("cd");
        }

      if (strncmp (buffer, "pwd", 3) == 0)
        pwd ();

      if (strncmp (buffer, "export", 6) == 0)
        {
          char *kvpair = strtok (&buffer[7], " \t\n");
          export(kvpair);
        }

      if (strncmp (buffer, "unset", 6) == 0)
        {
          char *key = strtok (&buffer[6], " \t\n");
          unset (key);
        }

      if (strncmp (buffer, "which", 5) == 0)
        {
          char *cmd = strtok (&buffer[6], " \t\n");
          which (cmd);
        }

      if (strncmp (buffer, "./bin/ls", 8) == 0)
        {
          char *argv[10];
          argv[0] = "./bin/ls";
          int argc = 1;

          char *path_dir = strtok (&buffer[9], " \t\n");
          while (path_dir != NULL)
            {
              argv[argc++] = path_dir;
              path_dir = strtok (NULL, " \t\n");
            }
          argv[argc] = NULL;

          int status = run_single ("./bin/ls", argv);
          char status_str[10];
          snprintf (status_str, sizeof (status_str), "%d", status);
          hash_remove ("?");
          hash_insert ("?", status_str);
          continue;
        }

      if (strncmp (buffer, "./bin/head", 10) == 0)
        {
          char *args[10];
          args[0] = "./bin/head";
          int c = 1;

          char *arg = strtok (&buffer[11], " \t\n");
          while (arg != NULL)
            {
              args[c++] = arg;
              arg = strtok (NULL, " \t\n");
            }
          args[c] = NULL;

          int status = run_single ("./bin/head", args);
          char status_str[10];
          snprintf (status_str, sizeof (status_str), "%d", status);
          hash_remove ("?");
          hash_insert ("?", status_str);
          continue;
        }

      if (strncmp (buffer, "./bin/env", 9) == 0)
        {
          char *args[20];
          args[0] = "./bin/env";
          int c = 1;

          char *arg = strtok (&buffer[10], " \t\n");
          while (arg != NULL)
            {
              args[c++] = arg;
              arg = strtok (NULL, " \t\n");
            }
          args[c] = NULL;

          int pipe_i = -1;
          for (int i = 1; i < c; i++)
            {
              if (strcmp (args[i], "|") == 0)
                {
                  pipe_i = i;
                  break;
                }
            }

          if (pipe_i != -1)
            {
              char *cmd1_args[20];
              char *cmd2_args[20];

              for (int i = 0; i < pipe_i; i++)
                {
                  cmd1_args[i] = args[i];
                }
              cmd1_args[pipe_i] = NULL;

              int c2 = 0;
              for (int i = pipe_i + 1; i < c; i++)
                {
                  cmd2_args[c2++] = args[i];
                }
              cmd2_args[c2] = NULL;

              int status = run_piped (cmd1_args[0], cmd2_args[0], cmd1_args,
                                      cmd2_args);
              char status_str[10];
              snprintf (status_str, sizeof (status_str), "%d", status);
              hash_remove ("?");
              hash_insert ("?", status_str);
              continue;
            }
          else
            {
              int status = run_single ("./bin/env", args);
              char status_str[10];
              snprintf (status_str, sizeof (status_str), "%d", status);
              hash_remove ("?");
              hash_insert ("?", status_str);
              continue;
            }
        }

      if (strncmp (buffer, "quit", 4) == 0)
        break;
    }
  printf ("\n");
  hash_destroy ();
}
