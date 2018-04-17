/* -*-C-*-
 * amswire.c - AMSWire PCI Board device driver for Linux Kernel 2.4 or above.
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

#include <linux/autoconf.h>

#include <asm/segment.h>
#include <asm/system.h>
#include <asm/irq.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/page.h>
#include <asm/mman.h>

#include <linux/version.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/mm.h>
//#include <linux/wrapper.h>		// Removed in Kernel 2.6
#include <linux/byteorder/swab.h>

#include <linux/interrupt.h>

#include <linux/kernel.h>
#include <linux/sched.h>
//#include <linux/malloc.h>		// Replaced by slab.h in Kernel 2.6
#include <linux/slab.h>
#include <linux/sys.h>
#include <linux/vmalloc.h>

#include "amswire.h"
/************************************************************************/
/* Defines	                            							*/
/************************************************************************/
#define AMS_VENDOR_ID			0x414D
#define AMSW_DEVICEICE_ID		0x5304
#define AMSWIRE_SUB_VENDOR_ID	0x0000
#define AMSWIRE_SUBSYSTEM_ID	0x0000

#define AMSW_USE_IOREMAP
#define AMSW_USE_REQUEST_REGION

#define AMSW_MAJOR_NUMBER		240

/************************************************************************/
/* Typedefs / Structs                                                   */
/************************************************************************/

struct buff_desc {
	u_long 				vAddr;			/* Virtual address */
	u_long				mAddr;			/* Address after remap for user space */
	int				size;
	struct buff_desc	*next;
};

typedef struct buff_desc ams_buff_desc;

typedef struct {
	int			inuse;			/* Device inuse flag */
	int			irq_ena;		/* Interrupt enabled flag */
	int			irq_mask;		/* Interrupt mask */
	int			dma_ena;		/* DMA enabled flag */
	int			dma_inio;		/* DMA in IO space flag */
	struct pci_dev	*pdev;			/* Device information */
	u_char   		irq;			/* IRQ number */
	u_long *		mem;			/* Memory space */
	u_long *		reg;			/* Register space */
	u_long *		dma;			/* DMA space */
	void			*v_addr;		/* Virtual address for DMA */
	u_long			p_addr;			/* Physical address for DMA */
	volatile u_long	status;			/* Current status register value */
	u_short			a_ver;			/* Actel version */
	u_long			x_ver;			/* Xilinx version */
} AMSW_DEVICE;

/************************************************************************/
/* Globals                                                              */
/************************************************************************/

char amswire_vers[] = "2.0";
char device_name[] = "amswire";

/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

static int amsw_dev_open(struct inode *inode, struct file *file);
static int amsw_dev_close(struct inode *inode, struct file *file);
static ssize_t amsw_dev_read(struct file *file, char *buf, size_t nbytes, loff_t *ppos);
static ssize_t amsw_dev_write(struct file *file, const char *buf, size_t nbytes, loff_t *ppos);
static int amsw_dev_ioctl(struct inode *inode, struct file *file, u_int cmd, u_long param);
static void amsw_irq_handle(int irq, void *user_arg, struct pt_regs *regs);
static int amsw_probe(struct pci_dev *, const struct pci_device_id *);
static void amsw_remove(struct pci_dev *);
static int amsw_dev_mmap(struct file *file, struct vm_area_struct *vma);
void *amsw_dev_kmalloc(struct file * file, int size);
void amsw_dev_kfree(struct file *file, void *addr);
int amsw_direct_dma(AMSW_DEVICE *pd, int dir, AMSW_ADDRS *pa, int size);
int amsw_indirect_dma(AMSW_DEVICE *pd, int dir, AMSW_ADDRS *pa, int size);

/************************************************************************/
/* Locals                                                               */
/************************************************************************/

static int ncards;
static AMSW_DEVICE amswdv[AMSW_MAX_NCARDS];

static struct file_operations amsw_dev_fops =
{
	open:			amsw_dev_open,
	release:		amsw_dev_close,
	read:			amsw_dev_read,
	write:			amsw_dev_write,
	ioctl:			amsw_dev_ioctl,
	mmap:			amsw_dev_mmap,
};

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_dev_open - Open AMSWire device									*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
static int amsw_dev_open(struct inode *inode, struct file *file) {
	AMSW_DEVICE *pd;

	/* Check minor device ID */
	if ( MINOR(inode->i_rdev) >= ncards ) {
		printk("AMSWIRE: open non-exist device, minor ID = %d\n", MINOR(inode->i_rdev));
		return -EINVAL;
	}

	/* Pass device pointer to file structure */
	pd = &amswdv[MINOR(inode->i_rdev)];
	file->private_data = (void *) pd;
	file->f_pos = 0;

	/* Increase module in use count */
	//MOD_INC_USE_COUNT;	//Removed

	return 0;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_dev_close - Close AMSWire device								*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
static int amsw_dev_close(struct inode *inode, struct file *file) {
	ams_buff_desc *mp, **mpp;

	/* Pass device pointer to file structure */
	mpp = (ams_buff_desc **) &(file->f_pos);

	/* Free all buffers */
	if ( ((int) *mpp) ) {
		/* Find address */
		for ( mp = *mpp; mp; mp = mp->next ) {
			mpp = &(mp->next);
			amsw_dev_kfree(file, (void *) mp->mAddr);
		}
	}

	/* Clear module in use count */
	//MOD_DEC_USE_COUNT;	//Removed

	return 0;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_direct_dma - Direct DMA R/W between device and user buffer		*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_direct_dma(AMSW_DEVICE *pd, int dir, AMSW_ADDRS *pa, int size) {
	int i;
	u_int dma_ctrl;
	u_short pci_stat;
	u_long offset;
	u_long addr;
	int blk_len;

	/* Set initial offset */
	offset = pa->pci_off;

#ifdef AMSW_DEBUG
	printk("AMSWIRE: Direct DMA %s, addr = %08lx, offset = %08lx, size = %d\n",
		(dir == AMSW_DMA_RD ? "read" : "write"), pa->phys_addr, pa->pci_off, size);
#endif

	for ( addr = pa->phys_addr; size > 0; addr += AMSW_DMA_SIZE, size -= AMSW_DMA_SIZE, offset += AMSW_DMA_SIZE ) {
		/* Calculate block length */
		blk_len = size > AMSW_DMA_SIZE ? AMSW_DMA_SIZE : size;

		/* Prepare DMA control word */
		dma_ctrl = dir | (blk_len == AMSW_DMA_SIZE ? 0 : blk_len << 16);

		if ( pd->dma_inio ) {
			/* Write physical address (seen from PCI) */
			outl(addr, (int) &pd->dma[0]);

			/* Write device local address offset */
			outl(offset, (int) &pd->dma[1]);

			/* Write DMA control */
			if ( pd->irq_ena && (pd->irq_mask & AMSW_MASK_DMADONE) && (pd->status & AMSW_INT_DMADONE) ) {
				printk("AMSWIRE: unexpected DMA done interrupt\n");
				pd->status &= ~((u_long) AMSW_INT_DMADONE);
			}
			outl(dma_ctrl, (int) &pd->dma[2]);

			for ( i = 0; i < AMSW_DMA_TIMEOUT; i++ ) {
				if ( !pd->irq_ena || !(pd->irq_mask & AMSW_MASK_DMADONE) ) {
					pd->status |= pd->reg[AMSW_CSR_INT];
				}

				if ( (pd->status & AMSW_INT_DMADONE) ) {
					pd->status &= ~((u_long) AMSW_INT_DMADONE);
					dma_ctrl = inl((int) &pd->dma[2]);
					if ( dma_ctrl & AMSW_DMA_ERROR ) {
						pci_read_config_word(pd->pdev, PCI_STATUS, &pci_stat);
						printk("AMSWIRE: DMA (%s) error, INT Status = %08lx, DMA status = %08x, PCI status = %04x\n",
								(dir == AMSW_DMA_RD ? "read" : "write"), pd->status, dma_ctrl, pci_stat);
						return -EFAULT;
					}
					break;
				}
			}
		}
		else {
			/* Write physical address (seen from PCI) */
			if ( pci_write_config_dword(pd->pdev, (int) &pd->dma[0], addr) ) {
				printk("AMSWIRE: Fail to write DMA PCI address\n");
				return -EIO;
			}

			/* Write device local address offset */
			if ( pci_write_config_dword(pd->pdev, (int) &pd->dma[1], offset) ) {
				printk("AMSWIRE: Fail to write DMA local address\n");
				return -EIO;
			}

			/* Write DMA control */
			if ( pd->irq_ena && (pd->irq_mask & AMSW_MASK_DMADONE) && (pd->status & AMSW_INT_DMADONE) ) {
				printk("AMSWIRE: unexpected DMA done interrupt\n");
				pd->status &= ~((u_long) AMSW_INT_DMADONE);
			}
			if ( pci_write_config_dword(pd->pdev, (int) &pd->dma[2], dma_ctrl) ) {
				printk("AMSWIRE: Fail to write DMA control register\n");
				return -EIO;
			}

			for ( i = 0; i < AMSW_DMA_TIMEOUT; i++ ) {
				if ( !pd->irq_ena || !(pd->irq_mask & AMSW_MASK_DMADONE) ) {
					pd->status |= pd->reg[AMSW_CSR_INT];
				}

				if ( (pd->status & AMSW_INT_DMADONE) ) {
					pd->status &= ~((u_long) AMSW_INT_DMADONE);
					if ( pci_read_config_dword(pd->pdev, (int) &pd->dma[2], &dma_ctrl) ) {
						printk("AMSWIRE: Fail to read DMA control register\n");
						return -EIO;
					}
					if ( dma_ctrl & AMSW_DMA_ERROR ) {
						pci_read_config_word(pd->pdev, PCI_STATUS, &pci_stat);
						printk("AMSWIRE: DMA (%s) error, INT Status = %08lx, DMA status = %08x, PCI status = %04x\n",
								(dir == AMSW_DMA_RD ? "read" : "write"), pd->status, dma_ctrl, pci_stat);
						return -EFAULT;
					}
					break;
				}
			}
		}

		/* Timeout */
		if ( i == AMSW_DMA_TIMEOUT ) {
			pci_read_config_word(pd->pdev, PCI_STATUS, &pci_stat);
			printk("AMSWIRE: DMA (%s) timeout, DMA status = %08x, PCI status = %04x\n",
				(dir == AMSW_DMA_RD ? "read" : "write"), dma_ctrl, pci_stat);
			return -EFAULT;
		}
	}
	return 0;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_indirect_dma - Indirect DMA R/W between device and user buffer	*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int amsw_indirect_dma(AMSW_DEVICE *pd, int dir, AMSW_ADDRS *pa, int size) {
	void *vAddr;
	u_long pAddr;
	int i;
	u_int dma_ctrl;
	u_short pci_stat;
	u_long offset;
	u_long addr;
	int blk_len;

	/* Allocate memory for DMA */
	if ( !(vAddr = (void *) __get_dma_pages(GFP_KERNEL, ((AMSW_DMA_SIZE + PAGE_SIZE - 1) /  PAGE_SIZE))) ) {
		printk("AMSWIRE: Fail to allocate memory for DMA\n");
		return -EFAULT;
	}

	/* Get physical address */
	pAddr = (u_long) __pa(vAddr);

	/* Set initial offset */
	offset = pa->pci_off;

#ifdef AMSW_DEBUG
	printk("AMSWIRE: Indirect DMA %s, addr = %08lx, offset = %08lx, size = %d, kernel buffer = %08lx\n",
		(dir == AMSW_DMA_RD ? "read" : "write"), (u_long) pa->buff, pa->pci_off, size, pAddr);
#endif

	for ( addr = (u_long) pa->buff; size > 0; addr += AMSW_DMA_SIZE, size -= AMSW_DMA_SIZE, offset += AMSW_DMA_SIZE ) {
		/* Calculate block length */
		blk_len = size > AMSW_DMA_SIZE ? AMSW_DMA_SIZE : size;

		/* Memory copy (write only) */
		if ( dir == AMSW_DMA_WR ) memcpy((char *) vAddr, (char *) addr, blk_len);

		/* Prepare DMA control word */
		dma_ctrl = dir | (blk_len == AMSW_DMA_SIZE ? 0 : blk_len << 16);

		if ( pd->dma_inio ) {
			/* Write physical address (seen from PCI) */
			outl(pAddr, (int) &pd->dma[0]);

			/* Write device local address offset */
			outl(offset, (int) &pd->dma[1]);

			/* Write DMA control */
			if ( pd->irq_ena && (pd->irq_mask & AMSW_MASK_DMADONE) && (pd->status & AMSW_INT_DMADONE) ) {
				printk("AMSWIRE: unexpected DMA done interrupt\n");
				pd->status &= ~((u_long) AMSW_INT_DMADONE);
			}
			outl(dma_ctrl, (int) &pd->dma[2]);

			for ( i = 0; i < AMSW_DMA_TIMEOUT; i++ ) {
				if ( !pd->irq_ena || !(pd->irq_mask & AMSW_MASK_DMADONE) ) {
					pd->status |= pd->reg[AMSW_CSR_INT];
				}

				if ( (pd->status & AMSW_INT_DMADONE) ) {
					pd->status &= ~((u_long) AMSW_INT_DMADONE);
					dma_ctrl = inl((int) &pd->dma[2]);
					if ( dma_ctrl & AMSW_DMA_ERROR ) {
						pci_read_config_word(pd->pdev, PCI_STATUS, &pci_stat);
						printk("AMSWIRE: DMA (%s) error, INT Status = %08lx, DMA status = %08x, PCI status = %04x\n",
								(dir == AMSW_DMA_RD ? "read" : "write"), pd->status, dma_ctrl, pci_stat);
						return -EFAULT;
					}
					break;
				}
			}
		}
		else {
			/* Write physical address (seen from PCI) */
			if ( pci_write_config_dword(pd->pdev, (int) &pd->dma[0], pAddr) ) {
				printk("AMSWIRE: Fail to write DMA PCI address\n");
				goto config_error;
			}

			/* Write device local address offset */
			if ( pci_write_config_dword(pd->pdev, (int) &pd->dma[1], offset) ) {
				printk("AMSWIRE: Fail to write DMA local address\n");
				goto config_error;
			}

			/* Write DMA control */
			if ( pd->irq_ena && (pd->irq_mask & AMSW_MASK_DMADONE) && (pd->status & AMSW_INT_DMADONE) ) {
				printk("AMSWIRE: unexpected DMA done interrupt\n");
				pd->status &= ~((u_long) AMSW_INT_DMADONE);
			}
			if ( pci_write_config_dword(pd->pdev, (int) &pd->dma[2], dma_ctrl) ) {
				printk("AMSWIRE: Fail to write DMA control register\n");
				goto config_error;
			}

			for ( i = 0; i < AMSW_DMA_TIMEOUT; i++ ) {
				if ( !pd->irq_ena || !(pd->irq_mask & AMSW_MASK_DMADONE) ) {
					pd->status |= pd->reg[AMSW_CSR_INT];
				}

				if ( (pd->status & AMSW_INT_DMADONE) ) {
					pd->status &= ~((u_long) AMSW_INT_DMADONE);
					if ( pci_read_config_dword(pd->pdev, (int) &pd->dma[2], &dma_ctrl) ) {
						printk("AMSWIRE: Fail to read DMA control register\n");
						return -EIO;
					}
					if ( dma_ctrl & AMSW_DMA_ERROR ) {
						pci_read_config_word(pd->pdev, PCI_STATUS, &pci_stat);
						printk("AMSWIRE: DMA (%s) error, INT Status = %08lx, DMA status = %08x, PCI status = %04x\n",
								(dir == AMSW_DMA_RD ? "read" : "write"), pd->status, dma_ctrl, pci_stat);
						return -EFAULT;
					}
					break;
				}
			}
		}

		/* Timeout */
		if ( i == AMSW_DMA_TIMEOUT ) {
			pci_read_config_word(pd->pdev, PCI_STATUS, &pci_stat);
			printk("AMSWIRE: DMA (%s) timeout, DMA status = %08x, PCI status = %04x\n",
				(dir == AMSW_DMA_RD ? "read" : "write"), dma_ctrl, pci_stat);
			goto dma_error;
		}

		/* Memory copy (read only) */
		if ( dir == AMSW_DMA_RD ) memcpy((char *) addr, (char *) vAddr, blk_len);
	}

	/* Free memory for DMA */
	free_pages((u_long) vAddr, ((AMSW_DMA_SIZE + PAGE_SIZE - 1) /  PAGE_SIZE));
	return 0;

dma_error:
	/* Free memory for DMA */
	free_pages((u_long) vAddr, ((AMSW_DMA_SIZE + PAGE_SIZE - 1) /  PAGE_SIZE));

	return -EFAULT;

config_error:
	/* Free memory for DMA */
	free_pages((u_long) vAddr, ((AMSW_DMA_SIZE + PAGE_SIZE - 1) /  PAGE_SIZE));

	return -EIO;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_dev_read - Read from BAR2 of AMSWire device						*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
static ssize_t amsw_dev_read(struct file *file, char *buf, size_t nbytes, loff_t *ppos) {
	register AMSW_DEVICE *pd;
	AMSW_ADDRS *pa;
	int size;
	int status;
	struct vm_area_struct *vma;
#ifndef __PPC__
	u_long *dd;
#endif

	/* Get device pointer */
	pd = (AMSW_DEVICE *) file->private_data;
	pa = (AMSW_ADDRS *) buf;

	/* Check in use flag */
	if ( pd->inuse ) return -EBUSY;
	pd->inuse = 1;

	/* Get size in long word */
	size = (nbytes + 3) / 4 * 4;

	/* Read Data */
	if ( size <= 0 ) {
		pd->inuse = 0;
		return 0;
	}
	else if ( size > 32 && pd->dma_ena ) {
		vma = find_vma(current->mm, (int) pa->buff);
		if ( (int) vma->vm_pgoff <= 0 ) status = amsw_indirect_dma(pd, AMSW_DMA_RD, pa, size);
		else {
			pa->phys_addr = (u_long) (vma->vm_pgoff << PAGE_SHIFT) | ((u_long) pa->buff - vma->vm_start);
			status = amsw_direct_dma(pd, AMSW_DMA_RD, pa, size);
		}
		if ( status < 0 ) {
			pd->inuse = 0;
			return status;
		}
	}
	else {
		memcpy((char *) pa->buff, (char *) ((u_long) pd->mem + (u_long) pa->pci_off), size);
	}

#ifndef __PPC__
	/* Check the last word */
	dd = (u_long *) pa->buff;
	switch ( size - nbytes ) {
		case 1:
			dd[nbytes / 4] >>= 8;
			break;
		case 2:
			dd[nbytes / 4] >>= 16;
			break;
		case 3:
			dd[nbytes / 4] >>= 24;
			break;
	}
#endif

	pd->inuse = 0;
	return nbytes;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_dev_write - Write to BAR2 of AMSWire device						*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
#undef AMSWIRE_WRITE_DMA
static ssize_t amsw_dev_write(struct file *file, const char *buf, size_t nbytes, loff_t *ppos) {
	register AMSW_DEVICE *pd;
	AMSW_ADDRS *pa;
	int size;
#ifdef AMSWIRE_WRITE_DMA
	int status;
	struct vm_area_struct *vma;
#endif
#ifndef __PPC__
	u_long data;
#endif

	/* Get device pointer */
	pd = (AMSW_DEVICE *) file->private_data;
	pa = (AMSW_ADDRS *) buf;

	/* Check in use flag */
	if ( pd->inuse ) return -EBUSY;
	pd->inuse = 1;

	/* Get size in long word */
	size = nbytes / 4 * 4;

#ifdef AMSWIRE_WRITE_DMA
	/* Write Data */
	if ( (int) nbytes <= 0 ) {
		return 0;
		pd->inuse = 0;
	}
	else if ( size > 32 && pd->dma_ena ) {
		vma = find_vma(current->mm, (int) pa->buff);
		if ( (int) vma->vm_pgoff <= 0 ) status = amsw_indirect_dma(pd, AMSW_DMA_WR, pa, size);
		else {
			pa->phys_addr = (u_long) (vma->vm_pgoff << PAGE_SHIFT) | ((u_long) pa->buff - vma->vm_start);
			status = amsw_direct_dma(pd, AMSW_DMA_WR, pa, size);
		}
		if ( status < 0 ) {
			pd->inuse = 0;
			return status;
		}
	}
	else {
		memcpy((char *) ((u_long) pd->mem + (u_long) pa->pci_off), (char *) pa->buff, size);
	}
#else	/* AMSWIRE_WRITE_DMA */
	if ( (int) nbytes <= 0 ) {
		pd->inuse = 0;
		return 0;
	}
	else {
		memcpy((char *) ((u_long) pd->mem + (u_long) pa->pci_off), (char *) pa->buff, size);
	}
#endif	/* AMSWIRE_WRITE_DMA */

#ifndef __PPC__
	/* Check the last word */
	if ( (nbytes - size) ) {
		data = pa->buff[size >> 2];
		switch ( nbytes - size ) {
			case 1:
				data <<= 24;
				break;
			case 2:
				data <<= 16;
				break;
			case 3:
				data <<= 8;
				break;
		}
		pd->mem[((int) pa->pci_off + size) >> 2] = data;
	}
#endif	/* __PPC__ */

	pd->inuse = 0;
	return nbytes;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_dev_mmap - remap memory											*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
static int amsw_dev_mmap(struct file *file, struct vm_area_struct *vma) {
	u_long offset;

	/* Get offset */
	offset = vma->vm_pgoff << PAGE_SHIFT;

	/* remap range */
	if ( remap_pfn_range(vma, vma->vm_start, offset, vma->vm_end - vma->vm_start, vma->vm_page_prot) )	//Modified
		return -EAGAIN;

	return 0;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_dev_kmalloc - Allocate kernel memory for user space				*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
void * amsw_dev_kmalloc(struct file *file, int size) {
	ams_buff_desc *mp_old, *mp_new;
	u_long mAddr, vAddr;

	/* Allocate kernel memory for descriptor */
	mp_new = (ams_buff_desc *) kmalloc(sizeof(ams_buff_desc), GFP_KERNEL);
	if ( !mp_new ) {
		printk("AMSWIRE: can't allocate kernel memory for ams_buff_desc\n");
		return 0;
	}

	/* why ? */
	if ( size <= PAGE_SIZE / 2 ) size = PAGE_SIZE - 0x10;
	if ( size > 0x1FFF0 ) {
		printk("AMSWIRE: can't allocate so big kernel memory for user, size = %d\n", size);
		return 0;
	}

	/* Allocate memory for user */
	vAddr = (u_long) kmalloc(size, __GFP_DMA | __GFP_HIGH | __GFP_WAIT );
	if ( !vAddr ) {
		printk("AMSWIRE: can't allocate %d bytes of kernel memory for user\n", size);
		kfree(mp_new);
		return 0;
	}

	/* Reserve memory area in mem_map table */
	for ( mAddr = vAddr & PAGE_MASK; mAddr < vAddr + size; mAddr += PAGE_SIZE ) 
		SetPageReserved(virt_to_page(mAddr));

	/* make memory map */
	mAddr = do_mmap(file,						/* file structure */
				0,							/* don't care the target address */
				size,						/* size */
				PROT_READ | PROT_WRITE,		/* Protection */
				MAP_SHARED,					/* flag */
				__pa((void *) vAddr));						/* Offset */

	/* Check error */
	if ( (int) mAddr <= 0 ) {
		printk("AMSWIRE: error from do_mmap, error = %d\n", (int) mAddr);
		kfree(mp_new);
		kfree((void *) vAddr);
		return 0;
	}
#ifdef AMSW_DEBUG
	printk("AMSWIRE: Allocate kernel memory %d bytes @ 0x%08lx, mapped to 0x%08lx\n", size, vAddr, mAddr);
#endif

	/* Get the old point for the next */
	mp_old = (ams_buff_desc *) ((long) file->f_pos);
	mp_new->vAddr = vAddr;
	mp_new->mAddr = mAddr;
	mp_new->size = size;
	mp_new->next = mp_old;
	file->f_pos = (loff_t) ((long) mp_new);

	return (void *) mAddr;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_dev_kfree - Free kernel memory for user space					*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
void amsw_dev_kfree(struct file *file, void *addr) {
	ams_buff_desc *mp, **mpp;

	/* Get the first point */
	mpp = (ams_buff_desc **) &file->f_pos;

	/* Find address */
	for ( mp = *mpp; mp; mp = mp->next ) {
		if ( mp->mAddr == (u_long) addr ) break;
		mpp = &(mp->next);
	}

	/* Unmap memory */
#ifdef AMSW_DEBUG
	printk("AMSWIRE: unmap memory %d bytes @ 0x%08lx (virtual @ 0x%08lx)\n", mp->size, mp->mAddr, mp->vAddr);
#endif
	do_munmap(current->mm, (int) addr & PAGE_MASK, (mp->size + ~PAGE_MASK) & PAGE_MASK);

	addr = (void *) mp->vAddr;

	/* Remove reserved memory area from mem_map table */
	do {
		ClearPageReserved(virt_to_page(addr));
		addr += PAGE_SIZE;
	} while ( (u_long) addr < mp->vAddr + mp->size );

	/* Change the next point */
	*mpp = mp->next;

	/* Free memory */
	kfree((void *) mp->vAddr);
	kfree(mp);
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_dev_ioctl - I/O control of AMSWire device						*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
static int amsw_dev_ioctl(struct inode *inode, struct file *file, u_int cmd, u_long param) {
	register AMSW_DEVICE *pd;
	void *mp;

	/* Get device pointer */
	pd = &amswdv[MINOR(inode->i_rdev)];

	/* Process commands without accessing card */
	if ( (cmd & AMSW_SPACE_MASK) == AMSW_CMD_SPACE && (cmd & 0xff) >= 20 ) {
		switch ( cmd & 0xff ) {
			case AMSW_CMD_ENA_DMA:
				if ( pd->a_ver & AMSW_AVER_DMA ) {
					pd->dma_ena = 1;
					printk("AMSWIRE: DMA is enabled\n");
				}
				else
					printk("AMSWIRE: DMA is not supported in this version\n");
				break;
			case AMSW_CMD_DIS_DMA:
				pd->dma_ena = 0;
				printk("AMSWIRE: DMA is disabled\n");
				break;
			case AMSW_CMD_INUSE:
				*((int *) param) = pd->inuse;
				break;
			case AMSW_CMD_BAR0:
				*((int *) param) = (int) pd->mem;
				break;
			case AMSW_CMD_BAR1:
				*((int *) param) = (int) pd->reg;
				break;
			case AMSW_CMD_MALLOC:
				mp = amsw_dev_kmalloc(file, *((int *) param));
				*((u_long *) param) = (u_long) mp;
				break;
			case AMSW_CMD_FREE:
				amsw_dev_kfree(file, (void *) param);
				break;
			case AMSW_CMD_CLR_STAT:
				pd->status &= ~param;
				break;
			case AMSW_CMD_CUR_STAT:
				*((u_long *) param) = pd->status;
				break;
			default:
				printk("AMSWIRE: Invalid command, cmd = %#8.8x\n", cmd);
				return -EINVAL;
		}
		return 0;
	}

	/* Check in use flag */
	if ( pd->inuse ) return -EBUSY;
	pd->inuse = 1;

	/* Process functions */
	switch ( cmd & (AMSW_SPACE_MASK | AMSW_RW_MASK) ) {
		case (AMSW_CSR_SPACE | AMSW_IOCTL_RD):
			*((u_long *) param) = pd->reg[cmd & 0xff];
			break;
		case (AMSW_CSR_SPACE | AMSW_IOCTL_WR):
			pd->reg[cmd & 0xff] = param;
			break;
		case (AMSW_DMA_SPACE | AMSW_IOCTL_RD):
			if ( pd->dma_inio ) {
				*((u_long *) param) = inl((int) &pd->dma[cmd & 0xff]);
			}
			else {
				if ( pci_read_config_dword(pd->pdev, (int) &pd->dma[cmd & 0xff], (u_int *) param) ) goto ioerror;
			}
			break;
		case (AMSW_DMA_SPACE | AMSW_IOCTL_WR):
			if ( pd->dma_inio ) {
				outl(param, (int) &pd->dma[cmd & 0xff]);
			}
			else {
				if ( pci_write_config_dword(pd->pdev, (int) &pd->dma[cmd & 0xff], param) ) goto ioerror;
			}
			break;
		case (AMSW_CMD_SPACE):
		case (AMSW_CMD_SPACE | AMSW_IOCTL_RD):
		case (AMSW_CMD_SPACE | AMSW_IOCTL_WR):
			switch ( cmd & 0xff ) {
				case AMSW_CMD_EIRQ_ALL:
					if ( pd->irq_ena ) {
						pd->irq_mask = AMSW_MASK_RXDONE | AMSW_MASK_DMADONE;
						pd->reg[AMSW_CSR_MASK] = AMSW_MASK_RXDONE | AMSW_MASK_DMADONE;
						printk("AMSWIRE: All interrupts are enabled\n");
					}
					break;
				case AMSW_CMD_DIRQ_ALL:
					pd->irq_mask = 0;
					pd->reg[AMSW_CSR_MASK] = 0;
					printk("AMSWIRE: All interrupts are disabled\n");
					break;
				case AMSW_CMD_EIRQ_RX:
					if ( pd->irq_ena ) {
						pd->irq_mask |= AMSW_MASK_RXDONE;
						pd->reg[AMSW_CSR_MASK] |= AMSW_MASK_RXDONE;
						printk("AMSWIRE: RX done interrupts are enabled\n");
					}
					break;
				case AMSW_CMD_DIRQ_RX:
					pd->irq_mask &= ~((u_long) AMSW_MASK_RXDONE);
					pd->reg[AMSW_CSR_MASK] &= ~((u_long) AMSW_MASK_RXDONE);
					printk("AMSWIRE: RX done interrupts are disabled\n");
					break;
				case AMSW_CMD_EIRQ_DMA:
					if ( pd->irq_ena ) {
						pd->irq_mask |= AMSW_MASK_DMADONE;
						pd->reg[AMSW_CSR_MASK] |= AMSW_MASK_DMADONE;
						printk("AMSWIRE: DMA done interrupt is enabled\n");
					}
					break;
				case AMSW_CMD_DIRQ_DMA:
					pd->irq_mask &= ~((u_long) AMSW_MASK_DMADONE);
					pd->reg[AMSW_CSR_MASK] &= ~((u_long) AMSW_MASK_DMADONE);
					printk("AMSWIRE: DMA done interrupt is disabled\n");
					break;
				case AMSW_CMD_SET_MASK:
					pd->irq_mask = param;
					pd->reg[AMSW_CSR_MASK] = param;
					break;
				case AMSW_CMD_NEW_STAT:
					if ( !pd->irq_ena || !(pd->irq_mask & AMSW_MASK_RXDONE) ) pd->status |= pd->reg[AMSW_CSR_INT];
					*((u_long *) param) = pd->status;
					break;
				default:
					goto invalid;
			}
			break;
		default:
			goto invalid;
	}

	/* Normal return */
	pd->inuse = 0;
	return 0;
	
ioerror:	/* IO Error */
	printk("AMSWIRE: IO error in IOCTL, cmd = %#8.8x\n", cmd);
	pd->inuse = 0;
	return -EIO;
invalid:	/* Invalid address or command */
	printk("AMSWIRE: Invalid command or address, cmd = %#8.8x\n", cmd);
	pd->inuse = 0;
	return -EINVAL;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_irq_handle - Interrupt handler routine 							*/
/*																*/
/* RETURNS: 															*/
/* 	0 on success													*/
/************************************************************************/
static void amsw_irq_handle(int irq, void *user_arg, struct pt_regs *regs) {
	register AMSW_DEVICE *pd = (AMSW_DEVICE *) user_arg;

	/* Disable global interrupt */
	local_irq_disable();

	/* Read INT status register */
	pd->status |= pd->reg[AMSW_CSR_INT];

#if 0
	if ( !(pd->status & 0x10000ff) ) {
		printk("AMSWIRE: no interrupt bits are set, status = %08lx\n", pd->status);
	}
#endif

	/* Enable global interrupt */
	local_irq_enable();

	/* Set got IRQ flag */
	return;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_probe - initialize AMSWire PCI module							*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
static int amsw_probe(struct pci_dev *pdev, const struct pci_device_id *ent) {
	register AMSW_DEVICE *pd;
	u_char  latency;

	/* Enable Device and set master */
	pci_write_config_word(pdev, PCI_COMMAND, (PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER));

	/* Get device information */
	pd = &amswdv[ncards++];

	pd->pdev = pdev;
	pd->irq = pdev->irq;

	pd->mem = (u_long *) pci_resource_start(pdev, 0);
	pd->mem = ioremap((u_long) pd->mem, pci_resource_len(pdev, 0));
	
	pd->reg = (u_long *) pci_resource_start(pdev, 1);
	pd->reg = ioremap((u_long) pd->reg, pci_resource_len(pdev, 1));

	/* Get versions */
	pci_read_config_word(pdev, PCI_CLASS_REVISION, &pd->a_ver);
	pd->x_ver = pd->reg[AMSW_CSR_XVER];
	printk("AMSWIRE: Card No.%d: Actel version = %#4.4x, Xilinx version = %#8.8lx, Software version = %s\n",
		ncards - 1, pd->a_ver, pd->x_ver, amswire_vers);
		
	/* Check number of channels */
	if ( pd->x_ver & AMSW_XVER_CHN ) 
		printk("AMSWIRE:             8 Channel ");
	else
		printk("AMSWIRE:             10 Channel ");
		
	/* Check board type */
	if ( pd->a_ver & AMSW_AVER_FLIGHT )
		printk("Flight version, ");
	else
		printk("Prototype, ");
		
	/* Request IRQ */
	if ( request_irq(pd->irq, &amsw_irq_handle, IRQF_SHARED, device_name, pd) < 0 ) {
		printk("AMSWIRE: Fail to request IRQ\n");
		return -EFAULT;
	}
	pd->status = pd->reg[AMSW_CSR_INT];

	/* Enable only DMA_DONE interrupt */
	pd->irq_mask = AMSW_MASK_DMADONE;
	pd->reg[AMSW_CSR_MASK] = AMSW_MASK_DMADONE;

	/* Check IRQ function */
	if ( pd->a_ver & AMSW_AVER_IRQ ) {
		pd->irq_ena = 1;
		printk("IRQ exist, IRQ = %d, ", pd->irq);

	}
	else {
		pd->irq_ena = 0;
		printk("no IRQ, ");
	}
	
	/* Check DMA function */
	if ( pd->a_ver & AMSW_AVER_DMA ) {
		pd->dma_ena = 1;

		/* Check bases */
		if ( (pd->a_ver & AMSW_AVER_SPACE) == AMSW_AVER_SPACE ) {
			pd->dma_inio = 1;
			pd->dma = (u_long *) (pci_resource_start(pdev, 2) + AMSW_DMA_OFFSET);
			printk("DMA exist, DMA in IO Space, Address = %lx\n", (u_long) pd->dma);
		}
		else {
			pd->dma_inio = 0;
			pd->dma = (u_long *) AMSW_DMA_OFFSET;
			printk("DMA exist, DMA in Config Space, Address = %lx\n", (u_long) pd->dma);
		}
	}
	else {
		pd->dma_ena = 0;
		pd->dma_inio = 0;
		pd->dma = (u_long *) AMSW_DMA_OFFSET;
		printk("no DMA\n");
	}
	
#ifdef AMSW_SET_LATENCY_TIMER
	/* Set Latency Timer */
	if ( timer != AMSW_LATENCY_TIMER ) {
		timer = AMSW_LATENCY_TIMER;
		if ( pci_write_config_byte(pd->pdev, PCI_LATENCY_TIMER, timer) ) {
			printk("AMSWIRE: Read Latency Timer fails\n");
			return -EIO;
		}
	}
#endif	/* AMSW_SET_LATENCY_TIMER */

	/* Read the current setting */
	pci_read_config_byte(pdev, PCI_LATENCY_TIMER, &latency);

	printk("AMSWIRE:             BAR0 = %ldK @ %#8.8lx, BAR1 = %ld @ %#8.8lx, PCI Latency Timer = %d\n", 
		pci_resource_len(pdev, 0) / 1024, (u_long) pd->mem, pci_resource_len(pdev, 1), (u_long) pd->reg, latency);

	/* Set device is not in use */
	pd->inuse = 0;

	return 0;
}


/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_remove - remove AMSWire PCI module								*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
static void amsw_remove(struct pci_dev *pdev) {
	register AMSW_DEVICE *pd;
	int card;

	/* Find card number */
	pd = &amswdv[0];
	for ( card = 0; card < ncards; card++ ) {
		pd = &amswdv[card];
		if ( pd->pdev == pdev ) break;
	}

	/* Unmap addresses */
	iounmap(pd->mem);
	iounmap(pd->reg);

	/* Free IRQ */
	free_irq(pd->irq, pd);

	printk("AMSWIRE: device driver for card No.%d is removed\n", card);
}

/* Define the AMSWire Device ID table */
static struct pci_device_id amsw_pci_tbl[] = {
	{AMS_VENDOR_ID, AMSW_DEVICEICE_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
	{0x11AA, 0x5555, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
	{0,}
};

/* Define AMSWire Device driver */
static struct pci_driver amsw_pci_driver = {
	name: device_name,
	probe: amsw_probe,
	remove: amsw_remove,
	id_table: amsw_pci_tbl,
};

/************************************************************************/
/* Function:															*/
/*																*/
/* init_module - install and initialize AMSWire PCI module.				*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int init_module(void) {
	int err;

	/* Reset number of cards */
	ncards = 0;

	/* Initialize cards */
//	if ( (err = pci_module_init(&amsw_pci_driver)) < 0 ) {	// pci_module_init deprecated in Kernel 2.6
	if ( (err = pci_register_driver(&amsw_pci_driver)) < 0 ) {
		if ( !ncards )
			printk("AMSWIRE: No AMSWire PCI card found.\n");
		else 
			printk("AMSWIRE: error during pci_module_init, error = %x\n", err);
		return err;
	}
	
	/* Check if there is no cards found */
	if ( !ncards ) {
		printk("AMSWIRE: No AMSWire PCI card found.\n");
		return -ENODEV;
	}
	
	/* Register as a character device */
	if ( (err = register_chrdev(AMSW_MAJOR_NUMBER, device_name, &amsw_dev_fops)) < 0 ) {
		printk("AMSWIRE: Unable to allocate a major number. Error = %x\n", err);
		return -EIO;
	}

	printk("AMSWIRE: %d AMSWire PCI cards found in the system.\n", ncards);
	
	return 0;
}


/************************************************************************/
/* Function:															*/
/*																*/
/* cleanup_module - deinstall AMSWire module.							*/
/*																*/
/* RETURNS:															*/
/* 	none															*/
/************************************************************************/
void cleanup_module(void){

	/* Unregister device driver */
	pci_unregister_driver(&amsw_pci_driver);

	/* Unregister character device */
//	if ( (unregister_chrdev(AMSW_MAJOR_NUMBER, device_name)) )	// No return value of unregister_chrdev in Kernel 2.6
//		printk("AMSWIRE: Fail to unregister character device\n");
	unregister_chrdev(AMSW_MAJOR_NUMBER, device_name);
	return;
}
