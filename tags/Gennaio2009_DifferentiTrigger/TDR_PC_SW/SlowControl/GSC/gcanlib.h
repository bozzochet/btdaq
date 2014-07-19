// file gcanlib.h
//
// Stolen from S.Xu (gcan) and reworked
// (is used in cab-slave programs etc).
//
// A.Lebedev - Feb 2008 ...
//

#ifndef _GCANLIB_H_
#define _GCANLIB_H_

#include "mylib.h"

//~----------------------------------------------------------------------------

//~--  ADDRESS MAP

#define CTL      0x00      // control register                     
#define STA      0x01      // status register                      
#define MCPU     0x02      // CPU interface register               
#define HSR0     0x04      // high speed access register-0 (read only) 
#define HSR1     0x05      // high speed access register-1 (read only) 
#define SGM0     0x06      // standard global mask register-0
#define SGM1     0x07      // standard global mask register-1
#define EGM0     0x08      // extended global mask register-0               
#define EGM1     0x09      // extended global mask register-1               
#define EGM2     0x0A      // extended global mask register-2               
#define EGM3     0x0B      // extended global mask register-3               
#define M15M0    0x0C      // message object 15 mask register-0                       
#define M15M1    0x0D      // message object 15 mask register-1                      
#define M15M2    0x0E      // message object 15 mask register-2                      
#define M15M3    0x0F      // message object 15 mask register-3                      
#define MSG(i)   0x10*i    // message object                          
#define CLK      0x1F      // clkout register                      
#define BUS      0x2F      // bus config register                  
#define BT0      0x3F      // bit timing register-0
#define BT1      0x4F      // bit timing register-1                 
#define IRQ      0x5F      // interrupt register                
#define P1C      0x9F      // p1 conf                              
#define P1I      0xBF      // p1 in                                
#define P1O      0xDF      // P1 out                               
#define P2C      0xAF      // p2 conf                              
#define P2I      0xCF      // p2 in                                
#define P2O      0xEF      // p2 out                               
#define SRA      0xFF      // serial reset address                 

//~--  MESSAGE BOX

#define MSGCTL0  0x00
#define MSGCTL1  0x01
#define MSGID0   0x02
#define MSGID1   0x03
#define MSGID2   0x04
#define MSGID3   0x05
#define MSGCFG   0x06
#define MSGDAT0  0x07
#define MSGDAT1  0x08
#define MSGDAT2  0x09
#define MSGDAT3  0x0A
#define MSGDAT4  0x0B
#define MSGDAT5  0x0C
#define MSGDAT6  0x0D
#define MSGDAT7  0x0E

#define DATPKT           0x80
#define SIGPKT           0x00

#define REQPKTI	         0x00
#define REQPKTL	         0x20
#define REQPKTF	         0x40
#define REQPKTS	         0x60
#define REQPKTNEXT       0x00
#define REQPKTERROR      0x20
#define REQPKTABORT      0x40
#define REQPKTEND        0x60

#define REPPKTI	         0x10
#define REPPKTL	         0x30
#define REPPKTF	         0x50
#define REPPKTS	         0x70
#define REPPKTNEXT       0x10
#define REPPKTERROR      0x30
#define REPPKTABORT      0x50
#define REPPKTEND        0x70

#define CHKBC1(msb)      msb &   0x40
#define CHKBC2(msb)      msb &   0x20

#define CHKRR(msb)       msb &   0x10
#define CHKRW(msb)       msb &   0x08
#define CHKBUSREQ(msb)   msb &   0x04
#define CHKBUSREP(msb)   msb &   0x02
#define CHKNENBORN(msb)  msb &   0x01
                                 
#define SETBC1(msb)      msb |=  0x40	
#define SETBC2(msb)      msb |=  0x20
#define SETRR(msb)       msb |=  0x10
#define SETRW(msb)       msb |=  0x08
#define SETBUSREQ(msb)   msb |=  0x04
#define SETBUSREP(msb)   msb |=  0x02
#define SETNEWBORN(msb)  msb |=  0x01

#define CLRBC1(msb)      msb &= ~0x40	
#define CLRBC2(msb)      msb &= ~0x20
#define CLRRR(msb)       msb &= ~0x10
#define CLRRW(msb)       msb &= ~0x08
#define CLRBUSREQ(msb)   msb &= ~0x04
#define CLRBUSREP(msb)   msb &= ~0x02
#define CLRNEWBORN(msb)  msb &= ~0x01

#define BUS_AA      (0x00)
#define BUS_AB      (0x02)
#define BUS_BA      (0x04)
#define BUS_BB      (0x06)
#define BUS_RAND    (rand()&0x06)

#define REQRD       (0x00)
#define REQWR       (0x08)
#define NEWBORN     (0x01)

typedef enum {
    JMDC_0,
    JMDC_1,
    JMDC_2,
    JMDC_3,  
} JMDCID;

typedef enum {
    BUSA,
    BUSB
} CANBUS;

typedef struct {
    CANBUS bus;
    int8  egm[4];
} CANCFG;

typedef struct {
    int8 cid[4];
    int8 dlc;
    int8 dat[8];
} CANPKT;

typedef struct {
    int32 len;
    int8  cid[4];
    int8  dat[0x40000];
} CANBLK;

typedef enum {            
    SACT_INIT_CHANNEL     , 
    SACT_WAIT_REQPKT      , 
    SACT_RECV_REQPKTI     , 
    SACT_RECV_REQPKTL     , 
    SACT_RECV_REQPKTF     , 
    SACT_RECV_REQPKTS     , 
    SACT_RECV_REQPKTNEXT  , 
    SACT_RECV_REQPKTERROR , 
    SACT_RECV_REQPKTABORT , 
    SACT_RECV_REQPKTEND   , 
    SACT_PROC_CENTER      , 
    SACT_SEND_REPPKT
} SFSM_ACTION;

typedef enum {            
    SHIS_RECV_REQPKTI     = DATPKT | REQPKTI     ,
    SHIS_RECV_REQPKTL     = DATPKT | REQPKTL     ,
    SHIS_RECV_REQPKTF     = DATPKT | REQPKTF     ,
    SHIS_RECV_REQPKTS     = DATPKT | REQPKTS     ,
    SHIS_RECV_REQPKTNEXT  = SIGPKT | REQPKTNEXT  ,
    SHIS_RECV_REQPKTERROR = SIGPKT | REQPKTERROR ,
    SHIS_RECV_REQPKTABORT = SIGPKT | REQPKTABORT ,
    SHIS_RECV_REQPKTEND   = SIGPKT | REQPKTEND    
} SFSM_HISTORY;

typedef enum {
    SERR_NONE    , // No error.
    SERR_PKTSEQU , // Packet sequence error.
    SERR_PKTTYPE , // Packet type (R/W) error.
    SERR_PKTLOST , // Packet lost.       
} SFSM_ERRNO;

typedef struct {
    int32 rx;
    int32 tx;
    int32 pkt_sequ_err;
    int32 pkt_type_err;
    int32 pkt_lost_err;
} SFSM_COUNTER;

typedef struct {
    SFSM_ACTION   action;
    SFSM_HISTORY  history;
    SFSM_ERRNO    error;   
    SFSM_COUNTER  counter;

    int8  mark;

    CANBUS rxbus;
    int8  rxbox;
    int8  rxmsb;
    int8  rxdlc;
    int32 rxlen;
    int8 *rxdat;
    
    CANBUS txbus;
    int8  txbox;
    int8  txmsb;
    int8  txdlc;
    int32 txlen;
    int8 *txdat;
} SFSM_CHANNEL;

typedef int sfsm_user_call(int8 *txbuf, int8 *rxbuf, int relen);

#define CHKNEWDAT(bval)  (bval & 0x02)
#define CHKMSGLST(bval)  (bval & 0x08)

//~----------------------------------------------------------------------------

void open_epp(void);
int test_epp(void);
void close_epp(void);

int8 rcan(CANBUS bus, int8 reg);
void wcan(CANBUS bus, int8 reg, int8 val);
void init_can(CANCFG *p);

void dump_buff(int8 *buff, int nbytes);
void dump_block(CANBLK *blk);
void dump_packet(CANPKT *pkt);
void dump_can(CANBUS bus);

inline void slave_send_s(SFSM_CHANNEL *p);
inline void slave_send_f(SFSM_CHANNEL *p);
inline void slave_send_i(SFSM_CHANNEL *p);
inline void slave_send_l(SFSM_CHANNEL *p);
inline void slave_send_abort(SFSM_CHANNEL *p);
inline void slave_send_next(SFSM_CHANNEL *p);
inline void slave_send_error(SFSM_CHANNEL *p);
inline void slave_send_end(SFSM_CHANNEL *p);
inline void slave_send_nothing(SFSM_CHANNEL *p);

void sfsm(int8 master, int8 slave, sfsm_user_call *user_func);
void sfsm_status(int8 slave);

//~============================================================================

#endif // _GCANLIB_H_
