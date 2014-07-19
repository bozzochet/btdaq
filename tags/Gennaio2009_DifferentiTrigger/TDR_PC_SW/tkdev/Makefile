#  $Id: Makefile,v 1.10 2008/10/23 10:12:39 zuccon Exp $
#
# Makefile to build tkdev - Tracker tools
#
# This Makefile is confirmed to work on 
#  SLC 4.5 with root-5.10.00
#
#   17/Jan/2008 PZ Rev. 1.1  First version
#   21/Jan/2008 SH Rev. 1.2  GbatchInt, some comments, and utils are added
#

### Build all ###
subsystem:
	@$(MAKE) -C AMSbase 
	@$(MAKE) -C AMSGeom 
	@$(MAKE) -C TrackerLib 
	@$(MAKE) -C TrackerTools
	@$(MAKE) -C Decode_tools
#	@$(MAKE) -C gbatch_sim 

.PHONY: clean status

### Cleanup ###
clean:
	@echo "Cleaning up"
	@$(MAKE) -C AMSbase clean
	@$(MAKE) -C AMSGeom clean
	@$(MAKE) -C TrackerLib clean
	@$(MAKE) -C TrackerTools clean
	@$(MAKE) -C Decode_tools clean
#	@$(MAKE) -C gbatch_sim clean

### Check CVS status ###
status:
	@cvs status | grep 'Status:' | grep -v 'Up-to-date' | cat
