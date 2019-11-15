MD:

14-15 Nov 2019 I integrated all the modifications made by Viviana (that started from some version of end 2018, or even before) into the current trunk.

Now compiles and I had to fix just few errors: a couple were my errors on the importation, but most of them were in the "new" Viviana functions (the one related to MC) that were using the old framework (tdrCmp and tdrRaw instead of tdrMap, for example...). So is promising.
For sure, however, Viviana made some modification changing 64->256, 1024->4096, and other like this to adapt the number of channels, va's, etc... This must be done general, via the ladderconf.
She also inserted the tdrAlign[] map to store if a POX ladder is X or Y. This must be don via the ladderconf, like for the DAMPE ladder that are seen as XY modules but with one side not seeing nothing and with dummy pitch, resolutions, etc...
