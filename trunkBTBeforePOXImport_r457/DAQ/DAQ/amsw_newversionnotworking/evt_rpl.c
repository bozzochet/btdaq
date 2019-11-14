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
	u_long uStatus, uData, uBuff;
	u_short uRxSuccess, uMask, uNum;
	/* Open the PCI devices. */
	printf("Opening devices...\n");
	amsw_open(iCard);
	/* Check DONE and RUN. */
	printf("Waiting for RX done...\n");
	while (1)
		if (amsw_read_register(iCard, AMSW_CSR_INT, &uStatus) < 0)
		{
			printf("amsw_read_register error!\n");
			amsw_close(iCard);
			exit(1);
		}
		else if ((uStatus & 0x000000FF) != 0 && (uStatus & 0x0000FF00) != 0x0000FF00)
		{
			for (uNum = 0, uMask = 1; uNum < 8; uNum ++, uMask <<= 1)
				if ((uStatus & (uMask | (uMask << 8))) == uMask)
				{
					amsw_read_register(iCard, AMSW_CSR_RX1, &uData);
					if ((uData & 0x00003F00) == 0)
					{
						amsw_read_buffer(iCard, AMSW_BUFF_RX1, &uBuff, (uData & AMSW_RX_SIZE_MASK) >> 16);
						printf("RX Status: %08X\n", uData);
						printf("Card %d INT_Status: 0x%08X\nPackage successfully received @ RX%d: %08X\n", iCard, uStatus, uNum, uBuff);
					}
//			break;
				}
		}
//	if ((uTxDone & 0x00070000) == 0x00070000)
//	{
		/* Fill the TX buffer. */
//		printf("Writing 0x2E01 to register AMSW_BUFF_TX1...\n");
//		uBuff = 0x2E01;
//		amsw_write_buffer(iCard, AMSW_BUFF_TX1, &uBuff, 2);
//		amsw_read_buffer(iCard, AMSW_BUFF_TX1, &uBuff, 2);
//		printf("AMSW_BUFF_TX1: 0x%04X\n", uBuff);
		/* Write packet type and length in the TX register. */
//		printf("Writing packet type & length to register AMSW_CSR_TX1...\n");
//		amsw_write_register(iCard, AMSW_CSR_TX1, AMSW_TX_BC1 | AMSW_TX_BC2 | ((4 << AMSW_TX_SIZE_SHFT) & AMSW_TX_SIZE_MASK));
//		amsw_read_register(iCard, AMSW_CSR_TX1, &uData);
//		printf("AMSW_CSR_TX1: 0x%08X\n", uData);
		/* Assert START to start the data transmission. */
//		printf("Asserting START...\n");
//		amsw_write_register(iCard, AMSW_CSR_TX1, AMSW_TX_START | AMSW_TX_BC1 | AMSW_TX_BC2 | ((4 << AMSW_TX_SIZE_SHFT) & AMSW_TX_SIZE_MASK));
//		amsw_read_register(iCard, AMSW_CSR_TX1, &uData);
//		printf("AMSW_CSR_TX1: 0x%08X\n", uData);
		/* Close the PCI devices. */
//		printf("Closing devices...\n");
//		amsw_close(iCard);
//	}
//	sleep(1);
	return 0;
}
