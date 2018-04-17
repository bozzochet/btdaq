#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <limits.h>
#include <time.h>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <bitset>
#include <signal.h>
#include <math.h>
 
using namespace std; 
 
//GESTIONE INPUT 

int rsth = 0; 
int rsts = 0; 
int rst = 0; 
int repeat = 0; 
int gd = 0; 
int hd = 0; 
bool force_gain = false;
bool alt_mode = false;
bool set_time = false;
bool set_reset = false;

void HandleInputPar(int argc, char **argv)  
{ 
   
  stringstream usage; 
  usage.clear();  
  usage << endl; 
  usage << "Usage:" << endl << endl; 
  usage << "-g\t\t\tset gain: force gain" <<endl;
  usage << "-l\t\t\tset mode: alternative mode" <<endl; 
  usage << "-a\t\t\tset T_rh: 1 -> 40 ns, 15 -> 300 ns (default 5 -> 80 ns)" <<endl;
  usage << "-b\t\t\tset T_rs: 1 -> 0 ns, 15 -> 500 ns (default 4 -> 50 ns)" <<endl;
  usage << "-c\t\t\tset T_set: 1 -> 0 ns, 15 -> 400 ns (default 15 -> 400 ns)" <<endl;
  usage << "-t\t\t\tset T_repeat: 1 -> 10 us, 15 -> 2 ms (default 3 -> 20 us)" <<endl;
  usage << "-d\t\t\tset T_gd: 1 -> 1 us, 15 -> 100 us (default 4 -> 4 us)" <<endl;
  usage << "-y\t\t\tset T_hd: 1 -> 100 ns, 7 -> 1500 ns (default 2 -> 200 ns)" <<endl;
  usage << "-h\t\t\thelp" << endl; 
   
  int c; 
   
  while ((c = getopt(argc, argv, "gla:b:c:t:d:y:h")) != -1){ 
    switch (c) { 
    case 'g':
      force_gain=true;
      break;
    case 'l':
      alt_mode=true;
      break;      
    case 'a':
      rsth=atoi(optarg);
      if(rsth<1 || rsth>15 ) { 
      	cout << "Warning: bad T_rh value: 1 -> 40 ns, 15 -> 300 ns" << endl;
	exit(1);
      } 
      set_reset = true;	 	   
      break;
    case 'b':
      rsts=atoi(optarg);
      if(rsts<1 || rsts>15 ) { 
      	cout << "Warning: bad T_rs value: 1 -> 40 ns, 15 -> 500 ns" << endl;
	exit(1);
      } 
      set_reset = true;	 	   
      break;
    case 'c':
      rst=atoi(optarg);
      if(rst<1 || rst>15 ) { 
      	cout << "Warning: bad T_set value: 1 -> 100 ns, 15 -> 1500 ns" << endl;
	exit(1);
      } 
      set_reset = true;	 	   
      break;
    case 't':
      repeat=atoi(optarg);
      if(repeat<1 || repeat>15 ) { 
      	cout << "Warning: bad T_repeat value: 1 -> 10 us, 15 -> 2 ms" << endl;
	exit(1);
      } 
      set_time = true;	 	   
      break;
    case 'd':
      gd=atoi(optarg);
      if(gd<1 || gd>15 ) { 
      	cout << "Warning: bad T_gd value: 1 -> 1 us, 15 -> 100 us" << endl;
	exit(1);
      } 
      set_time = true;	 	   
      break;
    case 'y':
      hd=atoi(optarg);
      if(hd<1 || hd>15 ) { 
      	cout << "Warning: bad T_hd value: 1 -> 100 ns, 7 -> 1500 ns" << endl;
	exit(1);
      } 
      set_time = true;	 	   
      break;
    case 'h': 
      cout << usage.str().c_str() << endl; 
      exit(0); 
      break; 
    default: 
      cout << usage.str().c_str() << endl; 
      exit(0); 
      break; 
    } 
  } 
   
} 
 
//MAIN
		    
int main(int argc, char **argv)
{
 
  HandleInputPar(argc, argv); 
  
  unsigned short int write_set_roc;
  
  cout << "         hex  - dec" << endl;  

  write_set_roc = 0x0000;
	
  cout << "Fase A : " << setfill('0') << setw(4) << uppercase << hex << write_set_roc << " - " << dec << write_set_roc << endl; 
 	
  if (set_reset){ 
    write_set_roc = 0x0000 + (rst<<8) + (rsts<<4) + rsth;

    cout << "Fase B : " << setfill('0') << setw(4)  << uppercase << hex << write_set_roc << " - " << dec << write_set_roc << endl; 
  }
	
  if (set_time){ 
    write_set_roc = 0x1000 + (hd<<8) + (gd<<4) + repeat;

    cout << "Fase C : " << setfill('0') << setw(4) << uppercase << hex << write_set_roc << " - " << dec << write_set_roc << endl; 	 
  }
	
  if(force_gain) 
    if(alt_mode)
      write_set_roc = 0xB000; //avvia l'acquisizione forzando il guadagno in modalità alternativa
    else
      write_set_roc = 0xA000; //avvia l'acquisizione forzando il guadagno
  else   
    if(alt_mode)
      write_set_roc = 0x9000; //avvia l'acquisizione in modalità alternativa
    else
      write_set_roc = 0x8000; //avvia l'acquisizione
	  
  cout << "Fase D : " << setfill('0') << setw(4) << uppercase << hex << write_set_roc << " - " << dec << write_set_roc << endl; 	 

  return 0;
}
      

