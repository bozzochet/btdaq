#include "aallib.h"

#ifdef OS_LINUX
#include <sys/fcntl.h>
#include <sys/user.h>
#include <sys/types.h>
#include <unistd.h> // read()
#include <malloc.h>

#include <sys/stat.h>
#include <stdlib.h>
#include <pwd.h>
#endif // OS_ECOS

char *ExpandTilde(char *filename)
{
#ifdef OS_LINUX

static char *expanded_filename=NULL;
char *home_dir, *p;
struct passwd *passwd = NULL;

  if (!filename) 
     return NULL;

  if (filename[0] != '~')
     return filename;

  if (filename[1] == '/')
  {
      home_dir = getenv("HOME");
      filename++;
  }
  else
  {
  char *s;
  char *t;

    s = filename+1;
    while(*s && *s != '/') s++;

    if ((t = malloc(s - filename)))
    {
      memcpy(t, filename+1, s-filename-1);
      t[s-filename-1] = 0;
      passwd = getpwnam(t);
      free(t);
    }

    if (!passwd)
      return filename;

    filename = s;
    home_dir = passwd->pw_dir;
  }

  if ((p = realloc(expanded_filename, strlen(filename)+strlen(home_dir)+1)))
  {
    strcat(strcpy(expanded_filename = p, home_dir), filename);
    return expanded_filename;
  }
#endif // OS_ECOS
  return filename;
}

