#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

static void usage (void);
static void list (char *dir_path, int show_all, int show_sizes);


static void
usage (void)
{
  printf ("ls, list directory contents\n");
  printf ("usage: ls [FLAG ...] [DIR]\n");
  printf ("FLAG is one or more of:\n");
  printf ("  -a       list all files (even hidden ones)\n");
  printf ("  -s       list file sizes\n");
  printf ("If no DIR specified, list current directory contents\n");
}


int
main (int argc, char *argv[])
{
  int show_all = 0;
  int show_sizes = 0;
  int show_help = 0;
  char *dir_path = ".";

  for (int i = 1; i < argc; i++)
    {
      if (strcmp (argv[i], "-h") == 0 || strcmp (argv[i], "--help") == 0)
        {
          show_help = 1;
          break;
        }
      else if (argv[i][0] == '-')
        {
          int j = 1;
          while (argv[i][j] != '\0')
            {
              switch (argv[i][j])
                {
                  case 'a':
                    show_all = 1;
                    break;
                  case 's':
                    show_sizes = 1;
                    break;
                  default:
                    return EXIT_FAILURE;
                }
              j++;
            }
        }
      else
        {
          dir_path = argv[i];
        }
    }
  
  if (show_help)
    {
      usage();
      return EXIT_SUCCESS;
    }
  list (dir_path, show_all, show_sizes);
  return EXIT_SUCCESS;
}

static void
list (char *dir_path, int show_all, int show_sizes)
{
  DIR *dir = opendir (dir_path);
  if (dir == NULL)
    {
      perror ("Directory does not exist.");
      return;
    }

  struct dirent *entry;
  while ((entry = readdir (dir)) != NULL)
    {
      if (!show_all && entry->d_name[0] == '.')
        {
          continue;
        }

      if (show_sizes)
        {
          struct stat file_stat;
          char path[1024];
          snprintf (path, sizeof (path), "%s/%s", dir_path, entry->d_name);
          if (stat (path, &file_stat) == 0)
            {
              printf ("%10ld %s\n", file_stat.st_size, entry->d_name);
            }
          else
            {
              printf ("      ? %s\n", entry->d_name);
            }
        }
      else
        {
          printf ("%s\n", entry->d_name);
        }
    }
  closedir (dir);
}