#ifdef BUILD_FOR_CC

// file cceb.h
//
// A.Lebedev Mar 2007 - ...
//

#ifndef _CCEB_H_
#define _CCEB_H_

//~--- Commands ---

#define CC_ON    1
#define CC_OFF   2
#define CC_START 3
#define CC_STOP  4
#define CC_HALT  5

//~--- State Machine States ---

#define CC_IDLE        1
#define CC_SWITCH_ON_0 2
#define CC_SWITCH_ON_1 3
#define CC_SWITCH_ON_2 4
#define CC_SWITCH_OFF  5
#define CC_READY       6
#define CC_STARTUP_0   7
#define CC_STARTUP_1   8
#define CC_STARTUP_2   9
#define CC_STARTUP_3  10
#define CC_SHUTDOWN   11
#define CC_RUN        12

//~--- cc[i].fail codes ---

#define CC_FAILED_OSCILLATOR            0x0001
#define CC_FAILED_TO_POWER_ON_CCIF      0x0002
#define CC_FAILED_TO_ACTIVATE_RELAYS    0x0004
#define CC_FAILED_TO_CHECK_RELAY_STATUS 0x0008
#define CC_FAILED_TO_DEACTIVATE_RELAYS  0x0010
#define CC_FAILED_TO_SETUP_RELAYS       0x0020
#define CC_FAILED_TO_POWER_ON_PA        0x0040
#define CC_FAILED_TO_START_PA           0x0080
#define CC_FAILED_TO_CHECK_PA           0x0100
#define CC_FAILED_TO_STOP_PA            0x0200
#define CC_FAILED_PA_POWER              0x0400
#define CC_FAILED_TO_SET_PHASE          0x0800

//~--- cc[i].alarm codes ---

#define CC_ALARM_REJECT_T_TIMEOUT       0x0001
#define CC_ALARM_REJECT_T_LOW           0x0002
#define CC_ALARM_REJECT_T_HIGH          0x0004
#define CC_ALARM_CASE_T_TIMEOUT         0x0008
#define CC_ALARM_CASE_T_LOW             0x0010
#define CC_ALARM_CASE_T_HIGH            0x0020
#define CC_ALARM_P_BUS_LOW              0x0040
#define CC_ALARM_P_BUS_HIGH             0x0080
#define CC_ALARM_V_RMS_LOW              0x0100
#define CC_ALARM_V_RMS_HIGH             0x0200
#define CC_ALARM_I_RMS_LOW              0x0400
#define CC_ALARM_I_RMS_HIGH             0x0800
#define CC_ALARM_Z_RMS_LOW              0x1000
#define CC_ALARM_Z_RMS_HIGH             0x2000
#define CC_ALARM_POWER_LOW              0x4000
#define CC_ALARM_POWER_HIGH             0x8000

//~--- CCEB DACs ---

#define OSC_H_ON      0 
#define OSC_C_ON      1 

#define CCIF0_A_ON    2
#define CCIF0_B_ON    3
#define CCIF1_A_ON    4
#define CCIF1_B_ON    5
#define CCIF2_A_ON    6
#define CCIF2_B_ON    7
#define CCIF3_A_ON    8
#define CCIF3_B_ON    9

#define CCMO0_ON     12
#define CCMO1_ON     13

//~--- CCEB CCIF Status Bits ---

#define PA1_POWER     0x0001
#define PA1_RUN       0x0002
#define PA2_POWER     0x0004
#define PA2_RUN       0x0008

#define PAR_TO_FEED_A 0x0010
#define PAR_TO_FEED_B 0x0020
#define LLR_TO_OPER   0x0040
#define LLR_TO_LOCK   0x0080

#define PA1_POWER_FF  0x0010
#define PA2_POWER_FF  0x0020
#define PAR_STATUS    0x0040    // 0 = Feed B,  1 = Feed A.
#define LLR_STATUS    0x0080    // 0 = Locked,  1 = Operational.

#define PAR_ON_FEED_A   PAR_STATUS
#define PAR_ON_FEED_B   0x0000
#define LLR_LOCKED      0x0000
#define LLR_OPERATIONAL LLR_STATUS

//~--- CCEB ADCs ---

#define OSC_H_OK      0 
#define V_FEED_A      1

#define CC0_COLD_TIP  2
#define CC0_VCS       3
#define V_BUS_0       4
#define I_BUS_0       5
#define V_RMS_0       6
#define I_RMS_0       7

#define I_FEED_A      8
#define I_VCC         9

#define CC2_COLD_TIP 10
#define CC2_VCS      11
#define V_BUS_1      12
#define I_BUS_1      13
#define V_RMS_1      14
#define I_RMS_1      15

#define OSC_C_OK     16
#define V_FEED_B     17

#define CC1_COLD_TIP 18
#define CC1_VCS      19
#define V_BUS_2      20
#define I_BUS_2      21
#define V_RMS_2      22
#define I_RMS_2      23

#define I_FEED_B     24
#define I_VRR        25

#define CC3_COLD_TIP 26
#define CC3_VCS      27
#define V_BUS_3      28
#define I_BUS_3      29
#define V_RMS_3      30
#define I_RMS_3      31

//~--- Data Types Definition ---

typedef struct {
  uint8 selected;     // <<<== setup: select oscillator H(0) or C(1)
  bool on;
} _osc;

typedef struct {    // "power" limits
  uint32 value;     // <<<== setup (for P_bus may be changed automatically)
  uint32 probation; // <<<== setup (in msec)
  uint32 tstamp;    // timer
  uint16 signal;
} _P_limit;

typedef struct {    // "Dallas" limits
  char   value;     // <<<== setup
  uint32 probation; // <<<== setup (in msec)
  uint32 tstamp;    // timer
  uint16 signal;
} _D_limit;

typedef struct {
  uint32 measured;
  _P_limit min;
  _P_limit max;
} _power_data;

typedef struct {
  bool   use;       // <<<== setup
  char   measured;
  bool   ok;
  uint32 age;
  uint32 probation; // <<<== setup: in msec
  uint32 tstamp;    // timer
  uint16 signal;
  _D_limit min;
  _D_limit max;
} _dallas_data;

typedef struct {
  char   t_min;     // <<<== setup
  char   t_max;     // <<<== setup
  uint32 P_max;     // <<<== setup
} _dallas_t_table;

typedef struct {
  uint8  index;     // <<<== setup (but not used in USCM)
  bool   use;       // <<<== setup
  uint16 measured;
} _cernox_data;

typedef struct {
  uint16 Cold_tip_t_min;  // <<<== setup
  uint16 Cold_tip_t_max;  // <<<== setup
  uint16 VCS_t_min;       // <<<== setup
  uint16 VCS_t_max;       // <<<== setup
  uint32 P_max;           // <<<== setup
} _cernox_t_table;

typedef struct {
  bool   use;         // <<<== setup: TRUE(1) or FALSE(0)
  uint8  CCIF;        // <<<== setup: select CCIF A(0) or B(1)
  uint8  feed;        // <<<== setup: select feed A(0) or B(1) for PA
  uint8  state;       // state machine current state
  uint32 tstamp;      // timer
  uint8  command;     // CCEB Control Command (ON, OFF, START, STOP, HALT)
  struct {
    uint16 fail;      // pattern: hardware failures (CC_FAILED)
    uint16 alarm;     // pattern: sensor data out of limits (CC_ALARM)
    uint16 warning;   // pattern: sensor data out of limits (CC_ALARM)
  } now;
  struct {
    uint16 fail;      // pattern: hardware failures history (CC_FAILED)
    uint16 alarm;     // pattern: sensor data out of limits history (CC_ALARM)
    uint16 warning;   // pattern: sensor data out of limits (CC_ALARM)
  } was;
  struct {
    uint16 fail;      // <<<== setup: check only set bits in fail field (CC_FAILED)
    uint16 alarm;     // <<<== setup: check only set bits in alarm field (CC_ALARM)
  } check;
  struct {
    uint8 limit[16];
    uint8 counter[16];
  } fail;
  struct {
    uint8 limit[16];
    uint8 counter[16];
  } alarm;
  _power_data power;
  _power_data V_bus;
  _power_data I_bus;
  _power_data P_bus;
  _power_data V_RMS;
  _power_data I_RMS;
  _power_data Z_RMS;
  _dallas_data Reject_t;
  _dallas_data Case_t;
  _dallas_t_table dallas_t_table[2];
  _cernox_data Cold_tip_t;
  _cernox_data VCS_t;
  _cernox_t_table cernox_t_table[3];
  uint8  phase_now;
  uint8  phase_startup;   // <<<== setup
  uint8  phase_shutdown;  // <<<== setup (manual shutdown control)
  uint8  phase_target;    // <<<== setup (manual running control)
  uint8  phase_min;       // <<<== setup (for "safety" ("before" switching to manual)
  uint8  phase_max;       // <<<== setup (for "safety" ("before" switching to manual)
  uint32 power_target;    // <<<== setup
  uint32 V_RMS_shutdown;  // <<<== setup
} _cc;

//~--- Global Functions ---

extern void initialize_CCEB_task(void);
extern void CCEB_task(void);

//~============================================================================

#endif // _CCEB_H_

#endif // #ifdef BUILD_FOR_CC

