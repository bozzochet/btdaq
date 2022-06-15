//----------------------------------------------------------
//  File: decode_funcs.h
//
//  Header file for AMS-02 decode functions
//
//  History:
//    20190730 - created by X.Cai
//----------------------------------------------------------

#ifndef __DECODE_FUNCS_H
#define __DECODE_FUNCS_H

//----------------------------------------------------------
//  Global variables
//----------------------------------------------------------

extern char block_time[50];

#ifndef NAN
#define NAN     (0.0/0.0)
#endif

//----------------------------------------------------------
//  Types
//----------------------------------------------------------

// Sub-detector DAQ data decode functions
typedef void daq_func_t(AMSBlock *block,            // Block
                        const uint64_t utime,       // time to be set for time stamp
                        const uint32_t id,          // Run ID
                        const int node,             // Sub-detector node address
                        const int size,             // Sub-detector data size
                        uint8_t *dptr);             // Sub-detector data pointer

// USB box modes
enum __usbbox_mode {
    USB_TTTC = 0,
    USB_TTPC,
    USB_1553,
    USB_GSE,
    USB_FGSE,
    USB_LINF_V1,
    USB_LINF_V2,
};

extern char *usbbox_names[];

//----------------------------------------------------------
//  Utilities
//----------------------------------------------------------

// State operations
typedef struct __state_var {
    int                 state;      // last error state
    uint64_t            time;       // Last entry time
    char               *tags0;      // tags for state 0
    char               *field0;     // field name for state 0
    char               *tags1;      // tags for state 1
    char               *field1;     // field name for state 1
    struct __state_var *next;       // Next error in the list
} state_var_t;

// Constant
enum {
    sINT = 0,
    sBOOL,
    s1MIN = (60 * 1000000),
    s5MIN = (5 * 60 * 1000000),
    s10MIN = (10 * 60 * 1000000),
    s30MIN = (30 * 60 * 1000000),
};

extern state_var_t *state_var_new(const int initial,    // Initial value of the state
                                  const char *tags0,    // tags for state 0
                                  const char *field0,   // field for state 0
                                  const char *tags1,    // tags for state 1
                                  const char *field1);  // filed for state 1
extern void state_list_add(state_var_t **list, state_var_t *item);
extern void state_list_del(state_var_t **list, state_var_t *item);
extern state_var_t *state_list_find(state_var_t *list, const char *tags0, const char *tags1);
extern int state_var_set(state_var_t *stsv,        // State variable
                         const int state,          // state to set
                         const int mode,           // mode=0:0/1, mode=1:false/true
                         uint64_t utime,           // Time
                         const uint64_t period,    // Time period control, 0 means now
                         const int debug);         // Debug flag

//----------------------------------------------------------
//  CAN nodes (except TTCS)
//----------------------------------------------------------

extern int check_ds_sensor;     // Depend the DB name or path name to decide whether sensor numbers to be checked
extern state_var_t *ds_errs;    // DS error lists (new method)
extern state_var_t  ds_nerrs;   // No DS error state (new method)

#define DS_NNODES   12
#define LTA         0
#define LTW         1
#define HTW         2
#define HTA         3

extern const char *ds_tags[12][9][64];
extern const int ds_nsens[12][9];
extern float ds_limits[12][9][64][4];


// TRDGAS temperatures
extern int16_t spiro_temp[2][2];
extern int16_t mfdp_temp[2];

// For CAN nodes
extern void decode_can_nodes(AMSBlock *block, uint64_t utime);

// Find DS node ID
extern int ds_get_node(uint16_t node);

// PDS data
extern int pds_em;
extern void decode_pds_data(AMSBlock *block, uint64_t utime);

// GPS telemetry data
extern void decode_gps_data(AMSBlock *block, uint64_t utime);

// Star Tracker telemetry data
extern void decode_aste_data(AMSBlock *block, uint64_t utime);

// TAS data
extern void decode_tas_adc(AMSBlock *block, uint64_t utime);
extern void decode_tas_dac(AMSBlock *block, uint64_t utime);
extern void decode_tas_lvds(AMSBlock *block, uint64_t utime);

//----------------------------------------------------------
//  TTCS nodes
//----------------------------------------------------------

// For TTCS nodes
extern void decode_ttcs_nodes(AMSBlock *block, uint64_t utime);

//----------------------------------------------------------
//  UTTPS nodes
//----------------------------------------------------------

extern int uttps_format;
extern int uttps_calib;
extern int skip_2020;

extern void decode_uttps(AMSBlock *block, uint64_t utime);

// Pump test data (using usbTTTC or usbTTPC boxes)
enum __pt_sys {GSE_FS = 0, CERN_LT1 = 1, AACHEN = 10,};
extern int pt_sys;
extern void decode_pumptest(AMSBlock *block, uint64_t utime, const int mode);

//----------------------------------------------------------
//  JMDC nodes
//----------------------------------------------------------

extern char *event_errcode[16];

extern void decode_jmdc(AMSBlock *block, uint64_t utime);
extern void decode_jmdc_chd(AMSBlock *block, uint64_t utime);


//----------------------------------------------------------
//  Laptop nodes
//----------------------------------------------------------

extern void decode_alc(AMSBlock *block, uint64_t utime);

//----------------------------------------------------------
//  FEP servers
//----------------------------------------------------------

extern void decode_fep(AMSBlock *block, uint64_t utime);

//----------------------------------------------------------
//  DSP nodes
//----------------------------------------------------------

extern int check_dsp_err;

extern void decode_dsp(AMSBlock *block, uint64_t utime);

//----------------------------------------------------------
//  Tracker data
//----------------------------------------------------------

extern void decode_tracker_cal(AMSBlock *block,             // Block
                               const uint64_t utime,        // time to be set for time stamp
                               const uint32_t id,           // Run ID
                               const int node,              // Sub-detector node address
                               const int size,              // Sub-detector data size
                               uint8_t *dptr);              // Sub-detector data pointer

extern void decode_tracker_cfg(AMSBlock *block,             // Block
                               const uint64_t utime,        // time to be set for time stamp
                               const uint32_t id,           // Run ID
                               const int node,              // Sub-detector node address
                               const int size,              // Sub-detector data size
                               uint8_t *dptr);              // Sub-detector data pointer

extern void decode_tracker_hkd(AMSBlock *block,             // Block
                               const uint64_t utime,        // time to be set for time stamp
                               const uint32_t id,           // Run ID
                               const int node,              // Sub-detector node address
                               const int size,              // Sub-detector data size
                               uint8_t *dptr);              // Sub-detector data pointer

extern void decode_jinft_event(const int id,                // Tracker crate ID (0 ... 7)
                               uint16_t *data,              // Data pointer
                               const int len);              // Number of 16-bit words

extern void sum_jinft_event(const int evt_cnt,              // Number of events
                            const uint64_t utime);          // Time tag to be used for DB entry

extern void sum_jinft_distr(const uint64_t utime);          // Time tag to be used for DB entry

//----------------------------------------------------------
//  TRD data
//----------------------------------------------------------

extern void init_trd(void);

extern void decode_trd_cal(AMSBlock *block,                 // Block
                           const uint64_t utime,            // time to be set for time stamp
                           const uint32_t id,               // Run ID
                           const int node,                  // Sub-detector node address
                           const int size,                  // Sub-detector data size
                           uint8_t *dptr);                  // Sub-detector data pointer

extern void decode_trd_cfg(AMSBlock *block,                 // Block
                           const uint64_t utime,            // time to be set for time stamp
                           const uint32_t id,               // Run ID
                           const int node,                  // Sub-detector node address
                           const int size,                  // Sub-detector data size
                           uint8_t *dptr);                  // Sub-detector data pointer

extern void decode_trd_hkd(AMSBlock *block,                 // Block
                           const uint64_t utime,            // time to be set for time stamp
                           const uint32_t id,               // Run ID
                           const int node,                  // Sub-detector node address
                           const int size,                  // Sub-detector data size
                           uint8_t *dptr);                  // Sub-detector data pointer

extern void decode_jinfu_event(const int id,                // TRD crate ID (0, 1)
                               uint16_t *data,              // Data pointer
                               const int len);              // Number of 16-bit words

extern void sum_jinfu_event(const int evt_cnt,              // Number of events
                            const uint64_t utime);          // Time tag to be used for DB entry

extern void jinfu_sd_procedures(const int node, const uint16_t *data, const int size, uint64_t utime);
extern void jinfu_hk_procedures(const int node, const uint16_t *data, const int size, uint64_t utime);

extern void udr_sd_procedures(const int node, const uint16_t *data, const int size, uint64_t utime);
extern void udr_hk_procedures(const int node, const uint16_t *data, const int size, uint64_t utime);
extern void udr_cal_status(const int node, const uint16_t *data, const int size, uint64_t utime);

//----------------------------------------------------------
//  TRD Gas data
//----------------------------------------------------------

extern void init_trdgas(void);
extern void decode_ugscm_data(AMSBlock *block, uint64_t utime);
extern void decode_ug_mfdp(AMSBlock *block, uint64_t utime);
extern void decode_ug_hkdata(AMSBlock *block, uint64_t utime);
extern void decode_ug_chdparam(AMSBlock *block, uint64_t utime);

//----------------------------------------------------------
//  ACC data
//----------------------------------------------------------

// extern void decode_acc_hk(AMSBlock *block, uint64_t utime);

//----------------------------------------------------------
//  JLV1 data
//----------------------------------------------------------

extern void decode_jlv1_regs(int node, uint16_t *data, int nw, const uint64_t utime);
extern void decode_jlv1_dm(int node, uint16_t *data, int nw, const uint64_t utime);

extern void decode_jlv1_cal(AMSBlock *block,                // Block
                            const uint64_t utime,           // time to be set for time stamp
                            const uint32_t id,              // Run ID
                            const int node,                 // Sub-detector node address
                            const int size,                 // Sub-detector data size
                            uint8_t *dptr);                 // Sub-detector data pointer

extern void decode_jlv1_cfg(AMSBlock *block,                // Block
                            const uint64_t utime,           // time to be set for time stamp
                            const uint32_t id,              // Run ID
                            const int node,                 // Sub-detector node address
                            const int size,                 // Sub-detector data size
                            uint8_t *dptr);                 // Sub-detector data pointer

extern void decode_jlv1_hkd(AMSBlock *block,                // Block
                            const uint64_t utime,           // time to be set for time stamp
                            const uint32_t id,              // Run ID
                            const int node,                 // Sub-detector node address
                            const int size,                 // Sub-detector data size
                            uint8_t *dptr);                 // Sub-detector data pointer

//----------------------------------------------------------
//  TOF data
//----------------------------------------------------------

extern void decode_tof_cal(AMSBlock *block,                 // Block
                           const uint64_t utime,            // time to be set for time stamp
                           const uint32_t id,               // Run ID
                           const int node,                  // Sub-detector node address
                           const int size,                  // Sub-detector data size
                           uint8_t *dptr);                  // Sub-detector data pointer

extern void decode_tof_cfg(AMSBlock *block,                 // Block
                           const uint64_t utime,            // time to be set for time stamp
                           const uint32_t id,               // Run ID
                           const int node,                  // Sub-detector node address
                           const int size,                  // Sub-detector data size
                           uint8_t *dptr);                  // Sub-detector data pointer

extern void decode_tof_hkd(AMSBlock *block,                 // Block
                           const uint64_t utime,            // time to be set for time stamp
                           const uint32_t id,               // Run ID
                           const int node,                  // Sub-detector node address
                           const int size,                  // Sub-detector data size
                           uint8_t *dptr);                  // Sub-detector data pointer

extern void decode_sdr_event(const int id,                  // TRD crate ID (0, 1)
                             uint16_t *data,                // Data pointer
                             const int len);                // Number of 16-bit words

extern void sum_sdr_event(const int evt_cnt,                // Number of events
                          const uint64_t utime);            // Time tag to be used for DB entry

extern void sdr_sd_procedures(const int node, const uint16_t *data, const int size, uint64_t utime);
extern void sdr_hk_procedures(const int node, const uint16_t *data, const int size, uint64_t utime);

//----------------------------------------------------------
//  RICH data
//----------------------------------------------------------

extern void decode_rich_cal(AMSBlock *block,                // Block
                            const uint64_t utime,           // time to be set for time stamp
                            const uint32_t id,              // Run ID
                            const int node,                 // Sub-detector node address
                            const int size,                 // Sub-detector data size
                            uint8_t *dptr);                 // Sub-detector data pointer

extern void decode_rich_cfg(AMSBlock *block,                // Block
                            const uint64_t utime,           // time to be set for time stamp
                            const uint32_t id,              // Run ID
                            const int node,                 // Sub-detector node address
                            const int size,                 // Sub-detector data size
                            uint8_t *dptr);                 // Sub-detector data pointer

extern void decode_rich_hkd(AMSBlock *block,                // Block
                            const uint64_t utime,           // time to be set for time stamp
                            const uint32_t id,              // Run ID
                            const int node,                 // Sub-detector node address
                            const int size,                 // Sub-detector data size
                            uint8_t *dptr);                 // Sub-detector data pointer

extern void decode_jinfr_event(const int id,                // RICH crate ID (0, 1)
                               uint16_t *data,              // Data pointer
                               const int len);              // Number of 16-bit words

extern void sum_jinfr_event(const int evt_cnt,              // Number of events
                            const uint64_t utime);          // Time tag to be used for DB entry

//----------------------------------------------------------
//  ECAL data
//----------------------------------------------------------

extern void decode_ecal_cal(AMSBlock *block,                // Block
                            const uint64_t utime,           // time to be set for time stamp
                            const uint32_t id,              // Run ID
                            const int node,                 // Sub-detector node address
                            const int size,                 // Sub-detector data size
                            uint8_t *dptr);                 // Sub-detector data pointer

extern void decode_ecal_cfg(AMSBlock *block,                // Block
                            const uint64_t utime,           // time to be set for time stamp
                            const uint32_t id,              // Run ID
                            const int node,                 // Sub-detector node address
                            const int size,                 // Sub-detector data size
                            uint8_t *dptr);                 // Sub-detector data pointer

extern void decode_ecal_hkd(AMSBlock *block,                // Block
                            const uint64_t utime,           // time to be set for time stamp
                            const uint32_t id,              // Run ID
                            const int node,                 // Sub-detector node address
                            const int size,                 // Sub-detector data size
                            uint8_t *dptr);                 // Sub-detector data pointer

extern void decode_jinfe_event(const int id,                // ECAL crate ID (0, 1)
                               uint16_t *data,              // Data pointer
                               const int len);              // Number of 16-bit words

extern void sum_jinfe_event(const int evt_cnt,              // Number of events
                            const uint64_t utime);          // Time tag to be used for DB entry

extern void jinfe_sd_procedures(const int node, const uint16_t *data, const int size, uint64_t utime);
extern void jinfe_hk_procedures(const int node, const uint16_t *data, const int size, uint64_t utime);

//----------------------------------------------------------
//  Ground test data
//----------------------------------------------------------

extern void decode_ground_test(AMSBlock *block, uint64_t utime);

//----------------------------------------------------------
//  L0 test data
//----------------------------------------------------------

#define LEF_RAW_MAX 100000
extern double raw_data[1024][LEF_RAW_MAX+100];
extern int nevents;             // Number of events
extern uint64_t event_time;     // last event time for write DB
extern int last_node;           // Last node address used

extern void decode_l0_upgrade(AMSBlock *block, uint64_t utime, const int mode, const char *info);
extern void lef_raw_calib(const int mode, const char *info, const char *fname);

//----------------------------------------------------------
//  End
//----------------------------------------------------------
#endif  // __DECODE_FUNCS_H
