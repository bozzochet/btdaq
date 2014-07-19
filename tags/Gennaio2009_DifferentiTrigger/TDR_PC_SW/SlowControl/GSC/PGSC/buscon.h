/* buscon.h */

#define syn_pat 0xC95A6300
#define PDI_frame_size  160
#define PSP_frame_size  128
#define max_command_block_length (PSP_frame_size/2 - 4)
#define AMS_root_code 7
//#define g 0x1021 /* x16 + x12 + x5 + 1 CRC generator polynomial */
#define pdi_buffer_size 100

short enable_bus_controller(void);
void disable_bus_controller(void);
void send_PSP(unsigned char *, volatile unsigned short *);
unsigned short take_PDI(unsigned char **);
unsigned short fcs(unsigned short *, unsigned short);
void make_PSP(unsigned char *, struct AMS_command_block *);                    

struct AMS_command_block {
  unsigned short length;
  unsigned short body[max_command_block_length];
};



