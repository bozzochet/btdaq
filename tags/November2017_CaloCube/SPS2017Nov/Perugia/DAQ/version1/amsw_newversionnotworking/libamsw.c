/* -*-C-*-
 * libamsw.c - AMSWire PCI Board (version 1) library
 *
 *-----------------------------------------------------------------------
 *
 * Project          : Linux
 * Moduletyp        : C-Code
 * Author           : Xudong CAI
 * Created          : 05 Dec. 2001
 *
 *-----------------------------------------------------------------------
 */


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "amswire.h"
#include "amsw_func.h"
#include "amsw_protocol.h"

/************************************************************************/
/* Defines	                            							*/
/************************************************************************/

/************************************************************************/
/* Typedefs / Structs                                                   */
/************************************************************************/

/************************************************************************/
/* Globals                                                              */
/************************************************************************/

static int fd[4];

/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

/************************************************************************/
/* Locals                                                               */
/************************************************************************/

char *device_names[4] = {"/dev/amsw0", "/dev/amsw1", "/dev/amsw2", "/dev/amsw3"};
u_long xmit_addr[8] = {AMSW_BUFF_TX0, AMSW_BUFF_TX1, AMSW_BUFF_TX2, AMSW_BUFF_TX3,
			   AMSW_BUFF_TX4, AMSW_BUFF_TX5, AMSW_BUFF_TX6, AMSW_BUFF_TX7};
u_long recv_addr[8] = {AMSW_BUFF_RX0, AMSW_BUFF_RX1, AMSW_BUFF_RX2, AMSW_BUFF_RX3,
			   AMSW_BUFF_RX4, AMSW_BUFF_RX5, AMSW_BUFF_RX6, AMSW_BUFF_RX7};
u_long xmit_regs[8] = {AMSW_CSR_TX0, AMSW_CSR_TX1, AMSW_CSR_TX2, AMSW_CSR_TX3,
			   AMSW_CSR_TX4, AMSW_CSR_TX5, AMSW_CSR_TX6, AMSW_CSR_TX7};
u_long recv_regs[8] = {AMSW_CSR_RX0, AMSW_CSR_RX1, AMSW_CSR_RX2, AMSW_CSR_RX3,
			   AMSW_CSR_RX4, AMSW_CSR_RX5, AMSW_CSR_RX6, AMSW_CSR_RX7};

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_open - Open AMSWire device										*/
/*																*/
/* RETURNS:															*/
/* 	0 on sucess														*/
/************************************************************************/
int amsw_open(int card) {
	fd[card] = open(device_names[card],O_RDWR);
	return fd[card];
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_close - Close AMSWire device									*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
void amsw_close(int card) {
	close(fd[card]);
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_read_buffer - Read AMSWire buffer								*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_read_buffer(int card, u_long addr, void *buff, int nbytes) {
	int status;
	AMSW_ADDRS pa;

	/* Open device if device is not opened */
	if ( !fd[card] ) {
		fd[card] = open(device_names[card],O_RDWR);
		if ( fd[card] < 0 ) return fd[card];
	}

	/* Encode addresses */
	pa.pci_off = addr;
	pa.buff = (u_long *) buff;

	/* Read buffer */
	status = read(fd[card], (char *) &pa, (size_t) nbytes);
	return status;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_write_buffer - Write AMSWire buffer								*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_write_buffer(int card, u_long addr, void *buff, int nbytes) {
	int status;
	AMSW_ADDRS pa;

	/* Open device if device is not opened */
	if ( !fd[card] ) {
		fd[card] = open(device_names[card],O_RDWR);
		if ( fd[card] < 0 ) return fd[card];
	}

	/* Encode addresses */
	pa.pci_off = addr;
	pa.buff = (u_long *) buff;

	/* Write buffer */
	status = write(fd[card], (char *) &pa, nbytes);
	return status;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_write_register - Write AMSWire register							*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_write_register(int card, int reg, u_long data) {
	int status;

	/* Open device if device is not opened */
	if ( !fd[card] ) {
		fd[card] = open(device_names[card],O_RDWR);
		if ( fd[card] < 0 ) return fd[card];
	}

	/* Write register via IOCTL */
	status = ioctl(fd[card], reg | AMSW_CSR_SPACE | AMSW_IOCTL_WR, data);
	return status;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_read_register - Read AMSWire register							*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_read_register(int card, int reg, u_long *data) {
	int status;

	/* Open device if device is not opened */
	if ( !fd[card] ) {
		fd[card] = open(device_names[card],O_RDWR);
		if ( fd[card] < 0 ) return fd[card];
	}

	/* Read register via IOCTL */
	status = ioctl(fd[card], reg | AMSW_CSR_SPACE | AMSW_IOCTL_RD, (u_long) data);
	return status;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_write_dma_reg - Write AMSWire DMA register						*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_write_dma_reg(int card, int reg, u_long data) {
	int status;

	/* Open device if device is not opened */
	if ( !fd[card] ) {
		fd[card] = open(device_names[card],O_RDWR);
		if ( fd[card] < 0 ) return fd[card];
	}

	/* Write DMA register via IOCTL */
	status = ioctl(fd[card], reg | AMSW_DMA_SPACE | AMSW_IOCTL_WR, data);
	return status;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_read_dma_reg - Read AMSWire DMA register						*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_read_dma_reg(int card, int reg, u_long *data) {
	int status;

	/* Open device if device is not opened */
	if ( !fd[card] ) {
		fd[card] = open(device_names[card],O_RDWR);
		if ( fd[card] < 0 ) return fd[card];
	}

	/* Read DMA register via IOCTL */
	status = ioctl(fd[card], reg | AMSW_DMA_SPACE | AMSW_IOCTL_RD, (u_long) data);
	return status;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_command - send command to AMSWire device driver					*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_command(int card, u_long cmd, u_long data) {
	int status;

	/* Open device if device is not opened */
	if ( !fd[card] ) {
		fd[card] = open(device_names[card],O_RDWR);
		if ( fd[card] < 0 ) return fd[card];
	}

	/* Send command via IOCTL */
	status = ioctl(fd[card], cmd | AMSW_CMD_SPACE, data);
	return status;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_malloc - allocate kernel memory for user						*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
void * amsw_malloc(int card, int size) {
	void *mp;

	/* Open device if device is not opened */
	if ( !fd[card] ) {
		fd[card] = open(device_names[card],O_RDWR);
		if ( fd[card] < 0 ) return (void *) 0;
	}

	/* Allocate buffer via IOCTL */
	mp = (void *) size;
	if ( ioctl(fd[card], (u_long) (AMSW_CMD_MALLOC | AMSW_CMD_SPACE), (u_long) &mp) < 0 ) return (void *) 0;
	return mp;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_free - free kernel memory										*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
void amsw_free(int card, void *addr) {
	/* Free buffer via IOCTL */
	ioctl(fd[card], AMSW_CMD_SPACE | AMSW_CMD_FREE, (u_long) addr);
	return;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_get_status - return status according to the mask				*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_get_status(int card, u_long mask) {
	u_long status;

	/* Open device if device is not opened */
	if ( !fd[card] ) {
		fd[card] = open(device_names[card],O_RDWR);
		if ( fd[card] < 0 ) return (u_long) fd[card];
	}

	/* Get channel status via IOCTL */
	ioctl(fd[card], AMSW_CMD_SPACE | AMSW_CMD_NEW_STAT, &status);

	return ((status & mask) == mask ? 1 : 0);
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_clr_status - Clear status bits according to mask				*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_clr_status(int card, u_long mask) {
	int status;

	/* Open device if device is not opened */
	if ( !fd[card] ) {
		fd[card] = open(device_names[card],O_RDWR);
		if ( fd[card] < 0 ) return (u_long) fd[card];
	}

	/* Clear channel status via IOCTL */
	status = ioctl(fd[card], AMSW_CMD_SPACE | AMSW_CMD_CLR_STAT, mask);

	return status;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_cur_status - Get current status									*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_cur_status(int card, u_long *stat) {
	int status;

	/* Open device if device is not opened */
	if ( !fd[card] ) {
		fd[card] = open(device_names[card],O_RDWR);
		if ( fd[card] < 0 ) return (u_long) fd[card];
	}

	/* Clear channel status via IOCTL */
	status = ioctl(fd[card], AMSW_CMD_SPACE | AMSW_CMD_CUR_STAT, stat);

	return status;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_transmit_block - Transmit AMSWire block							*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/

#define AMSW_TX_TIMEOUT 100000

int amsw_transmit_block(int card, int chan, struct amsw_block *block) {
	u_char *buff;
	int status;
	u_long csr;
	int i;
	
	/* Wait until TX ready of this channel */
	for ( i = 0, status = 0; i < AMSW_TX_TIMEOUT && status == 0; i++ ) {
		status = amsw_get_status(card, AMSW_INT_TX_MASK << chan);
	}
	if ( status < 0 ) {
		printf("Error from <amsw_get_status>, error = %d\n", status);
		return status;
	}
	if ( i == AMSW_TX_TIMEOUT ) {
		amsw_cur_status(card, &csr);
		printf("Timeout on waiting TX ready, status = %08lx\n", csr);
		return -1;
	}

	/* Check size (only single packet is support) */
	if ( block->h_size + block->d_size > 8188 ) block->d_size = 8188 - block->h_size;

	/* Check header size for write transmit buffer */
	if ( block->h_size == 0 ) {
		/* Write header into transmit buffer */
		if ( (status = amsw_write_buffer(card, xmit_addr[chan], block->data, block->d_size)) < 0 ) {
			printf("Error from <amsw_write_buffer>, error = %d\n", status);
			return status;
		}
	}
	else if ( block->d_size == 0 ) {
		/* Write header into transmit buffer */
		if ( (status = amsw_write_buffer(card, xmit_addr[chan], block->header, block->h_size)) < 0 ) {
			printf("Error from <amsw_write_buffer>, error = %d\n", status);
			return status;
		}
	}
	else if ( block->h_size % 4 != 0 ) {
		/* Allocate buffer */
		buff = (u_char *) malloc(block->h_size + block->d_size);
		if ( !buff ) return -1;

		/* Combine header and data buffers */
		memcpy(buff, block->header, block->h_size);
		memcpy(&buff[block->h_size], block->data, block->d_size);

		/* Write header and data into transmit buffer */
		if ( (status = amsw_write_buffer(card, xmit_addr[chan], buff, block->h_size + block->d_size)) < 0 ) {
			printf("Error from <amsw_write_buffer>, error = %d\n", status);
			return status;
		}

		/* Free buffer */
		free(buff);
	}
	else {
		/* Write header into transmit buffer */
		if ( (status = amsw_write_buffer(card, xmit_addr[chan], block->header, block->h_size)) < 0 ) {
			printf("Error from <amsw_write_buffer>, error = %d\n", status);
			return status;
		}

		/* Write data into transmit buffer */
		if ( (status = amsw_write_buffer(card, xmit_addr[chan] + block->h_size, block->data, block->d_size)) < 0 ) {
			printf("Error from <amsw_write_buffer>, error = %d\n", status);
			return status;
		}
	}

	/* Write TX register to start transmit */
	csr = ((block->h_size + block->d_size) << AMSW_TX_SIZE_SHFT) | AMSW_PCK_SINGLE | AMSW_TX_START;
	if ( (status = amsw_write_register(card, xmit_regs[chan], csr)) < 0 ) {
		printf("Error from <amsw_write_register>, error = %d\n", status);
		return status;
	}

	return 0;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_receive_block - Read AMSWire buffer								*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/

#define AMSW_RX_TIMEOUT 1000000

int amsw_receive_block(int card, int chan, struct amsw_block *block) {
	int status;
	int i;
	u_long csr;

	/* Wait for data received in this channel */
	for ( i = 0, status = 0; i < AMSW_RX_TIMEOUT && status == 0; i++ ) {
		status = amsw_get_status(card, AMSW_INT_RX_MASK << chan);
	}
	if ( status < 0 ) {
		printf("Error from <amsw_get_status>, error = %d\n", status);
		return status;
	}
	if ( i == AMSW_RX_TIMEOUT ) {
		amsw_cur_status(card, &csr);
		printf("Timeout on waiting RX done, status = 0x%08lx\n", csr);
		return -1;
	}

	/* Read channel RX register */
	if ( (status = amsw_read_register(card, recv_regs[chan], &csr)) < 0 ) {
		printf("Error from <amsw_read_register>, error = %d\n", status);
		return status;
	}

	/* Check run and overrun bits */
	if ( (csr & AMSW_RX_OVERRUN) || (csr & AMSW_RX_RUN) ) {
		printf("Unexpected packet comes, RX register = 0x%08lx\n", csr);
		return -1;
	}

	/* Check error */
	if ( csr & AMSW_RX_ERR_MASK ) {
		printf("Error on receiving, RX register = 0x%08lx\n", csr);
		return -1;
	}

	/* Check Packet type */
	if ( (csr & AMSW_PCK_MASK) != AMSW_PCK_SINGLE ) {
		printf("Current version is support single packet only. RX register = 0x%08lx\n", csr);
		return -1;
	}

	/* Decode length from RX register */
	block->d_size = (csr & AMSW_RX_SIZE_MASK) >> AMSW_RX_SIZE_SHFT;

	/* Read buffer */
	status = amsw_read_buffer(card, recv_addr[chan], block->data, block->d_size);

	/* Clear status */
	if ( (status = amsw_clr_status(card, AMSW_INT_CH_MASK << chan)) < 0 ) {
		printf("Error from <amsw_clr_status>, error = %d\n", status);
		return status;
	}

	return status;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_tx_ready - Check TX ready status								*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_tx_ready(int card, int chan) {
	int status;

	/* Wait for data received in this channel */
	status = amsw_get_status(card, AMSW_INT_TX_MASK << chan);
	if ( status < 0 ) {
		printf("Error from <amsw_get_status>, error = %d\n", status);
		return 0;
	}

	return status;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_rx_done - Check RX done status									*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_rx_done(int card, int chan) {
	int status;

	/* Wait for data received in this channel */
	status = amsw_get_status(card, AMSW_INT_RX_MASK << chan);
	if ( status < 0 ) {
		printf("Error from <amsw_get_status>, error = %d\n", status);
		return 0;
	}

	return status;
}
