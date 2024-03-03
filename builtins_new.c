#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Given a message as input, print it to the screen followed by a
// newline ('\n'). If the message contains the two-byte escape sequence
// "\\n", print a newline '\n' instead. No other escape sequence is
// allowed. If the sequence contains a '$', it must be an environment
// variable or the return code variable ("$?"). Environment variable
// names must be wrapped in curly braces (e.g., ${PATH}).
//
// Returns 0 for success, 1 for errors (invalid escape sequence or no
// curly braces around environment variables).
int
echo (char *message)
{
  if (message != NULL)
    {
      message[strlen (message) - 1] = '\0';
    }

  char *next;
  while ((next = strpbrk (message, "\\$")) != NULL)
    {
      if (*next == '\\')
        {
          if (*(next + 1) == 'n')
            {
              *next = '\0';
              printf ("%s\n", message);
              message = next + 2;
              continue;
            }
          else
            {
              return 1;
            }
        }
      else if (*next == '$')
        {
          if (*(next + 1) == '{')
            {
              char *end = strchr (next, '}');
              if (!end)
                {
                  return 1;
                }
              *next = '\0';
              printf ("%s", message);

              *end = '\0';
              char *envName = next + 2;
              char *envVal = hash_find (envName);
              if (envVal)
                {
                  printf ("%s", envVal);
                }
              message = end + 1;
              continue;
            }
          else if (*(next + 1) == '?')
            {
              char *last_code = hash_find ("?");
              printf ("%d", atoi (last_code));
              message = next + 2;
              continue;
            }
          else
            {
              return 1;
            }
        }
    }
  printf ("%s\n", message);
  return 0;
}

// Given a key-value pair string (e.g., "alpha=beta"), insert the mapping
// into the global hash table (hash_insert ("alpha", "beta")).
//
// Returns 0 on success, 1 for an invalid pair string (kvpair is NULL or
// there is no '=' in the string).
int export(char *kvpair)
{
  if (kvpair == NULL)
    {
      return 1;
    }
  char *equal = strchr (kvpair, '=');
  if (equal == NULL)
    {
      return 1;
    }

  *equal = '\0';
  char *key = kvpair;
  char *value = equal + 1;

  if (key[0] == '\0' || value[0] == '\0')
    {
      return 1;
    }

  hash_insert (key, value);
  return 0;
}

// Prints the current working directory (see getcwd()). Returns 0.
int
pwd (void)
{
  char cwd[1024];
  if (getcwd (cwd, sizeof (cwd)) != NULL)
    {
      printf ("%s\n", cwd);
      return 0;
    }
  else
    {
      perror ("getcwd");
      return 1;
    }
}

// Removes a key-value pair from the global hash table.
// Returns 0 on success, 1 if the key does not exist.
int
unset (char *key)
{
  if (key == NULL)
    {
      return 1;
    }
  if (!hash_find (key))
    {
      return 1;
    }

  hash_remove (key);
  return 0;
}

// Given a string of commands, find their location(s) in the $PATH global
// variable. If the string begins with "-a", print all locations, not just
// the first one.
//
// Returns 0 if at least one location is found, 1 if no commands were
// passed or no locations found.
int
which (char *cmdline)
{
  if (cmdline == NULL || cmdline[0] == '\0')
    {
      return 1;
    }

  int printAll = 0;
  if (strncmp (cmdline, "-a", 2) == 0)
    {
      printAll = 1;
      cmdline += 2;
    }

  char *token = strtok (cmdline, " ");
  while (token != NULL)
    {
      if (strcmp (token, "dukesh") == 0)
        {
          printf ("%s: dukesh built-in command\n", token);
          if (!printAll)
            {
              return 0;
            }
        }
      else if (strcmp (token, "cd") == 0 || strcmp (token, "echo") == 0
               || strcmp (token, "pwd") == 0 || strcmp (token, "which") == 0
               || strcmp (token, "export") == 0)
        {
          printf ("%s: dukesh built-in command\n", token);
          if (!printAll)
            {
              return 0;
            }
        }
      else if (strncmp (token, "./", 2) == 0)
        {
          if (access (token, X_OK) == 0)
            {
              printf ("%s\n", token);
              if (!printAll)
                {
                  return 0;
                }
            }
        }
      else
        {
          char *path = getenv ("PATH");
          char *pathCopy = strdup (path);
          char *pathTok = strtok (pathCopy, ":");

          while (pathTok != NULL)
            {
              char full[1000];
              snprintf (full, sizeof (full), "%s/%s", pathTok, token);

              FILE *fp = fopen (full, "r");
              if (fp)
                {
                  fclose (fp);
                  printf ("%s\n", full);
                  if (!printAll)
                    {
                      free (pathCopy);
                      return 0;
                    }
                }
              pathTok = strtok (NULL, ":");
            }
          free (pathCopy);
        }
      token = strtok (NULL, " ");
    }
  return 1;
}
