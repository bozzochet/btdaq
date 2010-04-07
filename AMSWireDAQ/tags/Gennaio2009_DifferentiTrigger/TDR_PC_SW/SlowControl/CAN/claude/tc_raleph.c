// file tc_raleph.c
// Extract TM SALEPH and generate TC RALEPH
// Claude Zurbach - Alexei Lebedev
// August 19th 2008
//
#include "gpsdef.h"
/**/
#define MAX_OUT 8192
#define MAX_IN 8192
/**/
char buff_in[MAX_IN];
char buff_out[MAX_OUT];
char buff_outf[MAX_OUT];
char input_file[40]; 
char output_file[40];
/**/
int number_saleph = 0;
int number_raleph = 0;
int error_checksum = 0;
int a, b, c, i, j, k, w, x, y, z;
int start_value = 0;
int lenght_value = 0;
int lenght_final_value = 0;
int32 integer_param;
int32 myCheck;
int32 tmCheck;
int test_char = 0;
int test_end = 0;
int test_week = 0;
int test_check = 0;
int8 test_health = 0;
int test_prn = 0;
int test_saleph = 0;
int duplicate_prn = 0;
int error_week = 0;
/**/
char ident1_saleph[11];
char string1_saleph[12] = "9C 3E 23 9A\0";
char string2_saleph[9] = "9C3E239A\0";
char end_saleph[24] = "55 55 55 55 55 55 55 55\0";
char temp_end_saleph[24];
int  week__number;
char week___number[5];
unsigned char value_param[21];
unsigned char value_def[16];
unsigned char output_byte = 0x0;
/**/
unsigned char ascii_tab[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
unsigned char hexa_tab[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
unsigned int  prn_tab[64];
/**/
FILE *f_in, *f_out;
/**/
/* search_saleph() */
/**/
void conversion()
{
test_char = 0;
      for (y=0; y<16; y++) {
      if (ascii_tab[y] == value_param[j])
           {
           output_byte = hexa_tab[y];
           test_char = 1;
           }
      }
}
/**/
/* checksum_test() */
/**/
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
       else	{
       error_checksum++;
       }
}
/**/
/* first_tests() */
/**/
void  first_tests()
{
      test_saleph = 0;
      test_health = 0;
/**/
/* extract health status of TM */
/**/
       sscanf(&buff_outf[14], "%2X", &test_health);
       test_health = test_health << 2;
       test_health = test_health >> 2;
/**/
/* test if week number equal to week number reference */
/**/          
       sscanf(&buff_outf[20], "%04X", &test_week);
       if (test_week == week__number || test_health != 0) {
/**/
/* test if PRN already exists */
/**/
       sscanf(&buff_outf[12], "%2X", &test_prn);
       test_prn = test_prn - 64;
       if (prn_tab[test_prn] == 0) {
       prn_tab[test_prn] = 1;
       test_saleph = 1;
       }
       else duplicate_prn++;
       }
       else error_week++;
}
/**/
/* to transform "extended hexa" in standard hexa */
/**/
void   decode_value() {  
       j = 0; 
       k = 0;
       integer_param = 0;
       while (j < lenght_value) {
            conversion();
            value_def[k] = output_byte;
            value_def[k] = value_def[k] * 16;
            j++;
            conversion();
            value_def[k] = value_def[k] + output_byte;
            integer_param = (integer_param * 256)+ value_def[k];
            k++;
            j++;
            }
       value_def[k] = '\0';
       lenght_final_value = k;
       }
/**/
/* value_saleph() - extraction of value */
/**/
void   print_format() {  
       switch (TC[13].par[z].fmt[1]) {
       case 'u':
            fprintf(f_out, "%u", integer_param);
            break;
       case 'd':
            fprintf(f_out, "%d", integer_param);
            break;
       default:
            fprintf(f_out, "%02X", integer_param); 
            break;      
            }
       }
/**/
/* build_raleph() - building of TC RALEPH in output file */
/**/
void   build_raleph() {
       i=0;
       fputs("RALEPH:\n", f_out);
       for (z=0; z <26; z++) {
           fputs(TC[13].par[z].nam, f_out);
           i=strlen(TC[13].par[z].nam);
           for (i=i+1; i<40; i++)         
		       fputs(" ", f_out);
/**/
           a=TC[13].par[z].bgn;
           start_value=(a+4)*2;           
           b=TC[13].par[z].end;
           lenght_value=((b-a)+1)*2;
/**/
           for (c=0; c < lenght_value; c++) {
               value_param[c] = buff_outf[start_value];
               start_value++;
               }
               value_param[c] = '\0';
               decode_value();
               print_format();
               fputs("\n", f_out);
           }
           fputs("\ndelay 2 \n\n", f_out);
       }
/**/
/* extract_week() - extraction of almanac week number in input file */
/**/
void   extract_week_number() {
/**/
        sscanf(&buff_out[28], "%04X", &week__number);
        sprintf(week___number, "%04d", week__number); 
        strcpy (output_file, "almanac-");
/**/
        strcat(output_file, week___number); 
        strcat (output_file, ".tc");
       }
/**/
void   test_end_saleph() {
       test_end = 0;
	     b = i;
/**/
	     for (a=0; a <23; a++){
          temp_end_saleph[a] = buff_in[b];
		      b++;
          }
	     temp_end_saleph[a] = '\0';
       if (strcmp(temp_end_saleph, end_saleph) == 0){
		      test_end = 1;
	        }
       }
/**/
int main(int argc, char *argv[]) 
{
     setbuf(stdout, NULL);
     printf ("usage : %s file name \n", argv[0]);
     if (argc < 2) exit(1);
     strcpy (input_file, argv[1]);
/**/
/* Opening of first input file */
/**/
     if ((f_in = fopen(input_file,"r")) == NULL) {
          fprintf(stderr, "\nError in reading file %s\n", argv[1]);
          return(1);
     }
/**/
/* initialisation of PRN table */
/**/
     for (z=0; z<64; z++) prn_tab[z] = 0;     
/**/
/* Read input until SALEPH found */
/**/
     while (!feof(f_in)){
          fgets(buff_in, MAX_IN, f_in);
          if (buff_in[0] == '9'){
          for (i=0; i <11; i++){
               ident1_saleph[i] = buff_in[i];
               }
               ident1_saleph[i] = '\0';
          if (strcmp(ident1_saleph, string1_saleph) == 0){
               i = 0;
               while (buff_in[i] !='\n'){
				           if (buff_in[i] == '5'){
						       test_end_saleph();
						       if (test_end == 1){
							         number_saleph++;
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
     }
     buff_out[y] = '\0';
     fclose(f_in);
/**/
     if (number_saleph < 32) {
          fprintf(stderr, "\nNot enough TM SALEPH found : %d\n\n", number_saleph);
          return(1);
     }
/**/
/* Opening of final tc output file */
/**/
      extract_week_number();
/**/
      if ((f_out = fopen(output_file,"w")) == NULL) {
       fprintf(stderr, "\nError in open file : %s\n", output_file);
       return(1);
      }
/**/       
          i=0;
          y=0;
          w=0;
          x=8;
          number_saleph = 0;
/**/      
      while (buff_out[y] != '\0') {
           for (y=w+8; y < w+144; y++) {
           buff_outf[i] = buff_out[x];
           i++;
           x++;
           }
      buff_outf[i] = '\0';
/**/
      checksum_test();
      if (test_check == 1) {
      first_tests();
         if (test_saleph == 1) {
            build_raleph();
            number_raleph++;
         }
      }
/**/
      i=0;
      x=x+16;
      w=x;
      x=x+8;
      number_saleph++;
      if (number_saleph == 32)
      break;
      }
/**/ 
     printf("\nTM SALEPH checksum error : %d\n", error_checksum);
     printf("Duplicate Signal Source error : %d\n", duplicate_prn); 
     printf("Week number error : %d\n", error_week);  
     printf("TC RALEPH number : %d\n", number_raleph); 
     printf("Output File = %s\n\n", output_file);   
     fclose(f_out);
     return (0);
}

