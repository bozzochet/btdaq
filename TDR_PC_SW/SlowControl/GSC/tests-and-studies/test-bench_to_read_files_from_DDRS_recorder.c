// 1.c
// test-bench to read files from DDRS - recorder.

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//----------------------------------------------------------------------------

main() {
  int i;
  int in, out;
  int bit[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
  for (in=0; in<256; in++) {
    out = 0;
    for (i=0; i<8; i++) {
      if (in & bit[i]) out |= bit[7-i];
    }
    printf( "0x%02X, ", out);
    if ((in+1) % 8 == 0) printf( "\n");
  }
}

//----------------------------------------------------------------------------
