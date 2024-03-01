#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// You may assume that lines are no longer than 1024 bytes
#define LINELEN 1024

static void usage (void);

int
main (int argc, char *argv[])
{
  int lines = 5;
  char line[LINELEN];
  FILE *file = stdin;

  for (int i = 1; i < argc; i++)
    {
      if (strcmp (argv[i], "-n") == 0)
        {
          if (i + 1 < argc)
            {
              lines = atoi (argv[++i]);
              if (lines < 1)
                {
                  fprintf (stderr, "Invalid number of lines: %d\n", lines);
                  usage();
                  return EXIT_FAILURE;
                }
            }
          else
            {
              fprintf (stderr, "Option -n requires an argument.\n");
              usage();
              return EXIT_FAILURE;
            }
        }
      else
        {
          file = fopen (argv[i], "r");
          if (!file)
            {
              return EXIT_FAILURE;
            }
        }
    }
  
  for (int count = 0; count < lines && fgets (line, LINELEN, file); count++)
    {
      printf("%s", line);
    }
  
  if (file != stdin)
    {
      fclose (file);
    }

  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("head, prints the first few lines of a file\n");
  printf ("usage: head [FLAG] FILE\n");
  printf ("FLAG can be:\n");
  printf ("  -n N     show the first N lines (default 5)\n");
  printf ("If no FILE specified, read from STDIN\n");
}
