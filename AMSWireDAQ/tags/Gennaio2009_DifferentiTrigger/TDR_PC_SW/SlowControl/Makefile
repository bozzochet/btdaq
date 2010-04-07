# file Makefile for .../SlowControl/ directory
#
# A.Lebedev - Mar-2005...

#------------------------------------------------------------------------------

all:
	$(MAKE) -C CAN             $@
	$(MAKE) -C AMSW            $@
	$(MAKE) -C GSC             $@
	$(MAKE) -C CAN/design      $@
	$(MAKE) -C CAN/FS          $@
	$(MAKE) -C AMSW/design     $@
	$(MAKE) -C PCI-AMSW        $@
	$(MAKE) -C MISC            $@
	$(MAKE) -C TTCP            $@

clean:
	rm -f *~ *.bak core
	$(MAKE) -C CAN             $@
	$(MAKE) -C AMSW            $@
	$(MAKE) -C GSC             $@
	$(MAKE) -C CAN/design      $@
	$(MAKE) -C CAN/FS          $@
	$(MAKE) -C AMSW/design     $@
	$(MAKE) -C PCI-AMSW        $@
	$(MAKE) -C MISC            $@
	$(MAKE) -C TTCP            $@

install:
	$(MAKE) -C CAN             $@
	$(MAKE) -C CAN/FS          $@
	$(MAKE) -C AMSW            $@
	$(MAKE) -C GSC             $@

export:
	$(MAKE) -C CAN             $@
	$(MAKE) -C CAN/FS          $@
	$(MAKE) -C AMSW            $@
	$(MAKE) -C GSC             $@

links:
	$(MAKE) -C CAN             $@
	$(MAKE) -C CAN/FS          $@
	$(MAKE) -C AMSW            $@
	$(MAKE) -C GSC             $@

unlinks:
	$(MAKE) -C CAN             $@
	$(MAKE) -C CAN/FS          $@
	$(MAKE) -C AMSW            $@
	$(MAKE) -C GSC             $@

purge:
	$(MAKE) -C CAN             $@
	$(MAKE) -C CAN/FS          $@
	$(MAKE) -C AMSW            $@
	$(MAKE) -C GSC             $@

#------------------------------------------------------------------------------

release: clean
	 (cd ..; tar zcvf SlowControl.`date +%Y%m%d`.tgz SlowControl/*)

save:
	 (cd ..; \
          scp -p SlowControl.`date +%Y%m%d`.tgz \
          lebedev@ams.cern.ch:/dat2/users/lebedev)

#==============================================================================
