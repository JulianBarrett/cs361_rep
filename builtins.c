#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include "hash.h"


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

  if (strncmp(message, "$?", 2) == 0)
    {
      char *value = hash_find (".?");
      printf ("%s\n", value); // print the mapping
      return 0;
    }

  char *key;
  char *value;
  
  //if (token != NULL) 
  if (strstr(message, "${") != NULL)
    {
      char *token = strtok( message, "${");
      key = strdup(token);
      token = strtok(NULL, "${");
      if (token != NULL)
        {
          value = strdup(token);
          size_t length = strlen(value);
            if (length > 0) {
                value[length - 2] = '\0';
            }
        }
      char *value2 = hash_find (value);
      if (value2 == NULL) {
        printf("%s\n", key);
        free (key);
        free (value);
        return 0;
      } else {
        printf("%s%s\n", key, value2);
        free (key);
        free (value);
        return 0;
      }
    }


  if (message != NULL)
      message[strlen(message) - 1] = '\0';

  // char *token = strtok(message, "\\n");
  char *token2 = strtok(message, "\\n");
  do 
  {
    printf("%s\n", token2);
    token2 = strtok (NULL, "\\n");
  } while (token2 != NULL);

  return 0;
}

// Given a key-value pair string (e.g., "alpha=beta"), insert the mapping
// into the global hash table (hash_insert ("alpha", "beta")).
//
// Returns 0 on success, 1 for an invalid pair string (kvpair is NULL or
// there is no '=' in the string).
int
export (char *kvpair)
{
  // hash_init (100); // store up to 100 items initially
  char *key;
  char *value;
  char *token = strtok(kvpair, "=");
  if (token != NULL) 
    {
      key = strdup(token);
      token = strtok(NULL, "=");
      if (token != NULL)
        {
          value = strdup(token);
        } else {
          //  free (key);
          // free (value);
          return 1;
        }
    } else {
      // free (key);
      // free (value);
      return 1;
    }

  hash_insert (key, value);
  // hash_dump();
  free (key);
  free (value);
  return 0;
}


// Prints the current working directory (see getcwd()). Returns 0.
int
pwd (void)
{
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  printf("%s\n", cwd);
  return 0;
}

// Removes a key-value pair from the global hash table.
// Returns 0 on success, 1 if the key does not exist.
int
unset (char *key)
{
  hash_remove (key); // removes hash[key] from the table
  char *value = hash_find (key);
  if (value == NULL) {
    return 1;
  }
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
  return 0;
}


// had to replace newline with null terminator
int
changeDir(const char *path) {
    if (chdir(path) == 0) 
      {
        return 0;
      } 
        else 
      {
        printf("%s", path);
        return -1;
      }
}
