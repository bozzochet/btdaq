#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>
#include <sys/mman.h>
#include <signal.h>

#include "amswire.h"
#include "amsw_func.h"
#include "util_func.h"
#include "amsw_protocol.h"

int main(int args, char *argv[]) {
	int iCard = 0;
	u_long uTxDone, uData;
	u_short uBuff;
	/* Open the PCI devices. */
	printf("Opening devices...\n");
	amsw_open(iCard);
	/* Check TX DONE. Its value shall be '1' when TX is ready. */
	printf("Reading register AMSW_CSR_INT...\n");
	if (amsw_read_register(iCard, AMSW_CSR_INT, &uTxDone) < 0)
	{
		printf("amsw_read_register error!\n");
		amsw_close(iCard);
		exit(1);
	}
	printf("Card %d INT_Status: 0x%08X\n", iCard, uTxDone);
	if ((uTxDone & 0x00070000) == 0x00070000)
	{
		/* Fill the TX buffer. */
		printf("Writing 0x2E01 to register AMSW_BUFF_TX1...\n");
		uBuff = 0x2E01;
		amsw_write_buffer(iCard, AMSW_BUFF_TX1, &uBuff, 2);
		amsw_read_buffer(iCard, AMSW_BUFF_TX1, &uBuff, 2);
		printf("AMSW_BUFF_TX1: 0x%04X\n", uBuff);
		/* Write packet type and length in the TX register. */
		printf("Writing packet type & length to register AMSW_CSR_TX1...\n");
		amsw_write_register(iCard, AMSW_CSR_TX1, AMSW_TX_BC1 | AMSW_TX_BC2 | ((4 << AMSW_TX_SIZE_SHFT) & AMSW_TX_SIZE_MASK));
		amsw_read_register(iCard, AMSW_CSR_TX1, &uData);
		printf("AMSW_CSR_TX1: 0x%08X\n", uData);
		/* Assert START to start the data transmission. */
		printf("Asserting START...\n");
		amsw_write_register(iCard, AMSW_CSR_TX1, AMSW_TX_START | AMSW_TX_BC1 | AMSW_TX_BC2 | ((4 << AMSW_TX_SIZE_SHFT) & AMSW_TX_SIZE_MASK));
		amsw_read_register(iCard, AMSW_CSR_TX1, &uData);
		printf("AMSW_CSR_TX1: 0x%08X\n", uData);
		/* Close the PCI devices. */
		printf("Closing devices...\n");
		amsw_close(iCard);
	}
	sleep(1);
	return 0;
}
