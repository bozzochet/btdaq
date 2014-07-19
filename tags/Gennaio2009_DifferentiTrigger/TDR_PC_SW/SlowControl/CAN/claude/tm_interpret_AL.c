// file tm_interpret_AL.c
//
// Stolen from Claude and reworked
//
// A.Lebedev - Sep 2008...
//

#include "gpsdef.h"
#include "gpsdef_czu.h"

#define MAX_OUT 8192
#define MAX_IN 8192

char search_key_tm[10];
char search_key_hx[4];
char buff_in[MAX_IN];
char buff_out[MAX_OUT];
char buff_outf[MAX_OUT];
unsigned char buff_outi[MAX_OUT];
char input_file[40]; 
char output_file[40];

int local_time = 0;
int first_value = 0;
int number_tm = 0;
int error_checksum = 0;
int a, b, c, d, e, f, g, h, i, j, k, u, v, w, x, y, z;
int start_value = 0;
int end_value = 0;
int lenght_value = 0;
int lenght_final_value = 0;
int first_subparam = 0;
int nbr_subparam = 0;
int nbr_position = 0;
int num_param;
int32 integer_param;
int32 binary_param = 0;
int32 myCheck;
int32 tmCheck;
int test_char = 0;
int test_end = 0;
int test_check = 0;
int test_tm = 0;
int test_value = 0;

char ident1_tm[11];
char string1_tm[12] = "9C 3E ";
char string2_tm[9] = "9C3E";
char end_tm[23] = "55 55 55 55 55 55 55 55";
char temp_end_tm[23];
char reserved[9] = "Reserved\0";
char param_format;
unsigned char value_param[21];
unsigned char value_def[16];
unsigned char output_byte = 0x0;

unsigned char ascii_tab[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
unsigned char hexa_tab[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};

FILE *f_in, *f_out;

//~============================================================================

void prep_extraction() {
     strcpy(search_key_hx, TM_PARAMETERS[i].hex_idtm);
     string1_tm[6]  = search_key_hx[0];
     string1_tm[7]  = search_key_hx[1];
     strcat(string1_tm, " ");
     string1_tm[9]  = search_key_hx[2];
     string1_tm[10] = search_key_hx[3];

     y=4;
     for (x=0; x<4; x++) {
     string2_tm[y]  = search_key_hx[x];
     y++;
     }
}

//~----------------------------------------------------------------------------

void conversion() {
test_char = 0;
      for (y=0; y<16; y++) {
      if (ascii_tab[y] == buff_outf[j])
           output_byte = hexa_tab[y];
      }
}

//~----------------------------------------------------------------------------

void  checksum_test()
{
int e = 0;
int f = 0;

       tmCheck = 0;
       test_check = 0;

       for (e=0; e<16; f=f+4) {
           b=f;
           integer_param = 0;
           for (a=0; a<4; a++) {
               integer_param=integer_param << 8;
               integer_param=integer_param+buff_outi[b];
               b++;
               }           
           tmCheck = tmCheck + integer_param;
           e++;
       }
       if (tmCheck == myCheck) {
           test_check = 1;  
       }
}

//~----------------------------------------------------------------------------

void   compact() {
       j = 16; 
       k = 0;
       sscanf(&buff_outf[8], "%8X", &myCheck);    
       lenght_value = 144;
       while (j < lenght_value) {
            conversion();
            buff_outi[k] = output_byte;
            buff_outi[k] = buff_outi[k] * 16;
            j++;
            conversion();
            buff_outi[k] = buff_outi[k] + output_byte;
            k++;
            j++;
            }
       value_def[k] = '\0';
       }

//~----------------------------------------------------------------------------

void   test_end_tm() {
       test_end = 0;
	     b = i;

	     for (a=0; a <22; a++){
          temp_end_tm[a] = buff_in[b];
		      b++;
          }
	     temp_end_tm[a] = '\0';
       if (strcmp(temp_end_tm, end_tm) == 0){
		      test_end = 1;
	        }
       }

//~----------------------------------------------------------------------------

void bits_position() {
        d=0;
        if (start_value > 9) d++;
        if (end_value > 9) d++;
        if ((start_value == end_value) && (start_value > 9)) printf("%d   ",start_value);
        if ((start_value == end_value) && (start_value < 10)) printf("%d  ",start_value);
        if (start_value != end_value) printf("%d-%d", start_value, end_value);
}

//~----------------------------------------------------------------------------

void    binary_extraction() {
        binary_param=integer_param;
			  binary_param = binary_param << (31 - end_value);
			  binary_param = binary_param >> (31 - (end_value-start_value));
}

//~----------------------------------------------------------------------------

void    print_value() {  
        switch (TM[u].par[z].fmt[1]) {
        case 'u':
            printf("%u d\n", integer_param);
            break;
        case 'd':
            printf("%d d\n", integer_param);
            break;  
        default:
            printf("%0X h\n", integer_param);
       }
}

//~----------------------------------------------------------------------------

void simple_extraction() {
        first_value = 0;
        i=0;
        while ((TM_PARAMETERS[v].parameters[z].tm_set[i].nam_value) != '\0') {
		    start_value = TM_PARAMETERS[v].parameters[z].tm_set[i].param_bgn;
		    end_value = TM_PARAMETERS[v].parameters[z].tm_set[i].param_end;

        if (first_value==1) {
            printf("\n");
            for (d=0; d<38; d++) printf(" ");
		    }
        else {(first_value=1);};
        bits_position();
        for (0; d<3; d++) printf(" ");
		    printf("%s", TM_PARAMETERS[v].parameters[z].tm_set[i].nam_value);

        lenght_value = 26-strlen(TM_PARAMETERS[v].parameters[z].tm_set[i].nam_value);
        for (d=0; d<lenght_value; d++) printf(" ");
        binary_extraction();
        printf("%X h", binary_param);

        i++;
        }
        printf("\n");
}

//~----------------------------------------------------------------------------

void selected_extraction() {
        first_value = 0;
        i=0;
        while ((TM_PARAMETERS[v].parameters[z].tm_set[i].nam_value) != '\0') {
		    start_value = TM_PARAMETERS[v].parameters[z].tm_set[i].param_bgn;
		    end_value = TM_PARAMETERS[v].parameters[z].tm_set[i].param_end;

        if (first_value==1) {
            printf("\n");
            for (d=0; d<38; d++) printf(" ");
		    }
        else {(first_value=1);};
        bits_position();
        for (0; d<3; d++) printf(" ");

        binary_extraction();
        j=i;
        if (strcmp(TM_PARAMETERS[v].parameters[z].tm_set[j].nam_value, reserved) != 0) {

            while ((TM_PARAMETERS[v].parameters[z].tm_set[j].nam_value) != '\0' &&  (TM_PARAMETERS[v].parameters[z].tm_set[j].param_bgn) == start_value) {
                if (TM_PARAMETERS[v].parameters[z].tm_set[j].param_value == binary_param){
            		    printf("%s", TM_PARAMETERS[v].parameters[z].tm_set[j].nam_value);
                    lenght_value = 26-strlen(TM_PARAMETERS[v].parameters[z].tm_set[j].nam_value);
                    for (d=0; d<lenght_value; d++) printf(" ");
                    printf("%X h", binary_param);
                }
            j++;
        }} 
        else {printf("%s", reserved); j++;};
        i=j;
        }
        printf("\n");
}

//~----------------------------------------------------------------------------

void display_tm() {

  d=0;
  i=0;
  c=0;

  printf("SRECEIVST - Local Time : %d \n\n",local_time);
  printf(" Byte | Description                 | Bit - Value \n");
  for (i=0; i<65; i++) printf("-");
  printf("\n");

  z = 0;
  while (TM[u].par[z].nam != '\0') { 
    start_value = TM[u].par[z].bgn;  
    end_value   = TM[u].par[z].end;        
    printf(" %d-%d", start_value, end_value);
    if (start_value < 10) printf(" ");
    if (end_value < 10)   printf(" ");  
    printf("  ");

    printf("%s", TM[u].par[z].nam);
    i = strlen(TM[u].par[z].nam);
    for (i=i+1; i<31; i++) printf(" ");

    a = TM[u].par[z].bgn;         
    b = TM[u].par[z].end;
    lenght_value = ((b - a) + 1);

    for (e=0; e<lenght_value; e++) {
      value_param[e] = buff_outi[a];
      a++;
    }

    j = 0; 
    integer_param = 0;
    for (j=0; j<lenght_value; j++) {
      integer_param = integer_param << 8;
      integer_param = integer_param + value_param[j];
    }

    switch (TM_PARAMETERS[v].parameters[z].treatement) {
      case 1:  simple_extraction();   break;
      case 2:  selected_extraction(); break;
      default: print_value();         break;
    }

    z++;
  }
}

//~============================================================================

int main(int argc, char *argv[]) {

  setbuf(stdout, NULL);
  printf ("usage : %s file name & selected TM name\n", argv[0]);
  if (argc < 3) exit(1);
  strcpy(input_file, argv[1]);
  strcpy(search_key_tm, argv[2]);

  if ((f_in = fopen(input_file, "r")) == NULL) {
    fprintf(stderr, "\nError in reading file %s\n", argv[1]);
    return(1);
  } 

  u = v = 99;
  i = 0;
  while (TM[i].nam != '\0' && u==99) {
    if(strcmp(TM[i].nam, search_key_tm) == 0) u=i;
    i++;
  }

  if (u == 99) {
	  fprintf(stderr, "\nNot present in table of TM : %s\n", argv[2]);
    return(1);
  }

  i = 0;
  while (TM_PARAMETERS[i].nam_parameter != NULL && v==99) {
	  if(strcmp(TM_PARAMETERS[i].nam_parameter, search_key_tm) == 0) {
      v=i;
      prep_extraction();        
    }
    i++;
  }

  if (v == 99) {
    fprintf(stderr, "\nNot present in table of TM_PARAMETERS : %s\n", argv[2]);
    return(1);
  }

  while (!feof(f_in)) {
    fgets(buff_in, MAX_IN, f_in);
    if (buff_in[0] == '9') {
      for (i=0; i <11; i++) {
        ident1_tm[i] = buff_in[i];
      }
      ident1_tm[i] = '\0';
      if (strcmp(ident1_tm, string1_tm) == 0) {
        number_tm = 1;
        i = 0;
        while (buff_in[i] !='\n') {
          if (buff_in[i] == '5') {
	          test_end_tm();
	          if (test_end == 1) number_tm = 1;
	        }
          if (buff_in[i] !=' ') {
            buff_out[y] = buff_in[i];
            y++;
          }
          i++;
        }
        buff_out[y] = '\0';    
      } 
    }
    if (number_tm == 1) break;
  }   

  fclose(f_in);

  if (number_tm == 0) {
    fprintf(stderr, "\nNo TM %s found : %d\n\n", search_key_tm, number_tm);
    return(1);
  }

  i=0;
  y=0;
  w=0;
  x=8;

  for (y=8; y<152; y++) {
    buff_outf[i] = buff_out[y];
    i++;
  }
  buff_outf[i] = '\0';

  compact();
  printf("TM - Past compression : %s \n", search_key_tm);
  for (y=0; y<64; y++) printf("%02X",buff_outi[y]);
  printf("\n");

  checksum_test();
  if (test_check == 0) {
    error_checksum++;
    fprintf(stderr, "\nChecksum error ! Check in TM : %08X - Check calculated : %08X \n", myCheck, tmCheck);
    return(1);
  } 

  display_tm();

  printf("\n\nTM %s checksum error : %d\n", search_key_tm, error_checksum); 

  return (0);
}

//~============================================================================
