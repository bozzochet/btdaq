/* -*-C-*-
 * amsw_lib.h - Header file for BOARD6 support library
 *
 *-----------------------------------------------------------------------
 *
 *
 *-----------------------------------------------------------------------
 */

#ifndef __amsw_lib_h
#define __amsw_lib_h

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************
	Definitions
 ****************************************************/

#define AMSW_MEM_SIZE		256 * 1024			/* 256 Kbytes */
#define AMSW_REG_SIZE		256				/* 256 bytes */

/****************************************************
	function declarations
 ****************************************************/

int amsw_open(int card);
void amsw_close(int card);
int amsw_mem_read(int card, u_long pci, void *buff, int size);
int amsw_mem_write(int card, u_long pci, void *buff, int size);
int amsw_reg_read(int card, int reg, u_long *data);
int amsw_reg_write(int card, int reg, u_long data);
int amsw_dma_read(int card, int addr, u_long *data);
int amsw_dma_write(int card, int addr, u_long data);
void *amsw_malloc(int card, int size);
void amsw_free(int card, void *addr);
void *amsw_mmap_bar0(int card);
void amsw_munmap_bar0(int card, void *addr);
void *amsw_mmap_bar1(int card);
void amsw_munmap_bar1(int card, void *addr);
void amsw_init(int card);
int amsw_tx_ready(int link);
int amsw_rx_ready(int link);
int amsw_tx_packet(int link, const void *data, int size, int bc);
int amsw_rx_packet(int link, void *data, int size, int *bc);

/****************************************************
	typedefs / structs
 ****************************************************/

typedef struct {
	int			chan;			/* TX and RX channel number */
	u_long			csr;			/* CSR for TX and RX */
	int			size;			/* size in bytes */
	u_long			data;			/* Buffer address */
	u_long			status;			/* Status for RX */
	u_long	 		pci_off;		/* PCI offset (memory access only) */
} AMSW_DATA;	

typedef struct {
	int			mask;			/* Channel mask for which got RX */
	int			size[8];		/* size of each received channel */
} AMSW_POLL;

typedef struct {
	u_long	start:		1;		/* Start TX */
	u_long	:		14;		/* unused bits */
	u_long	size:		13;		/* Size in bytes */
	u_long	:		2;		/* unused bits */
	u_long	bc:		2;		/* block control: 01 - first, 10 - last, 11 - continue, 00 - single */
} AMSWTXCtrlWord;

typedef struct {
	u_long	:		4;		/* unused bits */
	u_long	done:		1;		/* RX done */
	u_long	overrun:	1;		/* RX overrun */
	u_long	run:		1;		/* RX running */
	u_long	:		1;		/* unused bits */
	u_long	error:		7;		/* errors */
	u_long	size:		13;		/* size in bytes */
	u_long	:		2;		/* unused bits */
	u_long	bc:		2;		/* block control: 01 - first, 10 - last, 11 - continue, 00 - single */
} AMSWRXCtrlWord;

/****************************************************
	End
 ****************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __amsw_lib_h */
