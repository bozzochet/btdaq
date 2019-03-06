ReleaseFlags  = -c -Wall
DebugFlags = -c -Wall -g3 
libdir = /usr/lib
#make 
all: QuickUsb.o
	gcc -shared -Wl,-soname,libquickusb.so.2 -o libquickusb.so.2.0.0 QuickUsb.o -lc
	ar rcs libquickusb.a QuickUsb.o
#rm -f QuickUsb.o
QuickUsb.o: QuickUsb.cpp
	gcc -fPIC $(ReleaseFlags) QuickUsb.cpp
#make debug
debug: QuickUsbDebug.o
	gcc -g3 -shared -Wl,-soname,libquickusbdebug.so.2 -o libquickusbdebug.so.2.0.0 QuickUsbDebug.o -lc
	ar rcs libquickusbdebug.a QuickUsbDebug.o 
	rm -f QuickUsbDebug.o
QuickUsbDebug.o:
	gcc $(DebugFlags) QuickUsb.c -o QuickUsbDebug.o
#make install
install:
	install -c libquickusb.so.2.0.0 $(libdir)
	install -c libquickusb.a $(libdir)
	ln -f -s $(libdir)/libquickusb.so.2.0.0 $(libdir)/libquickusb.so.2
	ln -f -s $(libdir)/libquickusb.so.2.0.0 $(libdir)/libquickusb.so
	chmod --reference=$(libdir)/libquickusb.so.2.0.0 $(libdir)/libquickusb.* 
#make installDebug
installDebug:
	install -c libquickusbdebug.so.2.0.0 $(libdir)
	install -c libquickusbdebug.a $(libdir)
	ln -f -s $(libdir)/libquickusbdebug.so.2.0.0 $(libdir)/libquickusbdebug.so.2
	ln -f -s $(libdir)/libquickusbdebug.so.2.0.0 $(libdir)/libquickusbdebug.so
	chmod 755 $(libdir)/libquickusbdebug.*
#make clean
clean:
	rm *~ libquickusb* *.o
#make uninstall
uninstall:
	rm -f $(libdir)/libquickusb*



