// file 2.c
//  preparation of global-big-brother commanding log file


#include <stdio.h>
#include <stdlib.h>

// SPECIFIC INCLUDES

#include <unistd.h>
#include <libgen.h>


main( int argc, char *argv[]) {

  char b1[PATH_MAX], b2[PATH_MAX];
  char current_time[26];
  char *user_name;
  char *given_program_name, *base_program_name, *real_program_name;
  char *current_directory;
  int now;
  FILE *log_file;

  now = time( NULL);
  strcpy( current_time, (char *)(ctime(&now)+4));
  current_time[15] = '\0';
  
  current_directory = getwd( b1);
  user_name = getlogin();
  given_program_name = argv[0]; 
  real_program_name = realpath( given_program_name, b2); 
  base_program_name = (char *) basename( real_program_name);

  printf( "Current Time is:       %s\n", current_time);
  printf( "User Name is:          %s\n", user_name);
  printf( "Base Program Name is:  %s\n", base_program_name);
  printf( "Real Program Name is:  %s\n", real_program_name);
  printf( "Current Directory is:  %s\n", current_directory);
  printf( "Given Program Name is: %s\n", given_program_name);

  log_file = fopen( "/tmp/commanding_log.file", "a");
  if (log_file != NULL) {
    fprintf( log_file, "// %s %-8s %-8s (%s) %s\n",
            current_time, user_name,
            base_program_name, real_program_name,
            current_directory);
    fclose( log_file);
  }
}
/*
  char *ctime(
          const time_t *timer) ;

       Tue Nov  9 15:37:29 1993\n\0
       012345678901234567890
           123456789012345
/usr0/users/lebedev/AMS/Commanding../2
01234567890123456789012345678901234567890
          1         2         3         4           
*/

/*
     char *realpath(
          const char *file_name,
          char *resolved_name);
*/
/*
  #include <libgen.h>

  char *basename(
          char *path);

  char *dirname(
          char *path);
*/
