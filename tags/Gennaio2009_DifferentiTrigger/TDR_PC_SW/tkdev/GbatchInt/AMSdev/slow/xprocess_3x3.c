//  $Id: xprocess_3x3.c,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
/* Monitoring of working processes */
#include <sys/stat.h>
#include <ctype.h>
#include "xform_proc_3x3.h"
int lll;
int open_log(int i, int mode);
int open_elog(int i);
int read_status();
int write_status();

int statu[N_comp];

int main (int argc, char *argv[]) {
char chbuf[80],chbuf1[80],chbuf2[80],chb_t[80], ch[256], *ch1, *ch2;
ch256 chh[30], ch256;
int i,j,nn,er,count[8],disk_space,timer, time_int,dt, time_sleep, cpu_t;
int date1,date2,h1,h2,m1,m2,cpu,cpu_lim,cpu_cont,conn,cpu_b,cpu_tb,numb; 
time_t stat_loc[N_comp][N_comp+1];
struct stat stat_buf; 
FILE *fp;
const char delimiter[]=" %";
const char delim[]=" :";
const char delim_[]=" -";
char *token, *user_name, *cti;
pid_t pid;

  time_int=360; /* minutes */
  time_sleep=120; /* sec */
  cpu_lim=50;
  N_lim_log_length=100000000;
  user_name="ams";


  fp=fopen("/afs/cern.ch/user/a/ams/.xprocess","r");
  if (fp==NULL) {
    fclose(fp);
     puts("read_status: cannot open status file\n");      
    exit(1);
  }
  for (i=0; i<N_comp; i++) {
    ch1=fgets(ch,256,fp);
    if (feof(fp)){
     fclose(fp);
      puts("read_status: unexpected oef\n");      
     exit(1);
     }
    token=strtok(ch,delim);
    sprintf(host_name[i],token);
  }



  sprintf(names[0][0],"gbatch.");
  sprintf(names[0][1],"gbatch.");
  sprintf(names[0][2],"gbatch.");
  sprintf(names[0][3],"gbatch.");
  sprintf(names[0][4],"gbatch.");
  sprintf(names[0][5],"gbatch.");
  sprintf(names[0][6],"gbatch.");
  sprintf(names[0][7],"gbatch.");

  pid=getpid();
  sprintf(a_name,"tmp.%d",pid);
  sprintf(temp_name,"temp.%d",pid);

  /* --- Form Initialising -----------*/
  for (i=0; i<N_comp*2; i++) {
    i_log[i]=0;
  }
  fl_initialize(&argc,argv,"Offline Processes",0,0);
  Xf_init();
  fl_show_form(form,FL_PLACE_FREE,FL_FULLBORDER,"Offline Processes");

  timer=0;
  while ((but=fl_check_only_forms()) != quit ) {
    for (i=0; i<N_comp*2; i++) {
      if (but == stop_h[i]) {
        if (i_log[i]) fl_hide_form(log_f[i]);
      }
    }

    for (i=0; i<N_comp; i++) {
      if (but == b_open_log[i])
        open_log(i, 1); /* open mode */
      if (but == b_open_elog[i])
        open_elog(i);
    }
    if (fmod(timer,time_sleep)==0 ) {

      timer=0;
      ti=time(NULL);
      cti=ctime(&ti);
      fl_set_object_label(heade,cti);
    for (j=0; j<N_comp+1; j++) {


      if (j<N_comp) { /*  !!!! <(N_comp)
                          
      /* ------------ read statistics */
        er = read_status();
        for (i=0; i<6; i++) 
          stat_loc[i][j]=0;
        er = stat(a_name,&stat_buf);
        if (er>=0) {
          sprintf(ch,"rm %s",a_name);
          system(ch);
        }
        sprintf(chbuf,"/usr/sue/bin/rsh %s cat /dat0/local/logs/run_prod.log > %s",host_name[j],a_name);
        system(chbuf);
        er = stat(a_name,&stat_buf);
        if ((er<0)||(stat_buf.st_size<10)) {
          puts("cannot open /dat0/local/logs/run_prod.log file");
          statu[j]=1;
          fl_set_object_label(stati_[j],"------");
          for (i=0; i<6; i++) {
            stat_loc[i][j]=0;
          }
          goto END_stat;
        }
        fp=fopen(a_name,"r");
        while (!feof(fp)) {
          ch1=fgets(ch,256,fp);
          if (ch1!=NULL) {
            if (isdigit(ch1[2])>0) {
              token = strtok(ch,delimiter);
              for (i=0; i<4; i++) {
                token=strtok(NULL,delimiter);
                er=atoi(token);
              stat_loc[i][j] += er;
              }
            }
          }
        }
        stat_loc[4][j]=(stat_loc[1][j]+0.)/stat_loc[2][j];
        stat_loc[5][j]=stat_loc[1][j]/stat_loc[3][j];
        sprintf(chbuf,"%7d / %7d  %6d  %6d  %4d /%4d",stat_loc[0][j],stat_loc[1][j],
                stat_loc[2][j],stat_loc[3][j],stat_loc[4][j],stat_loc[5][j]);
        fl_set_object_label(stati_[j],chbuf);
        
        fclose(fp);
        sprintf(ch,"rm %s",a_name);
        system(ch);
      END_stat:
       lll=0;

      }


      if (j==5) { /*(j<N_comp) {*/
        /*--------------- check logfile length -------- */
        er=open_log(j, 0);
        if (er<0) 
          fl_set_object_label(log_check[j],"! CHECK LOG !");
        else
          fl_set_object_label(log_check[j],"");
      }


      if (j<N_comp) {  /* !!!! <N_comp */
      /*----------- check for gbatch ----------- */
      cpu_cont=2;
      cpu=0;
      cpu_t=0;
    CPU:
      count[0]=0;   
      if (strlen(names[0][j])>0) {
        if (j<N_comp-3) 
          sprintf(chbuf,"/usr/sue/bin/rsh %s ps -fe | grep %s > %s",host_name[j],names[0][j],a_name);
        else
          sprintf(chbuf,"/usr/sue/bin/rsh %s ps xuw | grep %s > %s",host_name[j],names[0][j],a_name);
	system(chbuf);
	er = stat(a_name,&stat_buf);
	if (stat_buf.st_size>0) {
	  fp=fopen(a_name,"r");
	  if (fp==NULL) {
	    puts("cannot open temp file tmp");
	    count[0]=-1;
	    goto END_gb; 
	  }
	REP:
	  ch1=fgets(ch,8,fp);
	  if (ch1 != NULL) {
	    token = strtok(ch1,delimiter);
	    if (strcmp(token,user_name)==0) {
              if (j<N_comp-2) {
                ch1=fgets(ch,15,fp);
                ch1=fgets(chbuf1,5,fp);
                cpu_b=atoi(chbuf1);
                ch1=fgets(ch,17,fp);
                ch1=fgets(ch,15,fp);
              }
              else {
                ch1=fgets(ch,9,fp);
                ch1=fgets(chbuf1,5,fp);
                cpu_b=atoi(chbuf1);
                ch1=fgets(ch,33,fp);
                ch1=fgets(ch,9,fp);
              }
	      strcpy(chbuf,ch1);
              sprintf(chbuf2,"%s cpu %s",chbuf1,chbuf);
              cpu_tb=atoi(chbuf);
              token=strtok(ch1,delim_);
              token=strtok(NULL,delim_);
              if (token!=NULL) {
                cpu_t *=24;
              }
              if (j>(N_comp-3)) {
                cpu_t=cpu_t/60;
              }
	      ch1=fgets(ch,256,fp);
	      /*token=strtok(ch1,delimiter);*/
	      if ((strncmp(ch1,"grep",4)!=0) && (strncmp(ch1,"sh -c",5)!=0)&&
                  (strncmp(ch1," grep",5)!=0) && (strncmp(ch1," sh -c",6))) {
		count[0]=1;
                if (j==1)
                  i=j;
                sprintf(chb_t," %s",chbuf);
                cpu+=cpu_b;
                cpu_t+=cpu_tb;
		goto REP;
	      }
	      goto REP;
	    }
	  }
	CONT: 
	  fclose(fp);
          sprintf(ch,"rm %s",a_name);
	  system(ch);
          sprintf(chbuf2,"%d cpu %s",cpu,chb_t);
        }  
      END_gb:
        if ((count[0]>0)&&(cpu<cpu_lim)) {
          if (cpu_cont) {
            cpu_cont--;
            goto CPU;
          }
          count[0]=2;
        }
        sprintf(chbuf2,"%d cpu %s",cpu,chb_t);
        switch (count[0]) {
	case 2:
          fl_set_object_color(check_[0][j],FL_YELLOW,FL_YELLOW);
	  fl_set_object_label(numb_[0][j],chbuf2);
          if (statu[j]!=0) 
            er=write_status();
          break;
        case 1:
	  fl_set_object_color(check_[0][j],FL_GREEN,FL_GREEN);
	  fl_set_object_label(numb_[0][j],chbuf2);
          if (statu[j]!=0) 
            er=write_status();
          break;
        case 0:
          if (statu[j]==0)
            fl_set_object_color(check_[0][j],FL_RED,FL_RED);
          else
            fl_set_object_color(check_[0][j],FL_BLUE,FL_BLUE);
          fl_set_object_label(numb_[0][j],"0");
          break;
        case -1:
          fl_set_object_color(check_[0][j],FL_BLACK,FL_BLACK);
          fl_set_object_label(numb_[0][j],"error");
          break;
        }
      
      }
      }

      if (j<N_comp+1) {
      /* ---- check for disk space -------- */
      count[1]=-1;
      if (j<N_comp) 
        sprintf(chbuf,"/usr/sue/bin/rsh %s df /dat0/local/amsdatantuple/ > %s",host_name[j],a_name);
      else 
        sprintf(chbuf,"df /Offline/ > %s",a_name);
      system(chbuf);
      er = stat(a_name,&stat_buf);
      if (stat_buf.st_size>0) {
	fp=fopen(a_name,"r");
	if (fp==NULL) {
	  puts("cannot open temp file tmp");
	  goto END_disk; 
	}    
	count[1]=0;
	ch1=fgets(ch,256,fp);
	ch1=fgets(ch,256,fp);
	fclose(fp);
        sprintf(chbuf,"rm %s",a_name);
        system(chbuf);
	token=strtok(ch1,delimiter);
	for (i=1; i<5; i++) 
	  token=strtok(NULL, delimiter);
	disk_space=atoi(token);
	if (disk_space>=90) {
	  if (disk_space>=95)
	    count[1]=2;
	  else 
	    count[1]=1;
	}
	
      }

    END_disk:
      switch (count[1]) {
      case -1: 
	fl_set_object_color(check_[1][j],FL_BLACK,FL_BLACK);
	fl_set_object_label(numb_[1][j],"error");
	break;
      case 0:
	fl_set_object_color(check_[1][j],FL_GREEN,FL_GREEN);
	sprintf(ch1,"%d percent",disk_space);
	fl_set_object_label(numb_[1][j],ch1);
	break;
      case 1:
	fl_set_object_color(check_[1][j],FL_YELLOW,FL_YELLOW);
	sprintf(ch1,"%d percent",disk_space);
	fl_set_object_label(numb_[1][j],ch1);
	break;
      case 2:
	fl_set_object_color(check_[1][j],FL_RED,FL_RED);
	sprintf(ch1,"%d percent",disk_space);
	fl_set_object_label(numb_[1][j],ch1);
	break;
      }

      }

      if (j<N_comp) {
      /*------ check last ntuple ----- */
      count[2]=-1;
      er = stat(a_name,&stat_buf);
      if (er>=0) {
        sprintf(ch,"rm %s",a_name);
        system(ch);
      }
      sprintf(chbuf,"/usr/sue/bin/rsh %s ls -lts /dat0/local/amsdatantuple/ > %s",host_name[j],a_name);
      system(chbuf);
      er = stat(a_name,&stat_buf);
      if (stat_buf.st_size>10) {
	fp=fopen(a_name,"r");
	if (fp==NULL) {
	  puts("cannot open tmp file");
	  goto END_nt; 
	}    
        
        count[2]=-2;
        nn=0;
        ch1=fgets(chh[nn],256,fp);
       	if (ch1==NULL) 
	   goto END_nt;
        if (strncmp(chh[nn],"total",5)==0) {
          token=strtok(chh[nn],delimiter);
          token=strtok(NULL,delimiter);
          er=atoi(token);
          if (er<=0) 
            goto END_nt;
        }
        else {
          nn++;
        }
        conn=1;
        while ((!feof(fp))&&(conn)) {
	  ch1=fgets(chh[nn],256,fp);
          if (nn>25)conn=0;
          if (ch1!=NULL) { 
            ch2=strrchr(chh[nn],'.');
            if (ch2!=NULL) {
              if (strcmp(ch2,".hbk\n")==0)nn++;
            }
            else if(nn>0)nn--;
          }
	}
        fclose(fp);
        if (nn<=0) {
          count[2]=2;
          goto END_nt;
        }
        numb=0;
      LNC:
	if (nn>numb+1) {
          sprintf(ch256,"%s",chh[numb+1]);
	  token=strtok(ch256,delim);
	  for (i=1; i<8; i++) {
	    token=strtok(NULL,delim);
	  }
	  sprintf(chbuf,"%s",token);
	  date1=atoi(chbuf);
	  token=strtok(NULL,delim);
	  sprintf(chbuf,"%s",token);
	  h1=atoi(chbuf);
	  token=strtok(NULL,delim);
	  sprintf(chbuf,"%s",token);
	  m1=atoi(chbuf);
	  token=strtok(NULL,delim);
          count[2]=0;
	}
        sprintf(ch256,"%s",chh[numb]);
	token=strtok(ch256,delim);
	for (i=1; i<8; i++) {
	  token=strtok(NULL,delim);
	}
	sprintf(chbuf,"%s",token);
	date2=atoi(chbuf);
	token=strtok(NULL,delim);
	sprintf(chbuf,"%s",token);
	h2=atoi(chbuf);
	token=strtok(NULL,delim);
	sprintf(chbuf,"%s",token);
	m2=atoi(chbuf);
	token=strtok(NULL,delim);
	
	if (nn<numb+2) 
	  dt=0;
	else {
	  if (date2 != date1)
	    h2 += 24;
	  dt=(h2-h1)*60+(m2-m1);
          if (dt<=3) {
            numb++;
            goto LNC;
          }
	  if (dt>time_int) {
            if (cpu_t>time_int)
              count[2]=1;
            else {
              dt=cpu_t*60.;
              count[2]=0;
            }
          }
	}
      END_nt:

        if (statu[j]!=0)
          count[2]=2;

	switch (count[2]) {
        case -2:
          sprintf(chbuf,"%d h",cpu_t);
          fl_set_object_color(check_[2][j],FL_YELLOW,FL_YELLOW);
          fl_set_object_label(numb_[2][j],chbuf);
          fl_set_object_label(spare[j],token);
          break;
        case -1: 
          fl_set_object_color(check_[2][j],FL_BLACK,FL_BLACK);
          fl_set_object_label(numb_[2][j],"error");
          break;
        case 0:
          fl_set_object_color(check_[2][j],FL_GREEN,FL_GREEN);
          sprintf(chbuf,"%5.1f h",(dt/60.));
          fl_set_object_label(numb_[2][j],chbuf);
          fl_set_object_label(spare[j],token);
          break;
        case 1:
          fl_set_object_color(check_[2][j],FL_RED,FL_RED);
          sprintf(chbuf,"%5.1f h",(dt/60.));
          fl_set_object_label(numb_[2][j],chbuf);
          fl_set_object_label(spare[j],token);
          break;
        case 2:
          fl_set_object_color(check_[2][j],FL_GREEN,FL_GREEN);
          sprintf(chbuf,"----",(dt/60.));
          fl_set_object_label(numb_[2][j],chbuf);
          fl_set_object_label(spare[j],"-----");
          break;
        }
      }
      }

           
    } /* end of host */
    /*------- statistic summary ------ */
    for (i=0; i<6; i++) {
      stat_loc[i][N_comp]=0;
      for (j=0; j<N_comp; j++) /* !!!!!!!!!!!!!! <N_comp */
        stat_loc[i][N_comp] += stat_loc[i][j];
    }
    sprintf(chbuf,"%7d / %7d  %6d  %6d  %4d /%4d",stat_loc[0][N_comp],
            stat_loc[1][N_comp],stat_loc[2][N_comp],stat_loc[3][7],
            stat_loc[4][N_comp],stat_loc[5][N_comp]);
        fl_set_object_label(stati_[N_comp],chbuf);

    }
    sleep(1);
    timer++;
  }
  fl_hide_form(form);
  fl_finish();
  return (1);
}

int open_log(int j, int mode) {
int er, ii,CO;
char *ch1, ch[256], chb[256], chb1[256], *token, chbuf[80],cho[256],cha[256];
struct stat stat_buf;
FILE *fp;
char delime[]=" .";
char del[]=" \n";

  sprintf(ch,"/usr/sue/bin/rsh %s ls -lts /dat0/local/logs/ > %s",host_name[j],temp_name);
  system(ch);
  er = stat(temp_name,&stat_buf);
  if (stat_buf.st_size<9) {
    puts("\athere is not log file");
    return -1;
  }
  fp=fopen(temp_name,"r");
  if (fp==NULL) {
    puts("\acannot open log.file");
    fclose(fp);
    sprintf(ch,"rm %s",temp_name);
    system(ch);
    return -1; 
  }   
  ch1=fgets(ch,256,fp);
CONT:
  if (feof(fp)) {
    puts("\athere is not log file");
    fclose(fp);
    sprintf(chbuf,"rm %s",temp_name);
    system(chbuf);
    return -1;
  }
  ch1=fgets(ch,256,fp);
  token=strtok(ch1,delime);
  for (ii=1; ii<10; ii++) 
     token=strtok(NULL,delime);
  sprintf(chb,"%s",token);
  /*  token=strtok(NULL,delime);*/
  er=isdigit(chb[0]);
  if (er==0)
    goto CONT;
  CO=0;
CO2:
  token=strtok(NULL,delime);
  if (token==NULL)
    goto CONT;
  if (CO==0)
    strcpy(ch,token);
  strcpy(cho,token);
  if (strcmp(cho,"log\n")!=0) {
    sprintf(cha,"%s",token);
    er=islower(cha[0]);
    if ((er==0)||(cha[0]=='e'))
      goto CONT;
    CO=1;
    goto CO2;
  }  
  fclose(fp);
  /*  token=strtok(chb,del);*/
  if (CO==0)
    sprintf(ch,"%s.log",chb);
  else
    sprintf(ch,"%s.%s.log",chb,cha);
  if (mode!=1) { /* check mode */
    sprintf(chb1,"/dat0/local/logs/%s",ch);
    er = stat(chb1,&stat_buf);
    /*    if (er!=0) {
      printf("%s: cannot open logfile\n",host_name[j]);
      return -1;
    }*/
    if (stat_buf.st_size>N_lim_log_length) {
       return -1;
    }
    return 0;
  }
  sprintf(ch,"/usr/sue/bin/rsh %s cat /dat0/local/logs/%s > %s",host_name[j],ch,temp_name);
  er=system(ch);
  if (er<0){
    puts("\acannot open log file");
    return -1;
  }
  ind_l++;
  if (ind_l>13) {
    puts("\a too many logs open\n");
    return -1;
  }
  log_(j,token);
  sprintf(ch,"rm %s",temp_name);
  system(ch);
  
  return 0;
}

int open_elog(int j) {
int er,ii,CO;
char *ch1, ch[256], chb[256], *token, chbuf[80], cho[256],cha[256];
struct stat stat_buf;
FILE *fp;
char delime[]=" .";
char del[]=" \n";

  sprintf(ch,"/usr/sue/bin/rsh %s ls -lts /dat0/local/logs/ > %s",host_name[j],temp_name);
  system(ch);
  er = stat(temp_name,&stat_buf);
  if (stat_buf.st_size<9) {
    puts("\athere is not log file");
    return -1;
  }
  fp=fopen(temp_name,"r");
  if (fp==NULL) {
    puts("\acannot open log.file");
    fclose(fp);
    sprintf(ch,"rm %s",temp_name);
    system(ch);
    return -1; 
  }   
  ch1=fgets(ch,256,fp);
CONT:
  if (feof(fp)) {
    puts("\athere is not log file");
    fclose(fp);
    sprintf(chbuf,"rm %s",temp_name);
    system(chbuf);
    return -1;
  }
  ch1=fgets(ch,256,fp);
  token=strtok(ch1,delime);
  for (ii=1; ii<10; ii++) 
     token=strtok(NULL,delime);
  sprintf(chb,"%s",token);
  /*  token=strtok(NULL,delime);*/
  er=isdigit(chb[0]);
  if (er==0)  
    goto CONT;
  CO=0;
CO2:
  token=strtok(NULL,delime);
  if (token==NULL)
    goto CONT;
  if (CO==0)
    strcpy(ch,token);
  strcpy(cho,token);
  if (strcmp(cho,"e")!=0) {
    sprintf(cha,"%s",token);
    er=islower(cha[0]);
    if ((er==0)||(cha[0]=='e'))
      goto CONT;
    CO=1;
    goto CO2;
  }
  token=strtok(NULL,delime);
  strcpy(ch,token);
  if (strcmp(ch,"log\n")!=0)
    goto CONT;   
  fclose(fp);
  /*  token=strtok(chb,del);*/
  if (CO==0)
    sprintf(ch,"%s.e.log",chb);
  else 
    sprintf(ch,"%s.%s.e.log",chb,cha);
  sprintf(ch,"/usr/sue/bin/rsh %s cat /dat0/local/logs/%s > %s",host_name[j],ch,temp_name);
  er=system(ch);
  if (er<0){
    puts("\acannot open log file");
    return -1;
  }
  ind_l++;
  if (ind_l>13) {
    puts("\a too many logs open\n");
    return -1;
  }
  log_(j,token);
  sprintf(ch,"rm %s",temp_name);
  system(ch);

  return 0;
}

/*----------------*/
int read_status() {
  FILE *fp;
  const char delim[]=" ";
  char ch[256], *ch1, *token;
  int i,j,er,comp;
  struct stat stat_buf; 

  for (i=0; i<N_comp; i++) 
   statu[i]=0;

  er = stat("/afs/cern.ch/user/a/ams/.xprocess",&stat_buf);
  if ((er<0)||(stat_buf.st_size<9)) {
    goto E0;
  }
  fp=fopen("/afs/cern.ch/user/a/ams/.xprocess","r");
  if (fp==NULL) {
    fclose(fp);
    goto E0;
  }
  for (i=0; i<N_comp; i++) {
    ch1=fgets(ch,256,fp);
    if (feof(fp))
      goto E1;
    token=strtok(ch,delim);
    comp=0;
    for (j=0; j<N_comp; j++) {
      if (strcmp(token,host_name[j])==0) {
        comp=1;
        token=strtok(NULL,delim);
        statu[j]=atoi(token);
        j=N_comp;
      }      
    }
    if (!comp) {
      puts("read_status: wrong host name\n");
    }
  }

E1:
  fclose(fp);
  return 1;

E0:
  puts("read_status: cannot open status file\n");
  return 0;
}

int write_status() {

  FILE *fp;
  int i;

  fp=fopen("/afs/cern.ch/user/a/ams/.xprocess","r");  
  if (fp==NULL) {
    puts("\awrite_status: cannot write status file\n");
    return 0;
  }
  for (i=0; i<N_comp; i++) {
    fprintf(fp,"%s  %d\n",host_name[i],statu[i]);
  }
  fclose(fp);
  return 1;
}
