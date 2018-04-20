/* -*-C-*-
 * amsw_func.h - Header file to define the functions in AMSWire operation library
 *
 *-----------------------------------------------------------------------
 *
 *
 *-----------------------------------------------------------------------
 */

#ifndef __amsw_func_h
#define __amsw_func_h

#include "amsw_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************/
/* function declarations							*/
/****************************************************/

int amsw_open(int card);
void amsw_close(int card);

int amsw_write_register(int card, int reg, u_long data);
int amsw_read_register(int card, int reg, u_long *data);

int amsw_read_dma_reg(int card, int reg, u_long *data);
int amsw_write_dma_reg(int card, int reg, u_long data);

int amsw_command(int card, u_long cmd, u_long data);

int amsw_read_buffer(int card, u_long addr, void *buff, int nbytes);
int amsw_write_buffer(int card, u_long addr, void *buff, int nbytes);

void * amsw_malloc(int card, int size);
void amsw_free(int card, void *addr);

int amsw_get_status(int card, u_long mask);
int amsw_clr_status(int card, u_long mask);

int amsw_transmit_block(int card, int chan, struct amsw_block *block);
int amsw_receive_block(int card, int chan, struct amsw_block *block);

int amsw_tx_ready(int card, int chan);
int amsw_rx_done(int card, int chan);

#if 0
int amsw_enable_irq(void);
int amsw_disable_irq(void);
int amsw_enable_dma(void);
int amsw_disable_dma(void);

int amsw_write_tx_buffer(int channel, u_char *buff, size_t nbytes);
int amsw_read_tx_buffer(int channel, u_char *buff, size_t nbytes);
int amsw_read_rx_buffer(int channel, u_char *buff, size_t nbytes);

int amsw_start_tx(int channel, size_t nbytes, u_long mode);
int amsw_start_rx(int channel);

int amsw_reg_put_ctrl(int channel, u_long data);
int amsw_reg_get_ctrl(int channel, u_long *data);
int amsw_reg_get_stat(int channel, u_long *data);
int amsw_reg_set_ctrl(int channel, u_long mask);
int amsw_reg_clr_ctrl(int channel, u_long mask);

int amsw_tx_done(int channel);
int amsw_rx_done(int channel);
int amsw_overrun(int channel);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __amsw_func_h */
