// file aa_client.c
//
//the client part for ADC test  - Weixin.Qiao Mar-2004...
//read out ADC from USCM , retry when uscm doesn't response
//based on the program tt written by A.Lebedev 

#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <pthread.h>

#include "uscmlib.h"

int JMDC_ID = 0x00;
int USCM_ID = 0x190;       //   1xx stands for Newborn ID
int P = 0;



volatile int h, m, s;
volatile time_t now, then;
volatile int endprogram=FALSE;
FILE * fp;
char STRErr1[]="Ready to reboot USCM!";
char STRErr2[]="Have Rebooted USCM!";
char STRErr3[]="Everythinng is OK!";
int sockfd,new_fd; 

volatile int HasSentError=FALSE;
volatile int USCMHasError=FALSE;	
volatile int which_is_error=-1;
volatile int server_JMDC_ID=1;
volatile char error_time[64];

pthread_mutex_t mutex;

//-----------------------------------------------------------------------------

void print_running_time(void) {

  int h, m, s;

  now = time(NULL);
  s =  (now - then) % 60;
  m = ((now - then) / 60) % 60;
  h =  (now - then) / 3600;
  printf("Running time HH:MM:SS = %02d:%02d:%02d \n", h, m, s);
}
void thread_do_test(void)
{
	int16 err;
	int n1;


	int16 dat[64];

	int Running=1;	
	int s,m,h;
	time_t time1, time2;
	char temp[256];



	while (Running) 
	{
		if(endprogram==TRUE) pthread_exit(NULL);
		if (USCMHasError==FALSE && which_is_error==-1)
		{
//			pthread_mutex_lock(&mutex);
	   		read_all_ADC(&n1, &dat[0], &err);
			printf(".");
			if (err || P > 0) printf("read_all_ADC: err=%04X\n", err);
	
	    	if (err)
			{
				printf("Error when client: read_all_ADC\n");

				USCMHasError=TRUE;
				which_is_error=JMDC_ID;
				HasSentError=FALSE;

			}	

//			pthread_mutex_unlock(&mutex);
		}
		else
		{
			if(which_is_error==JMDC_ID)	//this JMDC is error
			{
				sprintf(temp,"Error JMDC: %d--this JMDC_ID\n",JMDC_ID);
				if(HasSentError==FALSE)
				{		//send
					printf("client to server: \"USCM has Error\"\n");
					send(sockfd,STRErr1,strlen(STRErr1),0);
					HasSentError=TRUE;
				}
			}
			else if(which_is_error==server_JMDC_ID)
			{
				sprintf(temp,"Error JMDC: {%d}-- server JMDC_ID\n",server_JMDC_ID);
			}

			printf("%s",temp);
		} 


	}

}


void closefile()
{
//	fclose(fp);
	close(sockfd);
}

void ctrl_c_op(int signo)
{
	endprogram=TRUE;
}

//-----------------------------------------------------------------------------

main(int argc, char *argv[]) 
{
 	int i;
	int16 err;
  	int Running = 1;
//	char  filename[256]="adc90";
	char temp[256];
	
	time_t time1,time2;


	char socketbuffer[256];


//	char hello[]="Hello! the server is ready!\n"; 

	struct sockaddr_in server_addr; 
	struct hostent *host; 
	char hostname[16]="L3pcal2";
	int portnumber=2323,nbytes; 

	pthread_t id1,id2;

//capture "Ctrl + c "
	struct sigaction act;
	act.sa_handler=ctrl_c_op;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	if(sigaction(SIGINT,&act,NULL)<0)
	{
		fprintf(stderr,"Install Signal Action Error: %s\n\a",strerror(errno));
		exit(1);
	}
	
  	setbuf(stdout, NULL);
  	printf("Usage: %s [<hostname> <portnumber> <USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n",argv[0]);

	if(argc >1)	strcpy(hostname,argv[1]) ;
	if((host=gethostbyname(argv[1]))==NULL) 
	{ 
		fprintf(stderr,"Gethostname error\n"); 
		exit(1); 
	} 	
	if(argc >2)
	{
		if (atoi(argv[2])>0) portnumber=atoi(argv[2]) ;
	}
		
//	if(argc>3)	strcpy(filename,argv[3]);

	if (argc > 3) USCM_ID = strtoul(argv[3], NULL, 16);
  	if (argc > 4) JMDC_ID = atoi(argv[4]);
  	if (argc > 5) P = atoi(argv[5]);

	printf("hostname: %s\n",hostname);
	printf("portnumber: %d\n",portnumber);
//	printf("resultfile: %s\n",filename);
  	printf("USCM ID = 0x%02X\n", USCM_ID);
  	printf("JMDC ID = %d\n", JMDC_ID);
  	printf("P       = %d\n", P);


/* construct sockfd discription */ 
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) 
	{ 
		fprintf(stderr,"Socket Error:%s\a\n",strerror(errno)); 
		exit(1); 
	} 

	
/* fill out the client stuff */ 
	bzero(&server_addr,sizeof(server_addr)); 
	server_addr.sin_family=AF_INET; 
	server_addr.sin_port=htons(portnumber); 
	server_addr.sin_addr=*((struct in_addr *)host->h_addr); 
/* request connectting */ 
	if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
	{ 
		fprintf(stderr,"Connect Error:%s\a\n",strerror(errno)); 
		exit(1); 
	} 



/* connected successfully */ 
	i=recv(sockfd,socketbuffer,256,0);
	if(i>-1)	socketbuffer[i]='\0'; 
	printf("I have received:%s\nThe communication between server and client is established!\n",socketbuffer); 


  	if (!initialize_EPP()) exit(1);
  
	atexit(closefile);

  	start_USCM_peripherals(&err);
  	if (err || P > 0) printf("start_USCM_peripherals: err=%04X\n", err);
  	if (err) {	exit(1);}	

//	pthread_mutex_init(&mutex,NULL);
	i=pthread_create(&id1,NULL,(void*)thread_do_test,NULL);
	if(i!=0)
	{
		printf("Create pthread \"thread_do_test\" error\n");
		exit(1);
	}

	then = time(NULL);

	while (Running) 
	{
		if(endprogram==TRUE) break;
		if((i=recv(sockfd,socketbuffer,256,0))==-1)
		{
			fprintf(stderr,"recv error:%s\n\a",strerror(errno)); 
			endprogram=TRUE; 
			break;
		}
			
		socketbuffer[i]='\0';
		printf("server to client: \"%s\"\n",socketbuffer);
		if(i==0)
		{
			printf("server has exited, so I will exit!\n");
			endprogram=TRUE; 
			break;
		}
		

		if(strcmp(socketbuffer,STRErr1)==0)
		{
//			pthread_mutex_lock(&mutex);
			USCMHasError=TRUE;
			which_is_error=server_JMDC_ID;
//			pthread_mutex_unlock(&mutex);
		}
		if(strcmp(socketbuffer,STRErr2)==0)
		{
//			pthread_mutex_lock(&mutex);
			USCMHasError=FALSE;
			which_is_error=-1;
//			pthread_mutex_unlock(&mutex);
		}			

	}				   			

	pthread_join(id1,NULL);
//	pthread_join(id2,NULL);

/* end communication */ 
	close(sockfd);  
}
