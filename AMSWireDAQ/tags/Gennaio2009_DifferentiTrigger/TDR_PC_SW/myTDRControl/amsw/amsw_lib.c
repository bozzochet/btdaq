/* -*-C-*-
 * amsw_lib.c - HRDL PMC board (board 6) library routines
 *-----------------------------------------------------------------------
 *
 * Project          : Linux
 * Moduletyp        : C-Code
 * Author           : Xudong CAI
 * Created          : 10 September 2002
 *
 *-----------------------------------------------------------------------
 */

/************************************************************************
   Includes
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>

#include "amsw_dev.h"
#include "amsw_lib.h"

/************************************************************************
   Defines
 ************************************************************************/

/************************************************************************
   Typedefs / Structs
 ************************************************************************/

typedef struct {
	int		card;			/* Card number */
	int		chan;			/* Channel number on card */
} AMSWLink;

/************************************************************************
   Globals
 ************************************************************************/

static int fd[4] = {-1, -1, -1, -1};
static char *names[] = {"/dev/amsw0", "/dev/amsw1",
						"/dev/amsw2", "/dev/amsw3"};
static AMSWLink links[32] = {
	{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7},
	{1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7},
	{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6}, {2, 7},
	{3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7},
};

/************************************************************************
   Forward declarations
 ************************************************************************/

/************************************************************************
   Locals
 ************************************************************************/

/************************************************************************
	Function:
		amsw_open - open AMSW-PCI device

	INPUTS
		card ---- card number (0 - 3), maximum support up to 4 cards

   	RETURNS:
 		= 0 on success
		< 0 error code
 ************************************************************************/
int amsw_open(int card) {
	int n = card % 4;

	if ( (fd[n] = open(names[n], O_RDWR)) >= 0 ) return 0;

	return fd[n];
}

/************************************************************************
	Function:
		amsw_close - close AMSW-PCI device

	INPUTS
		card ---- card number (0 - 3), -1 means all

	RETURNS:
		NONE
 ************************************************************************/
void amsw_close(int card) {
	int n;

	if ( card == -1 ) {
		for ( n = 0; n < 4; n++ ) {
			if ( fd[n] < 0 ) continue;
			close(fd[n]);
			fd[n] = -1;
		}
	}
	else {
		n = card % 4;
		close(fd[n]);
		fd[n] = -1;
	}

	return;
}

/************************************************************************
	Function:
		amsw_mem_read - read AMSW-PCI dual-port memory

	INPUTS
		card ---- card number (0 - 3)
		pci ----- PCI offset to be read
		buff ---- destination buffer to put data read
		size ---- size in bytes to be read

	RETURNS:
		>= 0 size read
		<  0 error code
 ************************************************************************/
int amsw_mem_read(int card, u_long pci, void *buff, int size) {
	int n = card % 4;
	AMSW_DATA d;
	int status;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return fd[n];

	/* Prepare for read */
	d.pci_off = pci;
	d.data = (u_long) buff;
	d.size = size;

	/* read data */
	status = read(fd[n], (void *) &d, size);
	if ( status < 0 ) return status;

	return size;
}

/************************************************************************
	Function:
		amsw_mem_write - write AMSW-PCI dual-port memory

	INPUTS
		card ---- card number (0 - 3)
		pci ----- PCI offset to be read
		buff ---- source buffer to get data
		size ---- size in bytes to be write

	RETURNS:
		>= 0 size read
		<  0 error code
 ************************************************************************/
int amsw_mem_write(int card, u_long pci, void *buff, int size) {
	int n = card % 4;
	AMSW_DATA d;
	int status;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return fd[n];

	/* Prepare for read */
	d.pci_off = pci;
	d.data = (u_long) buff;
	d.size = size;

	/* read data */
	status = write(fd[n], (void *) &d, size);
	if ( status < 0 ) return status;

	return size;
}

/************************************************************************
	Function:
		amsw_reg_read - read AMSW-PCI firmware registers

	INPUTS
		card ---- card number (0 - 3)
		reg ----- register number (offset >> 2) should be less than 256.
		data ---- destination addr to put data read

	RETURNS:
		= 0 on success
		< 0 error code
 ************************************************************************/
int amsw_reg_read(int card, int reg, u_long *data) {
	int n = card % 4;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return fd[n];

	/* Read register */
	return ioctl(fd[n], AMSW_IOC_GET_REG | (reg & 0xff), (u_long) data);
}

/************************************************************************
	Function:
		amsw_reg_write - write AMSW-PCI firmware registers

	INPUTS
		card ---- card number (0 - 3)
		reg ----- register number (offset >> 2) should be less than 256.
		data ---- data to be written

	RETURNS:
		= 0 on success
		< 0 error code
 ************************************************************************/
int amsw_reg_write(int card, int reg, u_long data) {
	int n = card % 4;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return fd[n];

	/* Write register */
	return ioctl(fd[n], AMSW_IOC_SET_REG | (reg & 0xff), data);
}

/************************************************************************
	Function:
		amsw_dma_read - read AMSW-PCI configuration space

	INPUTS
		card ---- card number (0 - 3)
		addr ---- offset (should be less than 256)
		data ---- destination addr to put data read

	RETURNS:
		= 0 on success
		< 0 error code
 ************************************************************************/
int amsw_dma_read(int card, int addr, u_long *data) {
	int n = card % 4;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return fd[n];

	/* Read register */
	return ioctl(fd[n], AMSW_IOC_GET_DMA | (addr & 0xff), (u_long) data);
}

/************************************************************************
	Function:
		amsw_dma_write - write AMSW-PCI configuration space

	INPUTS
		card ---- card number (0 - 3)
		addr ---- offset (should be less than 256)
		data ---- data to be written

	RETURNS:
		= 0 on success
		< 0 error code
 ************************************************************************/
int amsw_dma_write(int card, int addr, u_long data) {
	int n = card % 4;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return fd[n];

	/* Write register */
	return ioctl(fd[n], AMSW_IOC_SET_DMA | (addr & 0xff), data);
}

/************************************************************************
	Function:
		amsw_malloc - allocate kernel buffer for application
					  the buffer can be used directly by DMA transfer

	INPUTS
		card ---- card number (0 - 3)
		size ---- size in bytes to be allocated

	RETURNS:
		!= 0 user space address returned
		=  0 on error
 ************************************************************************/
void *amsw_malloc(int card, int size) {
	int n = card % 4;
	int status;
	u_long data;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return NULL;

	/* allocate memory */
	data = (u_long) size;
	status = ioctl(fd[n], AMSW_IOC_KMALLOC, (u_long) &data);

	/* Check result */
	if ( status < 0 ) return NULL;

	return (void *) data;
}

/************************************************************************
	Function:
		amsw_free - free kernel buffer allocated for application

	INPUTS
		card ---- card number (0 - 3)
		addr ---- address to be freed

	RETURNS:
		NONE
 ************************************************************************/
void amsw_free(int card, void *addr) {
	int n = card % 4;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return;

	/* free memory */
	ioctl(fd[n], AMSW_IOC_KFREE, (u_long) addr);

	return;
}

/************************************************************************
	Function:
		amsw_mmap_bar0 - map AMSW-PCI BAR0 address to user space

	INPUTS
		card ---- card number (0 - 3)

	RETURNS:
		>  0 address mapped
		<= 0 error code
 ************************************************************************/
void *amsw_mmap_bar0(int card) {
	int n = card % 4;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return (void *) fd[n];

	return mmap(0, AMSW_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd[n], 0xba000000);
}

/************************************************************************
	Function:
		amsw_munmap_bar0 - unmap AMSW-PCI BAR0 from user space

	INPUTS
		card ---- card number (0 - 3)
		addr ---- address to be unmapped

	RETURNS:
		NONE
 ************************************************************************/
void amsw_munmap_bar0(int card, void *addr) {
	int n = card % 4;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return;

	munmap(addr, AMSW_MEM_SIZE);
}

/************************************************************************
	Function:
		amsw_mmap_bar1 - map AMSW-PCI BAR1 address to user space

	INPUTS
		card ---- card number (0 - 3)

	RETURNS:
		>  0 address mapped
		<= 0 error code
 ************************************************************************/
void *amsw_mmap_bar1(int card) {
	int n = card % 4;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return (void *) fd[n];

	return mmap(0, AMSW_REG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd[n], 0xba100000);
}

/************************************************************************
	Function:
		amsw_munmap_bar1 - unmap AMSW-PCI BAR1 from user space

	INPUTS
		card ---- card number (0 - 3)
		addr ---- address to be unmapped

	RETURNS:
		NONE
 ************************************************************************/
void amsw_munmap_bar1(int card, void *addr) {
	int n = card % 4;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return;

	munmap(addr, AMSW_REG_SIZE);
}

/************************************************************************
	Function:
		amsw_init - init AMSWire driver parameters

	INPUTS
		card ---- card number (0 - 3)

	RETURNS:
		NONE
 ************************************************************************/
void amsw_init(int card) {
	int n = card % 4;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return;

	ioctl(fd[n], AMSW_IOC_INIT, 0);
}

/************************************************************************
	Function:
		amsw_tx_ready - check TX channel ready status

	INPUTS
		link ---- link number (0 - 31)

	RETURNS:
		= 0 : not ready
		> 0 : ready
		< 0 : error code
 ************************************************************************/
int amsw_tx_ready(int link) {
	int mask;
	int n = links[link].card;
	int c = links[link].chan;
	int ret;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return -1;

	/* Poll TX status */
	ret = ioctl(fd[n], AMSW_IOC_POLL_TX, (u_long) &mask);
	if ( ret < 0 ) return ret;

	/* Check mask */
	if ( mask & (1 << c) ) return 1;

	return 0;
}

/************************************************************************
	Function:
		amsw_rx_ready - Check if RX link got data

	INPUTS
		link ---- link number (0 - 31)

	RETURNS:
		 < 0 : error code, no data
		>= 0 : data size
 ************************************************************************/
int amsw_rx_ready(int link) {
	AMSW_POLL poll;
	int n = links[link].card;
	int c = links[link].chan;
	int ret;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return -1;

	/* Poll RX status */
	ret = ioctl(fd[n], AMSW_IOC_POLL_RX, (u_long) &poll);
	if ( ret < 0 ) return ret;

	/* check mask */
	if ( poll.mask & (1 << c) ) return poll.size[c];

	return -ENODATA;
}

/************************************************************************
	Function:
		amsw_tx_packet - Transmit TX packet

	INPUTS
		link ---- link number (0 - 31)
		data ---- point to data to be sent
		size ---- size of data in bytes
		bc ------ block control (01 - first, 10 - last, 11 - continue, 00 - single)

	RETURNS:
		= 0 : success
		< 0 : error code
 ************************************************************************/
int amsw_tx_packet(int link, const void *data, int size, int bc) {
	AMSW_DATA d;
	AMSWTXCtrlWord *ctrl = (AMSWTXCtrlWord *) &d.csr;
	int n = links[link].card;
	int c = links[link].chan;
	int ret;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return -1;

	/* Prepare data */
	d.chan = c;
	d.data = (u_long) data;
	d.size = size;

	/* Prepare control word */
	d.csr  = 0;
	ctrl->size  = size;
	ctrl->bc    = (bc & 3);
	ctrl->start = 1;

	/* Send TX data */
	ret = ioctl(fd[n], AMSW_IOC_TX, (u_long) &d);
	if ( ret < 0 ) return ret;

	return 0;
}

/************************************************************************
	Function:
		amsw_rx_packet - Receive RX packet

	INPUTS
		link ---- link number (0 - 31)
		data ---- Point to data to be stored
		size ---- size to get in bytes
		bc ------ Block control returned

	RETURNS:
		= 0 : success
		= 1 : RX warning
		= 2 : RX error
		< 0 : error code for failed operation
 ************************************************************************/
int amsw_rx_packet(int link, void *data, int size, int *bc) {
	AMSW_DATA d;
	AMSWTXCtrlWord *ctrl = (AMSWTXCtrlWord *) &d.csr;
	int n = links[link].card;
	int c = links[link].chan;
	int ret;

	/* check open */
	if ( fd[n] < 0 ) amsw_open(n);
	if ( fd[n] < 0 ) return -1;

	/* Prepare to store data */
	d.chan = c;
	d.data = (u_long) data;
	d.size = size;

	/* Get RX data */
	ret = ioctl(fd[n], AMSW_IOC_RX, (u_long) &d);
	if ( ret < 0 ) return ret;

	/* Get block control info */
	*bc = ctrl->bc;

	return d.status;
}
