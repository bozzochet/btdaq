//  $Id: ins_dummy.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include <stdlib.h>
#ifdef  __IBMAIX__    
extern "C" void setkey(const char* a);
extern "C" void encrypt(char* a, int n);
extern "C" char* crypt(const char* a, const char* b);
extern "C"   void __setkey(const char* s){
              setkey(s);
}

extern "C" char* __crypt(const char* a, const char* b){
           return (crypt(a,b));
}


extern "C" void __encrypt(char* a, int n){
            encrypt(a,n);
}


#endif
#ifdef sun
extern "C" void _dlopen(){
};
extern "C" void _dlclose(){
};
extern "C" void _dlsym(){
};
extern "C" void _dlerror(){
};
extern "C" void dlopen(){
};
extern "C" void dlclose(){
};
extern "C" void dlsym(){
};
extern "C" void dlerror(){
};



//
// Scan the directory dirname calling select to make a list of selected
// directory entries then sort using qsort and compare routine dcomp.
// Returns the number of entries and a pointer to a list of pointers to
// struct dirent (through namelist). Returns -1 if there were any errors.
//

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
//
// The DIRSIZ macro is the minimum record length which will hold the directory
// entry.  This requires the amount of space in struct dirent without the
// d_name field, plus enough space for the name and a terminating nul byte
// (dp->d_namlen + 1), rounded up to a 4 byte boundary.
//
#undef DIRSIZ
#define DIRSIZ(dp) \
         ((sizeof(struct dirent) - sizeof(dp)->d_name) + \
         ((strlen((dp)->d_name) + 1 + 3) &~ 3))

typedef int (*select_t)( struct dirent* );
typedef int (*dcomp_t)( const void*, const void* );

extern "C" int scandir( const char* dirname,
             struct dirent*** namelist, select_t select, dcomp_t dcomp )
{
  register struct dirent* d;
  register struct dirent* p;
  register struct dirent** names;
  register size_t nitems;
  struct stat stb;
  size_t arraysz;
  DIR* dirp;

  if ( (dirp = opendir( dirname )) == NULL )
  {
    return (-1);
  }
  if ( fstat( dirp->dd_fd, &stb ) < 0 )
  {
    return (-1);
  }

  //
  // estimate the array size by taking the size of the directory file
  // and dividing it by a multiple of the minimum sizeentry.
  //
  arraysz = (stb.st_size / 24);
  names = (struct dirent **)malloc( arraysz * sizeof( struct dirent* ) );
  if ( names == NULL )
  {
    return (-1);
  }

  nitems = 0;
  while ( (d = readdir( dirp )) != NULL )
  {
    if ( select != NULL && !(*select)( d ) )
    {
      continue;       // just selected names
    }

    //
    // Make a minimum size copy of the data
    //
    p = (struct dirent *)malloc( DIRSIZ(d) );

    if ( p == NULL )
    {
      return(-1);
    }

    p->d_ino = d->d_ino;
    p->d_off = d->d_off;
    p->d_reclen = d->d_reclen;
    memcpy( p->d_name, d->d_name, strlen( d->d_name ) + 1 );

    //
    // Check to make sure the array has space left and
    // realloc the maximum size.
    //
    if ( ++nitems >= arraysz )
    {
      if ( fstat( dirp->dd_fd, &stb ) < 0 )
      {
        return (-1);     // just might have grown
      }
      arraysz = stb.st_size / 12;
      names = (struct dirent **)realloc( (char*)names,
                                         arraysz * sizeof( struct dirent* ) );
      if ( names == NULL )
      {
        return (-1);
      }
    }

    names[nitems-1] = p;
  }

  closedir( dirp );

  if ( nitems && dcomp != NULL )
  {
    qsort( names, nitems, sizeof( struct dirent* ),dcomp );
  }

  *namelist = names;

  return (nitems);
}

//
// Alphabetic order comparison routine for those who want it.
//
extern "C" int alphasort(const void* d1, const void* d2)
{
  return (strcmp( (*(struct dirent **)d1)->d_name,
                 (*(struct dirent **)d2)->d_name ));
}


#endif

#ifdef _GCCIFC_
extern "C" void __gxx_personality_v0(){
}
#endif
