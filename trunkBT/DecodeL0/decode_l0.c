/***********************************************************
 *  File: decode_l0.c
 *
 *  C code for AMS-02 decode functions for L0 upgrades nodes
 *
 *  History:
 *    20220330 - created by X.Cai
 ***********************************************************/

#include <math.h>
#include "aallib/logfile.h"
#include "aallib/al_lib.h"
#include "aallib/abi.h"

#include "node_address.h"
#include "decode_funcs.h"
#include "influxdb_client.h"

//----------------------------------------------------------
//  Global variables
//----------------------------------------------------------

#ifndef NAN
#define NAN (0.0/0.0)
#endif

#define NCH     1024
#define NVA     16
#define NVACH   (NCH / NVA)
#define NADC    8

double raw_data[NCH][LEF_RAW_MAX+100];
int nevents = 0;
uint64_t event_time = 0;     // last event time for write DB
int last_node = -1;          // Last node address used

char *usbbox_names[] = {"usbTTTC", "usbTTPC", "usb1553", "usbGSE", "usbFGSE", "usbLINF_v1", NULL};

//----------------------------------------------------------
//  Local variables
//----------------------------------------------------------

static char prename[100] = {""};    // Name prefix
static int last_file = -1;          // Last file number decoded
static double results[4][NCH];

enum __results {
    R_PED = 0,      // Pedestal
    R_SIG,          // Sigma
    R_RSIG,         // Raw sigma
    R_SIGN,         // Signals (???)
};

//----------------------------------------------------------
//  Function:
//      Function to compare two data for sort
//
//  Inputs:
//      a ---- first data
//      b ---- second data
//
//  Return:
//      -1 : a less than b
//       0 : a equal to b
//       1 : a greater than b
//----------------------------------------------------------
static int __compare(const void *a, const void *b)
{
    if      ( *((double *) a) < *((double *) b) ) return -1;
    else if ( *((double *) a) > *((double *) b) ) return 1;
    return 0;
}

//----------------------------------------------------------
//  Function: decode_lef_event
//      Decode function for event from LEF connected to usbL0 box
//
//  Inputs:
//      mode ----- USB box mode
//      info ----- Test info to be added as tag
//      fname ---- File name or NULL to use file number
//
//  Return:
//      None
//----------------------------------------------------------
void lef_raw_calib(const int mode, const char *info, const char *fname)
{
    char names[1000];
    char field[100];
    int v, c, i;

    // Process only when has enough event
    if ( nevents < 1000 ) return;

    // Make name prefix (once)
    if ( strlen(prename) < 10 ) {
        i = snprintf(prename, sizeof(prename), "raw_calib,name=calibration,usbbox=%s,", usbbox_names[mode]);
        if ( info != NULL )
            i += snprintf(&prename[i], (sizeof(prename) - i), "test=%s,", info);
        if ( fname == NULL )
            i += snprintf(&prename[i], (sizeof(prename) - i), "file=%04d/%03d", (last_file / 1000), (last_file % 1000));
        else
            i += snprintf(&prename[i], (sizeof(prename) - i), "file=%s", fname);
    }

    // Add file number to DB
    sprintf(names, "%s,node=%d,node_name=%s", prename, last_node, node_names[last_node]);
    if ( fname == NULL )
        snprintf(field, sizeof(field), "fileno=\"%04d/%03d\",nevents=%di", (last_file / 1000), (last_file % 1000), nevents);
    else
        snprintf(field, sizeof(field), "fileno=\"%s\",nevents=%di", fname, nevents);
    db_client_data(client, names, field, event_time, NO_DEBUG);

    // Clear data
    for ( c = 0; c < NCH; c++ ) {
        for ( i = 0; i < 4; i++ ) results[i][c] = 0.0;
    }

    // Calculate pedestal & raw sigma
    for ( c = 0; c < NCH; c++ ) {
        // Loop on events for pedestal
        for ( i = 0; i < nevents; i++ ) {
            results[R_PED][c] += raw_data[c][i];
        }
        results[R_PED][c] /= nevents;

        // Loop on events for raw sigma
        for ( i = 0; i < nevents; i++ ) {
            results[R_RSIG][c] += (raw_data[c][i] - results[R_PED][c]) * (raw_data[c][i] - results[R_PED][c]);
        }
        results[R_RSIG][c] = sqrt(results[R_RSIG][c] / nevents);
    }

    // Calculate common noise
    int processed[2][NCH];
    memset((void *) processed, 0, sizeof(processed));
    for ( i = 0; i < nevents; i++ ) {
        // Common noise per VA per event
        double noise[NVA];
        for ( v = 0; v < NVA; v++ ) noise[v] = NAN;

        // Loop on channels
        for ( c = 0; c < NCH; c++ ) {
            double vv;

            // VA number
            v = (c / NVACH);

            // Check common noise calculated for this va
            if ( noise[v] != noise[v] ) {
                // Loop on channels of this VA for common noise
                double sort_data[NVACH];
                int nc, vc;
                for ( nc = 0, vc = 0; vc < NVACH; vc++ ) {
                    int ch = (v * NVACH + vc);
                    vv = (raw_data[ch][i] - results[R_PED][ch]);
                    if ( fabs(vv / results[R_RSIG][ch]) < 3 ) sort_data[nc++] = vv;
                    //else printf("event=%d, v=%d, vc=%d, diff=%f, rsig=%f, diff/rsig=%f\n",
                    //            i, v, vc, vv, results[R_RSIG][ch], fabs(vv / results[R_RSIG][ch]));
                }

                //if ( i == 43 ) printf("event=%d, nc=%d\n", i, nc);
                // Sort the differences
                if ( nc >= 2 ) {
                    qsort(sort_data, nc, sizeof(double), __compare);
                    noise[v] = (0.5 * sort_data[nc / 2 - 1]) + sort_data[nc / 2];
                    //if ( i == 0 ) printf("event=%d, v=%d, nc=%d, noise=%f\n", i, v, nc, noise[v]);
                }
                //else printf("event=%d, v=%d, nc=%d\n", i, v, nc);
            }

            if ( noise[v] != noise[v] || fabs(noise[v]) > 10 ) continue;

            // Sigma and average signals
            vv = (raw_data[c][i] - results[R_PED][c] - noise[v]);
            results[R_SIG][c]  += (vv * vv);
            (processed[0][c])++;

            if ( vv > 3.5 ) {
                results[R_SIGN][c] += vv;
                (processed[1][c])++;
            }
        }
    }

    // Sigma calculation
    for ( c = 0; c < NCH; c++ ) {
        results[R_SIG][c]   = sqrt(results[R_SIG][c] / processed[0][c]);
        results[R_SIGN][c] /= processed[1][c];
        printf("c=%-4d ped=%f, rsig=%f, sig=%f, average=%f, n=%d/%d\n", c,
               results[R_PED][c], results[R_RSIG][c], results[R_SIG][c], results[R_SIGN][c], processed[0][c], processed[1][c]);
    }

    // Write to DB
    for ( c = 0; c < NCH; c++ ) {
        // Channel and VAs
        sprintf(names, "%s,node=%d,node_name=%s,va=%d,channel=%d,va_chan=%d,adc=%d",
                prename, last_node, node_names[last_node], (c / NVACH), c, (c % NVACH), (c % NADC));
        sprintf(field, "pedestal=%f,sigma=%f,raw_sigma=%f,signal=%f",
                results[R_PED][c], results[R_SIG][c], results[R_RSIG][c], results[R_SIGN][c]);
        db_client_data(client, names, field, event_time, NO_DEBUG);
    }

    // Prepare for the next calibration
    nevents = 0;
}

//----------------------------------------------------------
//  Function: decode_lef_event
//      Decode function for event from LEF connected to usbL0 box
//
//  Inputs:
//      block ---- Data words (16-bit)
//      utime ---- Time tag for DB entry
//      mode ----- USB box mode
//      info ----- Test info to be added as tag
//
//  Return:
//      None
//----------------------------------------------------------
void decode_lef_raw_event(AMSBlock *block, uint64_t utime, const int mode, const char *info)
{
    int c, b, i, a;

    // Check file no (do one calibration with one data file)
    if ( last_file < 0 ) last_file = file_no;
    else if ( last_file != file_no ) {
        // make calibration
        lef_raw_calib(mode, info, NULL);

        // Save file number
        last_file = file_no;
    }

    // Skip
    if ( block == NULL ) return;
    int s = block->DataCount;
    if ( s != 1794 ) return;

    // Check trigger number
    uint8_t *p = abiDataPtr8(block);
//     if ( trig_numb >= 0 && (trig_numb & 0xFF) == p[1] ) return;
//     trig_numb = p[1];
//     if ( info != NULL )
//         sprintf(names, "raw_events,test=%s,file=%d,name=trig_number,node=%d,node_name=%s",
//                 info, file_no, block->NodeAddress, node_names[block->NodeAddress]);
//     else
//         sprintf(names, "raw_events,file=%d,name=event_number,node=%d,node_name=%s",
//                 file_no, block->NodeAddress, node_names[block->NodeAddress]);
//     sprintf(field, "trig_numb=%di", trig_numb);
//     db_client_data(client, names, field, utime, NO_DEBUG);

    // Decode event data
    for ( p += 2, i = 0; i < (NCH / NADC); i++, p += 14 ) {
        // Loop on ADCs
        for ( a = 0; a < NADC; a++ ) {
            // Init data
            uint16_t w = 0;

            // Loop on 14 bits
            for ( b = 0; b < 14; b++ ) {
                w |= ((p[b] >> a) & 1) << b;
            }

            // one ADC word finished
            b = (a * (NCH / NADC) + i);                             // Original channel number
            c = (((NVA - 1) - (b / NVACH)) * NVACH + (b % NVACH));  // Corrected channel number

            // Save data
            if ( nevents < LEF_RAW_MAX ) raw_data[c][nevents] = ((double) w);

//             // Difference from pedestal
//             if ( info != NULL )
//                 sprintf(names, "raw_events,test=%s,file=%d,name=data,va=%d,channel=%d,node=%d,node_name=%s,adc=%d",
//                         info, last_file, (15 - v), ((15 - v) * 64 + c), block->NodeAddress, node_names[block->NodeAddress], a);
//             else
//                 sprintf(names, "raw_events,file=%d,name=data,va=%d,channel=%d,node=%d,node_name=%s,adc=%d",
//                         last_file, (15 - v), ((15 - v) * 64 + c), block->NodeAddress, node_names[block->NodeAddress], a);
//             if ( ped_saved == 0 )
//                 sprintf(field, "raw=%f", d);
//             else
//                 sprintf(field, "raw=%f,diff=%f", d, (d - pedestal[15 - v][c]));
//             db_client_data(client, names, field, event_time, NO_DEBUG);
        }
    }

    // Save last event time
    if ( nevents < LEF_RAW_MAX ) {
        nevents++;
        event_time = utime;
        last_node  = block->NodeAddress;
    }
}

//----------------------------------------------------------
//  Function: decode_ground_test
//      Decode function for Ground Test Data
//
//  Inputs:
//      block ---- AMS block
//      utime ---- Time in microseconds
//      mode ----- USB box mode
//      info ----- test info to be added as tag
//
//  Return:
//      None
//----------------------------------------------------------
void decode_l0_upgrade(AMSBlock *block, uint64_t utime, const int mode, const char *info)
{
    // Datatypes
    switch ( block->DataType ) {
        // USB box read event reply
        case 0x13:
            decode_lef_raw_event(block, utime, mode, info);
            break;
    }
}
