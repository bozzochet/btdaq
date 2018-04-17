#-------------------------------------------------------------------
# This file includes the common make rules for AMS02 JMDC software
#
#   Created by X.Cai, on 17.03.2002
#-------------------------------------------------------------------

DEBUG	= 0
#TRACE	= 1	# no hwreset()
TRACE	= 0	# perform hwreset()

MAKE		= make

SRCROOT = $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi | sed -e s/src.*/src/)
EXEROOT = $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi | sed -e s/aal.*/aal/)
DRVDIR  = .
BINDIR  = .
INCDIR  = /lib/modules/2.6.9-55.0.2.EL.cern/build/include
LIBDIR  = .

ARCH := $(shell if [ -f $(SRCROOT)/.hhl_target_cpu ]; then cat $(SRCROOT)/.hhl_target_cpu; else \
	  uname -m | sed -e s/i.86/i386/ -e s/sun4u/sparc64/ -e s/arm.*/arm/ \
	  -e s/sa110/arm/ ; fi)

HOSTCC  	= gcc
HOSTCFLAGS	= -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer

CROSS_COMPILE 	= $(shell if [ -f $(SRCROOT)/.hhl_cross_compile ]; then \
	  cat $(SRCROOT)/.hhl_cross_compile; fi)

#
# Include the make variables (CC, etc...)
#
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
RANLIB		= $(CROSS_COMPILE)ranlib
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
GENKSYMS	= /sbin/genksyms
DEPMOD		= /sbin/depmod
PERL		= perl

CP		= cp -f
RM		= rm -f

TARGET		= Linux
ARCH_SUFFIX	= $(ARCH)
TOOL		= gnu

EXTRA_SOURCE		=
EXTRA_MAKE_TARGET	=
EXTRA_DEFINE		=

GCC_WARN = -pedantic \
	-Wcast-align -Wcast-qual -Wtraditional -Wpointer-arith \
	-Wwrite-strings \
	-Wmissing-prototypes -Wmissing-declarations -Wredundant-decls \
	-Wnested-externs

GCC_WARN_HARD = \
	-Wconversion -Wstrict-prototypes

## Simple Warnings
CC_WARN = -Wall
## More detailed Warnings
#CC_WARN = $(GCC_WARN) $(GCC_WARN_HARD)
#CC_WARN = $(GCC_WARN)

CC_OPTIM_DRIVER = -O2 -fno-builtin
CC_OPTIM_NORMAL = -O -fstrength-reduce -fno-builtin
CC_OPTIM_TARGET = -O -fvolatile -fno-builtin
CC_DEFINES	= -DLINUX $(DEFINE_CC) $(EXTRA_DEFINE)

#MACHFLG = -I/tftpboot/spruce/usr/include $(MACHINE)
MACHDEP = $(CC_OPTIM_DRIVER) $(CC_WARN)

AMSFLAG = -I. -I$(INCDIR) -L. -L$(LIBDIR)

CFLAGS  = -g $(AMSFLAG) $(MACHFLG) $(MACHDEP) $(CC_DEFINES)
CKFLAGS = -D__KERNEL__ -DKERNEL  -DMODULE

#        -DDEBUG=$(DEBUG) -DPRINT=0 -DTRACE=$(TRACE) \

#
# Common rules
#
#
#%.s: %.c
#	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -S $< -o $@
#
#%.i: %.c
#	$(CPP) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) $< > $@
#
#%.o: %.c
#	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -c -o $@ $<
#	@ ( \
#	    echo 'ifeq ($(strip $(subst $(comma),:,$(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@))),$$(strip $$(subst $$(comma),:,$$(CFLAGS) $$(EXTRA_CFLAGS) $$(CFLAGS_$@))))' ; \
#	    echo 'FILES_FLAGS_UP_TO_DATE += $@' ; \
#	    echo 'endif' \
#	) > $(dir $@)/.$(notdir $@).flags
#
#%.o: %.s
#	$(AS) $(AFLAGS) $(EXTRA_CFLAGS) -o $@ $<
