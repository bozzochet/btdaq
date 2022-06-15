// file gsclib.h
//
// see gsclib.c
//

#ifndef _GSCLIB_H
#define _GSCLIB_H

//~----------------------------------------------------------------------------

void remove_trailing_slash(char *s);
int last_file_in(char *DirPath, int n);
int the_last_file_in(char *directory);
FILE *gcsOpenTempFile(char *Path, int FileNumber);
int gcsRenameTempFile(char *Path, int FileNumber, int SyncFileNumber);
int gcsMoveTempFile(char *Path, int FileNumber, char *user, char *group);
int gcsUnlinkTempFile(char *Path, int FileNumber);
int dir_check_and_create(char *name);
int dir_check_and_create_2(char *name, char *user, char *group);

#endif // _GSCLIB_H
