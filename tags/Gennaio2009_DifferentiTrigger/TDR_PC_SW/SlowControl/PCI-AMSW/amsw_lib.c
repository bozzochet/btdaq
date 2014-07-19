// file amsw_lib.c
//
// HRDL PMC board (board 6) library routines
//
// Stolen from XC and reworked
//
// A.Lebedev Nov-2006...
//

//~-- Includes

#include "mylib.h"
#include "amsw_dev.h"
#include "amsw_lib.h"

//~-- Globals

static int fd[4] = {
  -1, -1, -1, -1};
  
static char *names[4] = {
  "/dev/amsw0", "/dev/amsw1", "/dev/amsw2", "/dev/amsw3"};

typedef struct {
	int card;          // Card number
	int chan;          // Channel number on card
} AMSWLink;

static AMSWLink links[32] = {
	{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7},
	{1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7},
	{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6}, {2, 7},
	{3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7},
};


//~----------------------------------------------------------------------------

int amsw_open(int card) {

	int n = card % 4;

	if ((fd[n] = open(names[n], O_RDWR)) >= 0) return 0;

	return fd[n];
}

//~----------------------------------------------------------------------------

void amsw_close(int card) {

	int n;

	if (card == -1) {
		for (n = 0; n < 4; n++) {
			if (fd[n] < 0) continue;
			close(fd[n]);
			fd[n] = -1;
		}
	}
	else {
		n = card % 4;
		close(fd[n]);
		fd[n] = -1;
	}
}

//~----------------------------------------------------------------------------

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
  printf("amsw_mem_read: fd[%d] = %d\n", n, fd[n]);

	/* Prepare for read */
	d.pci_off = pci;
	d.data = (u_long) buff;
	d.size = size;

	/* read data */
	status = read(fd[n], (void *) &d, size);
  printf("amsw_mem_read: status = %d\n", status);
	if ( status < 0 ) return status;

	return size;
}

//~----------------------------------------------------------------------------

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

	/* Prepare for write */
	d.pci_off = pci;
	d.data = (u_long) buff;
	d.size = size;

	/* read write */
	status = write(fd[n], (void *) &d, size);
	if ( status < 0 ) return status;

	return size;
}

//~----------------------------------------------------------------------------

int amsw_reg_read(int card, int reg, u_long *data) {

	int n = card % 4;

	if (fd[n] < 0) amsw_open(n);
	if (fd[n] < 0) return fd[n];

	return ioctl(fd[n], AMSW_IOC_GET_REG | (reg & 0xff), (u_long)data);
}

//~----------------------------------------------------------------------------

int amsw_reg_write(int card, int reg, u_long data) {

	int n = card % 4;

	if (fd[n] < 0) amsw_open(n);
	if (fd[n] < 0) return fd[n];

	return ioctl(fd[n], AMSW_IOC_SET_REG | (reg & 0xff), data);
}

//~----------------------------------------------------------------------------
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

//~----------------------------------------------------------------------------

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

//~----------------------------------------------------------------------------

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

//~----------------------------------------------------------------------------

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

//~----------------------------------------------------------------------------

void amsw_init(int card) {

	int n = card % 4;

	if (fd[n] < 0) amsw_open(n);
	if (fd[n] < 0) return;

	ioctl(fd[n], AMSW_IOC_INIT, 0);
}

//~----------------------------------------------------------------------------

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

	if (fd[n] < 0) amsw_open(n);
	if (fd[n] < 0) return -1;

	ret = ioctl(fd[n], AMSW_IOC_POLL_TX, (u_long)&mask);
	if (ret < 0) return ret;

	if (mask & (1 << c)) return 1;

	return 0;
}

//~----------------------------------------------------------------------------

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

	if (fd[n] < 0) amsw_open(n);
	if (fd[n] < 0) return -1;

	ret = ioctl(fd[n], AMSW_IOC_POLL_RX, (u_long)&poll);
	if (ret < 0) return ret;

	if (poll.mask & (1 << c)) return poll.size[c];

	return -ENODATA;
}

//~----------------------------------------------------------------------------

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
	AMSWTXCtrlWord *ctrl = (AMSWTXCtrlWord*)&d.csr;
	int n = links[link].card;
	int c = links[link].chan;
	int ret;

	if (fd[n] < 0) amsw_open(n);
	if (fd[n] < 0) return -1;

	d.chan = c;
	d.data = (u_long)data;
	d.size = size;

	d.csr  = 0;
	ctrl->size  = size;
	ctrl->bc    = (bc & 3);
	ctrl->start = 1;

	ret = ioctl(fd[n], AMSW_IOC_TX, (u_long)&d);
	if (ret < 0) return ret;

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
