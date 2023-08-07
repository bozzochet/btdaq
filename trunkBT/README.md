#There are several directories:

- [this is essentially not anymore used] DAQ: Data Acquisition software (Usable via EPP or PCI. Compatible with TDR, JINF and  JINJ DSP programs as in July 2009 and usable with JLV1, NI-USB or manual trigger. The data can be written in AMS Block format or in binary standard one. Calibration can be written in AMS Block format or text one visible with SUMMARY on a pdf)
- [the core is here] Decode: Decoding software for converting binary files (it works only with this type of data and with text calibration files) to Root ones
- CommonTool: utilities used both in 'Align' and 'Analysis' codes
- Analysis: tempative to collect some new analysis sw (macros, etc...)
- Align: alignment code
- POXAnalysis: as Analysis but dedicated to POX
- RawData, CalData, RootData: defaults directories for raw/binary data, calibrations and output decoded ROOT files
- [essentially empty] GainEqualization: gain equalization code
- [outdated] MergeWithOthers: code to filter ROOT data and merge with other detector files (Bari, IHEP one, etc...)
- [outdated] Scan: Analisis software for creating Hisotgrams from Root files (IT IS MADE BY WJB AND IS NOT A STABLE VERSION. CAN BE USED ONLY AS TEMPLATE)

Enjoy!
