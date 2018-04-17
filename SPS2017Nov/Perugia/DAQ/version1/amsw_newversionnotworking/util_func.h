/* -*-C-*-
 * util_func.h - Header file for utility routines library
 *
 *-----------------------------------------------------------------------
 *
 *
 *-----------------------------------------------------------------------
 */

#ifndef __util_func_h
#define __util_func_h

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************/
/* function declarations							*/
/****************************************************/

void crc16_table_init(void);
u_short crc16(u_char *data, size_t nbytes);
u_long crc16_long(u_long *data, size_t nwords);

void fill_data(void *buff, int nbytes, int mode, u_long pattern);

#ifdef __cplusplus
}
#endif

#endif /* __util_func_h */
