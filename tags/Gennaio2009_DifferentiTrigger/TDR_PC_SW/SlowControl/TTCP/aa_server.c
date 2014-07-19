// file aa_server.c
//
// the server part to Test ADC - Weixin.Qiao Mar-2004...
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
//#include <bits/pthreadtypes.h>

#include "uscmlib.h"

int JMDC_ID = 0x01;
int USCM_ID = 0x190;       //   1xx stands for Newborn ID
int P = 0;

volatile time_t now, then;
char  filename[256]="adc90";
FILE * fp;

volatile int USCMHasError=FALSE;	//flag: USCM encounters error
volatile int clientError=FALSE;		//flag: client JMDC encounters error first

char STRErr1[]="Ready to reboot USCM!";
char STRErr2[]="Have Rebooted USCM!";
char STRErr3[]="Everything is OK!";

volatile int which_is_error=-1;
volatile int client_JMDC_ID=0;
char error_time[256];

int sockfd,new_fd; 

volatile int endprogram=FALSE;

pthread_mutex_t mutex;

//-----------------------------------------------------------------------------
void print_running_time(void) 
{

  int h, m, s;

  now = time(NULL);
//	printf("now is set to %s\n",ctime(&now));
  s =  (now - then) % 60;
  m = ((now - then) / 60) % 60;
  h =  (now - then) / 3600;
  printf("Running time HH:MM:SS = %02d:%02d:%02d \n", h, m, s);
}

void thread_do_test(void)
{
	int16 err1,err2;
	int n1;

	int16 dat[64];
	int Running=1;	
	time_t time1,time2;
	int tempJMDC_ID;
	char temp[256];
	char temp1[64],temp2[64],temp3[64];
	
	int s,m,h;
	
	while (Running) 
	{
		if(endprogram==TRUE) pthread_exit(NULL);
//		pthread_mutex_lock(&mutex);
		if (USCMHasError==FALSE && clientError==FALSE)
		{

	   		read_all_ADC(&n1, &dat[0], &err1);
			printf(".");
			if (err1 || P > 0) printf("read_all_ADC: err=%04X\n", err1);
	
	    	if (err1)
			{
				printf("Error when server: read_all_ADC\n");

				USCMHasError=TRUE;
				which_is_error=JMDC_ID;
	
				now = time(NULL);

			}	


		}
		else
		{
			if(which_is_error==JMDC_ID)	//this JMDC is error
			{
				sprintf(temp,"\n\n------\n------\nError:\n      JMDC: %d\n",which_is_error);
				//send
				send(new_fd,STRErr1,strlen(STRErr1),0);
			}
			else
			{
				sprintf(temp,"\n\n------\n------\nError:\n      JMDC: {%d}--client JMDC_ID\n",client_JMDC_ID);
			}

  			s =  (now - then) % 60;
  			m = ((now - then) / 60) % 60;
  			h =  (now - then) / 3600;

			time2=time(NULL);
			strcpy(temp1,ctime(&time2));
			time2=then;
			strcpy(temp2,ctime(&time2));
			time2=now;
			strcpy(temp3,ctime(&time2));
			sprintf(error_time,"DateTime: %s this test start at: %s end at: %s running time HH:MM:SS = %02d:%02d:%02d\n",temp1,temp2,temp3,h,m,s);
			printf("%s",temp);
			printf("%s",error_time);
		
			fputs(temp,fp);

			fputs(error_time,fp);
			time1=time(NULL);
			printf("delay 3 second to reboot USCM...\n");
			while((time(NULL)-time1)<3)
			{
				;
			}

			tempJMDC_ID=JMDC_ID;	//replace defferent JMDC_ID to boot and start_USCM_peripherals
			JMDC_ID=JMDC_ID+1;
			if(JMDC_ID>3) JMDC_ID-=4;
			time2=time(NULL);
			boot(&err2);
			if(err2||P>0) 
			{
				printf("boot: err=%04X\n",err2);
			}
			sprintf(temp,"      DateTime: %s, boot: err=%04X\n\n",ctime(&time2), err2);
			
//			printf("temp: %s",temp);
			fputs(temp,fp);

			printf("delay 3 second to start_USCM_peripherals...\n");
			time1=time(NULL);
			while((time(NULL)-time1)<3)
			{
				;
			}	
			time2=time(NULL);
			start_USCM_peripherals(&err2);
			JMDC_ID=tempJMDC_ID;
			sprintf(temp,"DateTime: %s, start_USCM_peripherals: err=%04X\n------\n------\n",ctime(&time2), err2);
			fputs(temp,fp);
			fflush(fp);
			if (err2 || P > 0) printf("start_USCM_peripherals: err=%04X\n", err2);
			if (err2)	exit(1);

			printf("server send \"%s\" to client...\n",STRErr2);
			send(new_fd,STRErr2,strlen(STRErr2),0);

			then=time(NULL);	//restart to timer
//			printf("then is set to %s\n",ctime(&then));
			clientError=FALSE;
			USCMHasError=FALSE;	
			which_is_error=-1;
		} 
//		pthread_mutex_unlock(&mutex);		
	}

}



void closefile()
{
	char temp[256];
	time_t time2;
	sprintf(temp,"//--------------------------------------Test ended, DateTime: %s\n\n",asctime(localtime(&time2)));
	fputs(temp,fp);
	fflush(fp);
	fclose(fp);
	close(new_fd);
	close(sockfd);
}

void ctrl_c_op(int signo)
{
	endprogram=TRUE;
}



//-----------------------------------------------------------------------------

main(int argc, char *argv[]) 
{
	int16 err;
 	int i;
 
//  float val[256];
  	int Running = 1;

	char temp[256];
	
	time_t time1,time2;
  	int h, m, s;


	struct sockaddr_in server_addr; 
	struct sockaddr_in client_addr; 
	int sin_size,portnumber=2323; 
	char socketbuffer[256];

	char hello[]="Hello! the server is ready!\n"; 

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
  	printf("Usage: %s [<portnumber> <resultfile> <USCM ID> [<JMDC ID> [<Debug Printout Level>]]]\n",argv[0]);
	printf("teminate me to press:ctrl + c !\n");
	if(argc >1)
	{
		if (atoi(argv[1])>0) portnumber=atoi(argv[1]) ;
	}
		
	if(argc>2)	strcpy(filename,argv[2]);

	if (argc > 3) USCM_ID = strtoul(argv[3], NULL, 16);
  	if (argc > 4) JMDC_ID = atoi(argv[4]);
  	if (argc > 5) P = atoi(argv[5]);

	printf("portnumber: %d\n",portnumber);
	printf("resultfile: %s\n",filename);
  	printf("USCM ID = 0x%02X\n", USCM_ID);
  	printf("JMDC ID = %d\n", JMDC_ID);
  	printf("P       = %d\n", P);

	if ((fp=fopen(filename,"a"))==NULL)
	{	
		printf("Can't open target file: %s\n",filename);
		exit(1);
	}

	time2=time(NULL);
	sprintf(temp,"\n\n//------------------------------------------------Test started, DateTime: %s\n",asctime(localtime(&time2)));
	fputs(temp,fp);
	atexit(closefile);
 	if (!initialize_EPP()) exit(1);

/*	time2=time(NULL);
	while((time(NULL)-time2)<3)	;*/

	start_USCM_peripherals(&err);

  	if (err || P > 0) printf("start_USCM_peripherals: err=%04X\n", err);
	if (err){	exit(1);}

/* start to construct socket scription*/ 
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) 
	{ 
		fprintf(stderr,"Socket error:%s\n\a",strerror(errno)); 
		exit(1); 
	} 
	


/* fill out the sockaddr struct */ 
	bzero(&server_addr,sizeof(struct sockaddr_in)); 
	server_addr.sin_family=AF_INET; 
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY); 
	server_addr.sin_port=htons(portnumber);
 
/* bind sockfd scription*/ 
	if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
	{ 
		fprintf(stderr,"Bind error:%s\n\a",strerror(errno)); 
		exit(1); 
	} 

/* listen sockfd scription */ 
	if(listen(sockfd,5)==-1) 
	{ 
		fprintf(stderr,"Listen error:%s\n\a",strerror(errno)); 
		exit(1); 
	} 

 
		/*server is blocked until it is connected by the client*/ 
		sin_size=sizeof(struct sockaddr_in); 


		if((new_fd=accept(sockfd,(struct sockaddr *)(&client_addr),&sin_size))==-1) 
		{ 
			fprintf(stderr,"Accept error:%s\n\a",strerror(errno)); 
			exit(1); 
		} 

		fprintf(stderr,"Server get connection from %s\n", inet_ntoa(client_addr.sin_addr)); 

		send(new_fd,hello,strlen(hello),0); 



//		pthread_mutex_init(&mutex,NULL);

		i=pthread_create(&id1,NULL,(void*)thread_do_test,NULL);
		if(i!=0)
		{
			printf("Create pthread \"thread_do_test\" error\n");
			exit(1);
		}

  		then = time(NULL);

		while (Running) 
		{
			if(endprogram==TRUE)
			{
				break;	
			}
			if((i=recv(new_fd,socketbuffer,256,0))==-1)
			{
				fprintf(stderr,"recv error:%s\n\a",strerror(errno)); 
				endprogram=TRUE;
				break; 
			}
			
			socketbuffer[i]='\0';
			printf("client to server: %s\n",socketbuffer);
			if(i==0)
			{
				printf("client has exited, so the connetion will close!\n");
				endprogram=TRUE;
				break;
			}
//			pthread_mutex_lock(&mutex);
			if(strcmp(socketbuffer,STRErr1)==0)
			{

				USCMHasError=TRUE;
				clientError=TRUE;
				now=time(NULL);
//				printf("now is set to %s",ctime(&now));

			}
//			pthread_mutex_unlock(&mutex);
			if(strcmp(socketbuffer,STRErr2)==0)
			{
				USCMHasError=FALSE;
				clientError=FALSE;
			}	
			
		}				   			

		pthread_join(id1,NULL);
		printf("id finished!\n");
//		pthread_join(id2,NULL);
		/*this communication ends*/ 
		exit(0);
}
