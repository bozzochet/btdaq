#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fnmatch.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

int ftplib_debug = 0;

#define FTP_BUFSIZ 8192

int sControl;

typedef struct NetBuf {
  char *cput, *cget;
  int handle;
  int cavail, cleft;
  char buf[FTP_BUFSIZ];
  char response[256];
} NETBUF;

NETBUF nControl, nData;

int remote_file_descriptor = -1;
long remote_file_size      = -1;
//-----------------------------------------------------------------------

//
//   read a line of text from the ftp-server
//
//   return -1 on error or bytecount
//

int readline( char *buf, int max, NETBUF *ctl) {

  int x, retval = 0;
  char *end;
  int eof = 0;

  if (max == 0) return 0;

  while (1) {
    if (ctl->cavail > 0) {
      x = (max >= ctl->cavail) ? ctl->cavail : max-1;
      end = memccpy( buf, ctl->cget, '\n', x);
      if (end != NULL) x = end - buf;
      retval      += x;
      buf         += x;
      *buf         = '\0';
      max         -= x;
      ctl->cget   += x;
      ctl->cavail -= x;
      if (end != NULL) break;
    }
    if (max == 1) {
      *buf = '\0';
      break;
    }
    if (ctl->cput == ctl->cget) {
      ctl->cput = ctl->cget = ctl->buf;
      ctl->cavail = 0;
      ctl->cleft = FTP_BUFSIZ;
    }
    if (eof) {
      if (retval == 0) retval = -1;
      break;
    }
    if ((x = read( ctl->handle, ctl->cput, ctl->cleft)) == -1) {
      perror( "read() in readline()");
      retval = -1;
      break;
    }
    if (x == 0) eof = 1;
    ctl->cleft  -= x;
    ctl->cavail += x;
    ctl->cput   += x;
  }
  return retval;
}

//-----------------------------------------------------------------------

//
//   read a response from the ftp-server and check first char
//
//   return 0 if first char doesn't match
//   return 1 if first char matches
//

int readresp( char c) {

  char match[5];
    
  if (readline( nControl.response, 256, &nControl) == -1) {
    perror( "readline() in readresp()");
    return 0;
  }
  
  if (ftplib_debug > 1) printf( "%s", nControl.response);
  if (nControl.response[3] == '-') {
    strncpy( match, nControl.response, 3);
    match[3] = ' ';
    match[4] = '\0';
    while (strncmp( nControl.response, match, 4)) {
      if (readline( nControl.response, 256, &nControl) == -1) {
        perror( "multi-line readline() in readresp()");
        return 0;
      }
      if (ftplib_debug > 1) printf( "%s", nControl.response);
    }
  }
  if (nControl.response[0] == c) return 1;
  return 0;
}

//-----------------------------------------------------------------------

//
//   Ftp_Send_Cmd - send a command and wait for expected response
//
//   return 1 if proper response received, 0 otherwise
//

int Ftp_Send_Cmd( char *cmd, char expresp)
{
  char buf[256];
  if (ftplib_debug > 2) printf( "%s\n", cmd);
  sprintf( buf, "%s\r\n", cmd);
  if (write( nControl.handle, buf, strlen(buf)) <= 0) {
    perror( "write() in Ftp_Send_Cmd()");
    return 0;
  }    
  return readresp( expresp);
}

//-----------------------------------------------------------------------

//
//   Ftp_Connect - connect to and log into remote ftp-server
//
//   return 1 if OK, 0 if not
//

int Ftp_Connect( char *host, char *user, char *pass) {

  struct sockaddr_in sin;
  struct hostent *phe;
  struct servent *pse;
  int on = 1;
  char buf[64];

  memset( &sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;

  if ((pse = getservbyname( "ftp", "tcp")) == NULL) {
    perror( "getservbyname() in Ftp_Connect()");
    return 0;
  }
  
  sin.sin_port = pse->s_port;

  if ((phe = gethostbyname( host)) == NULL) {
    perror("gethostbyname() in Ftp_Connect()");
    return 0;
  }
  
  memcpy( (char *)&sin.sin_addr, phe->h_addr, phe->h_length);
  
  if ((sControl = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror( "socket() in Ftp_Connect()");
    return 0;
  }
  
  if (setsockopt( sControl, SOL_SOCKET, SO_REUSEADDR,
                  (void *) &on, sizeof( on)) == -1) {
    perror( "setsockopt() in Ftp_Connect()");
    close( sControl);
    return 0;
  }

//fcntl(sControl,F_SETFL,O_NONBLOCK);
  
  if (connect( sControl, (struct sockaddr *) &sin, sizeof( sin)) == -1) {
    perror( "connect() in Ftp_Connect()");
    close( sControl);
    return 0;
  }
  
  nControl.handle = sControl;
  
  if (readresp( '2') == 0) {
    close( sControl);
    return 0;
  }
  
//fcntl(sControl,F_SETFL,O_NONBLOCK);
  
//printf("readresp=%d\n",readresp('2'));
  
  sprintf( buf, "USER %s", user);
  if (!Ftp_Send_Cmd( buf, '3')) return 0;
  sprintf( buf, "PASS %s", pass);
  return Ftp_Send_Cmd( buf, '2');
}

//-----------------------------------------------------------------------

//
//   Ftp_Disconnect - disconnect from remote ftp-server
//

void Ftp_DisConnect( void) {

  close( sControl);

}

//-----------------------------------------------------------------------

//
//    Ftp_Port - set up data connection
//
//    return 'sData' if successful, -1 otherwise
//

int Ftp_Port( void) {

  int sData;
  union {
    struct sockaddr sa;
    struct sockaddr_in in;
  } sin;
  struct linger lng = { 0, 0 };
  char buf[64];
  int i;
  int on = 1;
  char *cp;
  int v[6];

  memset (&sin, 0, sizeof( sin));
  sin.in.sin_family = AF_INET;
  
  if (!Ftp_Send_Cmd( "PASV", '2')) return -1;
  
  if ((cp = strchr( nControl.response, '(')) == NULL) return -1;
  
  cp++;
  sscanf( cp, "%d,%d,%d,%d,%d,%d", &v[2], &v[3], &v[4], &v[5], &v[0], &v[1]);
  for (i=0; i<=5; i++) sin.sa.sa_data[i] = v[i];

  if ((sData = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror( "socket() in Ftp_Port()");
    return -1;
  }
  
  if (setsockopt( sData, SOL_SOCKET, SO_REUSEADDR,
		  (void *) &on, sizeof( on)) == -1) {
    perror( "setsockopt(...SO_REUSEADDR...) in Ftp_Port()");
    close( sData);
    return -1;
  }
  
  if (setsockopt( sData, SOL_SOCKET, SO_LINGER,
		  (void *) &lng, sizeof(lng)) == -1) {
    perror( "setsockopt(...SO_LINGER...) in Ftp_Port()");
    close( sData);
    return -1;
  }
  
  if (connect( sData, &sin.sa, sizeof(sin.sa)) == -1) {
    perror( "connect() in Ftp_Port()");
    close( sData);
    return -1;
  }
  
  return sData;
}

//-----------------------------------------------------------------------

//
//   Ftp_Open - issue a RETR command to open remote file
//
//   return remote file descriptor if successful, -1 otherwise
//   assign values to global variables:
//                                     remote_file_descriptor
//                                     remote_file_size
//

int Ftp_Open( char *path) {

  int sData;
  char buf[256];
  char *c;

  if (remote_file_descriptor >=0 ) {
    printf( "try to close remote file...\n");  
  }
  
  remote_file_descriptor = -1;
  remote_file_size       = -1;

  if (!Ftp_Send_Cmd( "TYPE I", '2')) return -1;
  
  if ((sData = Ftp_Port()) == -1) return -1;
  
  sprintf( buf, "RETR %s", path);
  if (!Ftp_Send_Cmd( buf, '1')) {
    if ((shutdown( sData, SHUT_RDWR) == -1)) perror( "shutdown() in Ftp_Open()");
    if ((close( sData) == -1)) perror( "close() in Ftp_Open()");
    return -1;
  }

  remote_file_descriptor = sData;
  
  if (c = strchr( nControl.response, '(')) remote_file_size = atoi( ++c);

  return sData;
}

//-----------------------------------------------------------------------

//
//   Ftp_Close - read ftp-server reply and close data connection
//
//   return 0 if successful, -1 otherwise
//   assign values to global variables:
//                                     remote_file_descriptor
//                                     remote_file_size
//

int Ftp_Close( int sData) {

  if (remote_file_descriptor < 0) {
    printf( "remote file is already closed???\n");
  }
  
  remote_file_descriptor = -1;
  remote_file_size       = -1;
  
  if (shutdown( sData, SHUT_RDWR) == -1) perror( "shutdown() in Ftp_Close()");
  if (close( sData) == -1) perror( "close() in Ftp_Close()");

  if (readresp( '2')) return 0;
 
  return -1; 
}


//-----------------------------------------------------------------------

//
//   Ftp_last_file_number_in - issue CWD + NLST commands and do the job
//
//   return:
//            >0   last file number
//            =0   no files found
//            -1   errors
//
 
long Ftp_last_file_number_in( char *directory_name) {

  int sData, l, reply;
  char filename[256], buf[256];
  long file_number;
  long last_file_number;

  sprintf( buf, "CWD %s", directory_name);
  if (!Ftp_Send_Cmd( buf, '2')) return -1;

  if (!Ftp_Send_Cmd( "TYPE A", '2')) return -1;
  if ((sData = Ftp_Port()) == -1) return -1;
  if (!Ftp_Send_Cmd( "NLST", '1')) return -1;
	
  nData.handle = sData;

  last_file_number = 0;
  while ((l = readline( filename, 256, &nData))!= -1) {
    filename[l-1] = '\0';
    if (filename[l-2] == '\r') filename[l-2] = '\0';
    if (ftplib_debug > 3) printf( "Filename: %s\n", filename);
    if (fnmatch( "[0-9][0-9][0-9][0-9][0-9][0-9]", filename, 0) == 0) {
      file_number = atol( filename);
      if (ftplib_debug > 4) printf( "Filenumb: %6.6d\n", file_number);
      if (file_number > last_file_number) last_file_number = file_number;
    }
  }

  reply = readresp( '2') - 1;
  if (shutdown( sData, SHUT_RDWR) == -1) {
    reply |= -1;
    perror( "shutdown() in Ftp_last_file_number_in()");
  }
  if (close( sData) == -1) {
    reply |= -1;
    perror( "close() in Ftp_last_file_number_in()");
  }
  
  if (reply != 0) return reply;
  else            return last_file_number;
}

//-----------------------------------------------------------------------
