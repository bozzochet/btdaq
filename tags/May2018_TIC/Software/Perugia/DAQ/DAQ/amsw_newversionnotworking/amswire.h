/* -*-C-*-
 * amswire.h - header file for AMSWire PCI Board device related parameters
 *
 *-----------------------------------------------------------------------
 *
 *
 *-----------------------------------------------------------------------
 */

#ifndef __amswire_h
#define __amswire_h

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************/
/* General                                          */
/****************************************************/

#define AMSW_MAX_NCARDS		4
#define AMSW_LATENCY_TIMER	0x80

/****************************************************/
/* Version Code                                     */
/****************************************************/
#define AMSW_AVER_SPACE		3 << 8			/* number of base addresses, 00 means 4 */
#define AMSW_AVER_IRQ		1 << 10			/* 1 - With IRQ, 0 - Without IRQ */
#define AMSW_AVER_DMA		1 << 11			/* 1 - With DMA, 0 - Without DMA */
#define AMSW_AVER_FLIGHT	1 << 15			/* 1 - Flight version, 0 - Prototype */

#define AMSW_XVER_CHN		1 << 31			/* 1 - 8 channels, 0 - 10 channels */
//#define AMSW_XVER_ENDIAN	1 << 30			/* 1 - Big endian, 0 - little endian */

/****************************************************/
/* IOCTL Operations                                 */
/****************************************************/
#define AMSW_RW_MASK		3 << 24
#define AMSW_IOCTL_RD		1 << 24
#define AMSW_IOCTL_WR		1 << 25

#define AMSW_SPACE_MASK		15 << 28
#define AMSW_CSR_SPACE		1 << 28
#define AMSW_DMA_SPACE		1 << 29
#define AMSW_CMD_SPACE		1 << 30

/****************************************************/
/* DMA Control Registers Address                    */
/****************************************************/
#define AMSW_DMA_OFFSET		0x40
#define AMSW_DMA_PCI_ADDR	0x10		/* was 0x40 */
#define AMSW_DMA_LOC_ADDR	0x11		/* was 0x44 */
#define AMSW_DMA_CTRL		0x12		/* was 0x48 */

/****************************************************/
/* DMA Control Bits                                 */
/****************************************************/
#define AMSW_DMA_RD			0x30			/* From PCI board to Computer */
#define AMSW_DMA_WR			0x38			/* From Computer to PCI board */
#define AMSW_DMA_TIMEOUT	1000000			/* Timeout counter */
#define AMSW_DMA_DONE		4
#define AMSW_DMA_ERROR		3
#define AMSW_DMA_CHECK		7

#define AMSW_DMA_SIZE		4096			/* Limitation of current Actel DMA core */

/****************************************************/
/* IOCTL Commands                                   */
/****************************************************/
#define AMSW_CMD_EIRQ_ALL	1
#define AMSW_CMD_DIRQ_ALL	2
#define AMSW_CMD_EIRQ_RX	3
#define AMSW_CMD_DIRQ_RX	4
#define AMSW_CMD_EIRQ_DMA	5
#define AMSW_CMD_DIRQ_DMA	6
#define AMSW_CMD_SET_MASK	7
#define AMSW_CMD_NEW_STAT	8

#define AMSW_CMD_ENA_DMA	20
#define AMSW_CMD_DIS_DMA	21
#define AMSW_CMD_INUSE		22
#define AMSW_CMD_BAR0		23
#define AMSW_CMD_BAR1		24
#define AMSW_CMD_MALLOC		25
#define AMSW_CMD_FREE		26
#define AMSW_CMD_CLR_STAT	27
#define AMSW_CMD_CUR_STAT	28

/****************************************************/
/* Control & Status Registers (CSR) addresses       */
/****************************************************/
#define AMSW_CSR_TX0		0
#define AMSW_CSR_RX0		4 >> 2
#define AMSW_CSR_TX1		8 >> 2
#define AMSW_CSR_RX1		12 >> 2
#define AMSW_CSR_TX2		16 >> 2
#define AMSW_CSR_RX2		20 >> 2
#define AMSW_CSR_TX3		24 >> 2
#define AMSW_CSR_RX3		28 >> 2
#define AMSW_CSR_TX4		32 >> 2
#define AMSW_CSR_RX4		36 >> 2
#define AMSW_CSR_TX5		40 >> 2
#define AMSW_CSR_RX5		44 >> 2
#define AMSW_CSR_TX6		48 >> 2
#define AMSW_CSR_RX6		52 >> 2
#define AMSW_CSR_TX7		56 >> 2
#define AMSW_CSR_RX7		60 >> 2
#define AMSW_CSR_INT		64 >> 2
#define AMSW_CSR_MASK		68 >> 2
#define AMSW_CSR_XVER		72 >> 2

/****************************************************/
/* TX Registers Bits                                */
/****************************************************/
#define AMSW_TX_BC_MASK		3 << 30
#define AMSW_TX_BC1			1 << 31
#define AMSW_TX_BC2			1 << 30
#define AMSW_TX_SIZE_MASK	0x0FFF0000
#define AMSW_TX_SIZE_SHFT	15				/* for byte size, 16 for word size */
#define AMSW_TX_START		1

/****************************************************/
/* RX Registers Bits                                */
/****************************************************/
#define AMSW_RX_BC_MASK		3 << 30
#define AMSW_RX_BC1			1 << 31
#define AMSW_RX_BC2			1 << 30
#define AMSW_RX_SIZE_MASK	0x0FFF0000
#define AMSW_RX_SIZE_SHFT	15				/* for byte size, 16 for word size */
#define AMSW_RX_DONE		1 << 4
#define AMSW_RX_OVERRUN		1 << 5
#define AMSW_RX_RUN			1 << 6
#define AMSW_RX_ERR_MASK	0x3F << 8
#define AMSW_RX_ERR1		1 << 8			/* The ORing of bit5 to bit10 */
#define AMSW_RX_ERR2		1 << 9			/* Packet length error */
#define AMSW_RX_ERR3		1 << 10			/* ESC alone */
#define AMSW_RX_ERR4		1 << 11			/* FCT alone */
#define AMSW_RX_ERR5		1 << 12			/* Parity error */
#define AMSW_RX_ERR6		1 << 13			/* BC1 missing */
#define AMSW_RX_ERR7		1 << 14			/* BC2 missing */

/****************************************************/
/* INT Register Bits                                */
/****************************************************/
#define AMSW_INT_RXDONE		0xFF
#define AMSW_INT_RXRUN		0xFF << 8
#define AMSW_INT_TXDONE		0xFF << 16
#define AMSW_INT_DMADONE	1 << 24

#define AMSW_INT_CH_MASK	0x00101
#define AMSW_INT_TX_MASK	0x10000
#define AMSW_INT_RX_MASK	0x00001

/****************************************************/
/* MASK Register Bits                               */
/****************************************************/
#define AMSW_MASK_RXDONE	0xFF
#define AMSW_MASK_DMADONE	1 << 24

/****************************************************/
/* Buffer addresses                                 */
/****************************************************/
#define AMSW_BUFF_TX0		0				/* 0x00000000 */
#define AMSW_BUFF_TX1		1 << 13			/* 0x00002000 */
#define AMSW_BUFF_TX2		2 << 13			/* 0x00004000 */
#define AMSW_BUFF_TX3		3 << 13			/* 0x00006000 */
#define AMSW_BUFF_TX4		4 << 13			/* 0x00008000 */
#define AMSW_BUFF_TX5		5 << 13			/* 0x0000A000 */
#define AMSW_BUFF_TX6		6 << 13			/* 0x0000C000 */
#define AMSW_BUFF_TX7		7 << 13			/* 0x0000E000 */
#define AMSW_BUFF_RX0		16 << 13		/* 0x00020000 */
#define AMSW_BUFF_RX1		17 << 13		/* 0x00022000 */
#define AMSW_BUFF_RX2		18 << 13		/* 0x00024000 */
#define AMSW_BUFF_RX3		19 << 13		/* 0x00026000 */
#define AMSW_BUFF_RX4		20 << 13		/* 0x00028000 */
#define AMSW_BUFF_RX5		21 << 13		/* 0x0002A000 */
#define AMSW_BUFF_RX6		22 << 13		/* 0x0002C000 */
#define AMSW_BUFF_RX7		23 << 13		/* 0x0002E000 */

/****************************************************/
/* typedefs / structs                               */
/****************************************************/

typedef struct {
	u_long	 	pci_off;	/* PCI offset */
	u_long * 	buff;		/* Buffer address */
	u_long 		phys_addr;	/* Physical address of buffer */
} AMSW_ADDRS;

/****************************************************/
/* function declarations                            */
/****************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __amswire_h */

