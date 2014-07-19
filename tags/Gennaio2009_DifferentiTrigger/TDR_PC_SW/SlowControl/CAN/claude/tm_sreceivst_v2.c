// file tm_sreceivst.c
// Decoding of TM SRECEIVST
// Claude Zurbach
// September 12th 2008
//
/**/
#include "gpsdef.h"
#include "gpsdef_cz.h"
/**/
#define MAX_OUT 8192
#define MAX_IN 8192
/**/
char search_key_tm[9] = "SRECEIVST";
char buff_in[MAX_IN];
char buff_out[MAX_OUT];
char buff_outf[MAX_OUT];
unsigned char buff_outi[MAX_OUT];
char input_file[40]; 
char output_file[40];
/**/
int local_time = 0;
int first_value = 0;
int number_tm = 0;
int error_checksum = 0;
int a, b, c, d, e, f, g, h, i, j, k, w, x, y, z;
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
/**/
char ident1_tm[11];
char string1_tm[12] = "9C 3E 26 06\0";
char string2_tm[9] = "9C3E2606\0";
char end_tm[23] = "55 55 55 55 55 55 55 55";
char temp_end_tm[23];
char reserved[9] = "Reserved\0";
char param_format;
unsigned char value_param[21];
unsigned char value_def[16];
unsigned char output_byte = 0x0;
/**/
unsigned char ascii_tab[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
unsigned char hexa_tab[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
/**/
FILE *f_in, *f_out;
/****/
/* conversion() */
/****/
void conversion() {
test_char = 0;
      for (y=0; y<16; y++) {
      if (ascii_tab[y] == buff_outf[j])
           output_byte = hexa_tab[y];
      }
}
/****/
/* checksum_test() */
/****/
void  checksum_test()
{
int e = 0;
int f = 8;
/**/
       tmCheck = 0;
       test_check = 0;
       sscanf(&buff_outf[0], "%8X", &myCheck);
/**/
       for (e=0; e<16; f=f+8) {
           sscanf(&buff_outf[f], "%8X", &integer_param);
           tmCheck = tmCheck + integer_param;
           e++;
       }
       if (tmCheck == myCheck) {
           test_check = 1;  
       }
}
/****/
/* compact() */
/****/
void   compact() {
       j = 8; 
       k = 0;
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
/**/
/* test_end_tm() */
/**/
void   test_end_tm() {
       test_end = 0;
	     b = i;
/**/
	     for (a=0; a <22; a++){
          temp_end_tm[a] = buff_in[b];
		      b++;
          }
	     temp_end_tm[a] = '\0';
       if (strcmp(temp_end_tm, end_tm) == 0){
		      test_end = 1;
	        }
       }
/**/
/* bits_position() */
/**/
void bits_position() {
        d=0;
        if (start_value > 9) d++;
        if (end_value > 9) d++;
        if ((start_value == end_value) && (start_value > 9)) printf("%d   ",start_value);
        if ((start_value == end_value) && (start_value < 10)) printf("%d  ",start_value);
        if (start_value != end_value) printf("%d-%d", start_value, end_value);
}
/**/
/* binary_extraction() */
/**/
void    binary_extraction() {
        binary_param=integer_param;
			  binary_param = binary_param << (31 - end_value);
			  binary_param = binary_param >> (31 - (end_value-start_value));
}
/**/
/* print_value() - simple print of value */
/**/
void    print_value() {  
        switch (TM[5].par[z].fmt[1]) {
        case 'u':
            printf("%u d\n", integer_param);
            break;
        case 'd':
            printf("%d d\n", integer_param);
            break;  
        default:
            printf("%X h\n", integer_param);
       }
}
/**/
/* simple_extraction() */
/**/
void simple_extraction() {
        first_value = 0;
        i=0;
        while ((PARAM_SRECEIV[z].param[i].nam_value) != NULL) {
		    start_value = PARAM_SRECEIV[z].param[i].param_bgn;
		    end_value = PARAM_SRECEIV[z].param[i].param_end;
/**/
        if (first_value==1) {
            printf("\n");
            for (d=0; d<38; d++) printf(" ");
		    }
        else {(first_value=1);};
        bits_position();
        for (0; d<3; d++) printf(" ");
		    printf("%s", PARAM_SRECEIV[z].param[i].nam_value);
/**/
        lenght_value = 26-strlen(PARAM_SRECEIV[z].param[i].nam_value);
        for (d=0; d<lenght_value; d++) printf(" ");
        binary_extraction();
        printf("%X h", binary_param);
/**/
        i++;
        }
        printf("\n");
}
/**/
/* selected_extraction() */
/**/
void selected_extraction() {
        first_value = 0;
        i=0;
        while ((PARAM_SRECEIV[z].param[i].nam_value) != NULL) {
		    start_value = PARAM_SRECEIV[z].param[i].param_bgn;
		    end_value = PARAM_SRECEIV[z].param[i].param_end;
/**/
        if (first_value==1) {
            printf("\n");
            for (d=0; d<38; d++) printf(" ");
		    }
        else {(first_value=1);};
        bits_position();
        for (0; d<3; d++) printf(" ");
/**/
        binary_extraction();
        j=i;
        if (strcmp(PARAM_SRECEIV[z].param[j].nam_value, reserved) != 0) {
/**/
            while ((PARAM_SRECEIV[z].param[j].nam_value) != NULL &&  (PARAM_SRECEIV[z].param[j].param_bgn) == start_value) {
                if (PARAM_SRECEIV[z].param[j].param_value == binary_param){
            		    printf("%s", PARAM_SRECEIV[z].param[j].nam_value);
                    lenght_value = 26-strlen(PARAM_SRECEIV[z].param[j].nam_value);
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
/****/
/* display_tm() */
/****/
void   display_tm() {
       d=0;
       i=0;
       c=0;
/**/
/* print head */
/**/
       printf("SRECEIVST - Local Time : %d \n\n",local_time);
       printf(" Byte | Description                 | Bit - Value \n");
       for (i=0; i<65; i++) printf("-");
       printf("\n");
/**/
/* loop print parameter information */
/**/
       for (z=0; z <25; z++) { 
           start_value=TM[5].par[z].bgn;  
           end_value=TM[5].par[z].end;        
           printf(" %d-%d", start_value, end_value);
           if (start_value < 10) printf(" ");
           if (end_value < 10) printf(" ");  
		       printf("  ");
		       printf("%s",TM[5].par[z].nam);
           i=strlen(TM[5].par[z].nam);
           for (i=i+1; i<31; i++) printf(" ");
/**/
           a=TM[5].par[z].bgn;         
           b=TM[5].par[z].end;
           lenght_value=((b-a)+1);
/**/           
           for (e=0; e < lenght_value; e++) {
               value_param[e] = buff_outi[a];
               a++;
               }
           j = 0; 
           integer_param = 0;
           for (j=0; j<lenght_value; j++){
			     integer_param = integer_param << 8;
               integer_param = integer_param + value_param[j];
               }
/**/
               switch (PARAM_SRECEIV[z].treatement) {
               case 1 :
                   simple_extraction();
                   break;
               case 2 :
                   selected_extraction();
                   break;
               default :
                   print_value();               
               }
           }
}
/**/
/* main */
/**/
int main(int argc, char *argv[]) 
{
     setbuf(stdout, NULL);
     printf ("usage : %s file name \n", argv[0]);
     if (argc < 2) exit(1);
     strcpy (input_file, argv[1]);
/**/
/* Opening of input file */
/**/
     if ((f_in = fopen(input_file,"r")) == NULL) {
          fprintf(stderr, "\nError in reading file %s\n", argv[1]);
          return(1);
     }   
/**/
/* Read input until SRECEIVST found */
/**/
          fgets(buff_in, MAX_IN, f_in);
          if (buff_in[0] == '9'){
          for (i=0; i <11; i++){
               ident1_tm[i] = buff_in[i];
               }
               ident1_tm[i] = '\0';
          if (strcmp(ident1_tm, string1_tm) == 0){
          		 number_tm++;
               i = 0;
               while (buff_in[i] !='\n'){
				           if (buff_in[i] == '5'){
						       test_end_tm();
						       if (test_end == 1){
						       }
					         }
                   if (buff_in[i] !=' '){
                       buff_out[y] = buff_in[i];
                       y++;
                   }
                   i++;
                   }
               }
          }
     buff_out[y] = '\0';    
     fclose(f_in);
/**/
     if (number_tm == 0) {
          fprintf(stderr, "\nNo TM RECEIVST found : %d\n\n", number_tm);
          return(1);
     }
/**/      
          i=0;
          y=0;
          w=0;
          x=8;
          number_tm = 0;
/**/      
          for (y=w+8; y < w+144; y++) {
          buff_outf[i] = buff_out[x];
          i++;
          x++;
          }
      buff_outf[i] = '\0';
      number_tm++;
/**/
      checksum_test();
      if (test_check == 0) {
      error_checksum++;  
      }
/**/
      compact();
      printf("TM SRECEIVST : \n");
	    for (y=0; y<64; y++)
      printf("%02X",buff_outi[y]);
      printf("\n\n");
/**/
      display_tm();
      
      printf("\n\nTM SRECEIVST checksum error : %d\n", error_checksum); 
/**/
      return (0);
}
