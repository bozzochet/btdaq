//  $Id: file_reading.h,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <stdio.h>

#define buf_n 20000 /* number of file names buffered */

typedef char ch80[80];

int last_file_read;
char * CAS_dir, * Coo_db_dir, last_file_n[80], chb80[80];
char * last_name;
int new_files;
time_t file_t, last_file_time;

ch80    new_file_n[buf_n];
time_t  new_file_t[buf_n];
int new_file_c; /* how many new files */

/* opens a CAS file */
int new_files_coming() {
  FILE *last_f;
  char chbuf80[80], last_file_n[80];
  time_t last_file_time;
  DIR *dirp;
  struct dirent *dp;
  struct stat statf;
  time_t tmin=999999999;
  new_files=0;

  /* reads the last read file name */
  last_f=fopen(last_name,"r");
  if (last_f == NULL) {
    last_file_time = 0;
    cerr <<" Couldnot open file "<<last_name<<endl;
    cerr <<" check RUNFile env"<<endl;
    exit(1);
  }
  else {
    fgets(chb80,80,last_f);
    sscanf(chb80,"%d",&last_file_time);
    //cout <<" Last time "<<last_file_time<<endl;
    fclose(last_f);
  }
  /* list directory */
  if ((dirp = opendir(CAS_dir)) == NULL) {
    cerr<< "Coo_file: open CASDir fails. Stop. Check CASDir set"<< CAS_dir<<endl;
    exit(0);
  }
  new_file_c=-1;

  while ((dp = readdir( dirp)) != NULL) {
    if (fnmatch( "[0-9][0-9][0-9][0-9][0-9][0-9]", dp->d_name, 0) == 0) {
      sprintf(chbuf80,"%s/%s",CAS_dir,dp->d_name);
      stat(chbuf80,&statf);

      file_t = statf.st_mtime;
      if ((file_t>last_file_time) && (file_t<tmin)) {

	new_file_c++;
	new_file_t[new_file_c]=file_t;
	sprintf(new_file_n[new_file_c],"%s",dp->d_name);
	new_files=1;
	if (new_file_c >= (buf_n-1))
	  return(new_files);
      }
    }
  }
  return(new_files);
}

FILE * open_file(int i) {
char chbuf[80];
FILE *fp;

  sprintf(chbuf,"%s/%s",CAS_dir,new_file_n[i]);
  fp=fopen(chbuf,"r");
  if (fp == NULL) {
    printf("Warning: CAS file %s cannot be open\n",new_file_n[i]);
    return(NULL);
  }
  return(fp);
}
