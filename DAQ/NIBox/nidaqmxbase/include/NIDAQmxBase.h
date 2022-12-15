/*============================================================================*/
/*                 National Instruments / Data Acquisition                    */
/*----------------------------------------------------------------------------*/
/*    Copyright (c) National Instruments 2004.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Title:       nidaqmxbase.h                                                 */
/* Purpose:     Include file for NI-DAQmxBase library support.                */
/*                                                                            */
/*============================================================================*/

#ifndef ___nidaqmxbase_h___
#define ___nidaqmxbase_h___

#ifdef __cplusplus
    extern "C" {
#endif

#define __CFUNC         __cdecl
#define __CFUNC_C       __cdecl
#define __CFUNCPTRVAR   __cdecl
#if defined WIN32
   #ifndef DllExport
      #define DllExport     __declspec(dllexport)
   #endif
#else
   #ifndef DllExport
      #define DllExport
   #endif
   #ifndef __cdecl
      #define __cdecl
   #endif
#endif

// NI-DAQmx Typedefs
#ifndef _NI_int8_DEFINED_
#define _NI_int8_DEFINED_
   typedef signed char        int8;
#endif
#ifndef _NI_uInt8_DEFINED_
#define _NI_uInt8_DEFINED_
   typedef unsigned char      uInt8;
#endif
#ifndef _NI_int16_DEFINED_
#define _NI_int16_DEFINED_
   typedef signed short       int16;
#endif
#ifndef _NI_uInt16_DEFINED_
#define _NI_uInt16_DEFINED_
   typedef unsigned short     uInt16;
#endif
#ifndef _NI_int32_DEFINED_
#define _NI_int32_DEFINED_
   typedef signed long        int32;
#endif
#ifndef _NI_uInt32_DEFINED_
#define _NI_uInt32_DEFINED_
   typedef unsigned long      uInt32;
#endif
#ifndef _NI_float32_DEFINED_
#define _NI_float32_DEFINED_
   typedef float              float32;
#endif
#ifndef _NI_float64_DEFINED_
#define _NI_float64_DEFINED_
   typedef double             float64;
#endif
#ifndef _NI_int64_DEFINED_
#define _NI_int64_DEFINED_
   #if defined WIN32
      typedef __int64         int64;
   #else
      typedef long long       int64;
   #endif
#endif
#ifndef _NI_uInt64_DEFINED_
#define _NI_uInt64_DEFINED_
   #if defined WIN32
      typedef unsigned __int64    uInt64;
   #else
      typedef unsigned long long  uInt64;
   #endif
#endif

typedef uInt32             bool32;
// Original DAQmx is:
// typedef uInt32             TaskHandle;
typedef uInt32             *TaskHandle;

#ifndef TRUE
 #define TRUE            (1L)
#endif
#ifndef FALSE
 #define FALSE           (0L)
#endif
#ifndef NULL
 #define NULL            (0L)
#endif

/******************************************************************************
 *** NI-DAQmx Attributes ******************************************************
 ******************************************************************************/

//********** Calibration Info Attributes **********
#define DAQmx_SelfCal_Supported                                          0x1860 // Indicates whether the device supports self calibration.
#define DAQmx_SelfCal_LastTemp                                           0x1864 // Indicates in degrees Celsius the temperature of the device at the time of the last self calibration. Compare this temperature to the current onboard temperature to determine if you should perform another calibration.
#define DAQmx_ExtCal_RecommendedInterval                                 0x1868 // Indicates in months the National Instruments recommended interval between each external calibration of the device.
#define DAQmx_ExtCal_LastTemp                                            0x1867 // Indicates in degrees Celsius the temperature of the device at the time of the last external calibration. Compare this temperature to the current onboard temperature to determine if you should perform another calibration.
#define DAQmx_Cal_UserDefinedInfo                                        0x1861 // Specifies a string that contains arbitrary, user-defined information. This number of characters in this string can be no more than Max Size.
#define DAQmx_Cal_UserDefinedInfo_MaxSize                                0x191C // Indicates the maximum length in characters of Information.

//********** Channel Attributes **********
#define DAQmx_ChanType                                                   0x187F // Indicates the type of the virtual channel.
#define DAQmx_PhysicalChanName                                           0x18F5 // Indicates the name of the physical channel upon which this virtual channel is based.
#define DAQmx_ChanDescr                                                  0x1926 // Specifies a user-defined description for the channel.
#define DAQmx_AI_Max                                                     0x17DD // Specifies the maximum value you expect to measure. This value is in the units you specify with a units property. When you query this property, it returns the coerced maximum value that the device can measure with the current settings.
#define DAQmx_AI_Min                                                     0x17DE // Specifies the minimum value you expect to measure. This value is in the units you specify with a units property.  When you query this property, it returns the coerced minimum value that the device can measure with the current settings.
#define DAQmx_AI_CustomScaleName                                         0x17E0 // Specifies the name of a custom scale for the channel.
#define DAQmx_AI_MeasType                                                0x0695 // Indicates the measurement to take with the analog input channel and in some cases, such as for temperature measurements, the sensor to use.
#define DAQmx_AI_Voltage_Units                                           0x1094 // Specifies the units to use to return voltage measurements from the channel.
#define DAQmx_AI_Temp_Units                                              0x1033 // Specifies the units to use to return temperature measurements from the channel.
#define DAQmx_AI_Thrmcpl_Type                                            0x1050 // Specifies the type of thermocouple connected to the channel. Thermocouple types differ in composition and measurement range.
#define DAQmx_AI_Thrmcpl_CJCSrc                                          0x1035 // Indicates the source of cold-junction compensation.
#define DAQmx_AI_Thrmcpl_CJCVal                                          0x1036 // Specifies the temperature of the cold junction if CJC Source is DAQmx_Val_ConstVal. Specify this value in the units of the measurement.
#define DAQmx_AI_Thrmcpl_CJCChan                                         0x1034 // Indicates the channel that acquires the temperature of the cold junction if CJC Source is DAQmx_Val_Chan. If the channel does not use a custom scale, NI-DAQmx uses the correct units. If the channel uses a custom scale, the pre-scaled units of the channel must be degrees Celsius.
#define DAQmx_AI_RTD_Type                                                0x1032 // Specifies the type of RTD connected to the channel.
#define DAQmx_AI_RTD_R0                                                  0x1030 // Specifies in ohms the sensor resistance at 0 deg C. The Callendar-Van Dusen equation requires this value. Refer to the sensor documentation to determine this value.
#define DAQmx_AI_RTD_A                                                   0x1010 // Specifies the 'A' constant of the Callendar-Van Dusen equation. NI-DAQmx requires this value when you use a custom RTD.
#define DAQmx_AI_RTD_B                                                   0x1011 // Specifies the 'B' constant of the Callendar-Van Dusen equation. NI-DAQmx requires this value when you use a custom RTD.
#define DAQmx_AI_RTD_C                                                   0x1013 // Specifies the 'C' constant of the Callendar-Van Dusen equation. NI-DAQmx requires this value when you use a custom RTD.
#define DAQmx_AI_Thrmstr_A                                               0x18C9 // Specifies the 'A' constant of the Steinhart-Hart thermistor equation.
#define DAQmx_AI_Thrmstr_B                                               0x18CB // Specifies the 'B' constant of the Steinhart-Hart thermistor equation.
#define DAQmx_AI_Thrmstr_C                                               0x18CA // Specifies the 'C' constant of the Steinhart-Hart thermistor equation.
#define DAQmx_AI_Thrmstr_R1                                              0x1061 // Specifies in ohms the value of the reference resistor if you use voltage excitation. NI-DAQmx ignores this value for current excitation.
#define DAQmx_AI_ForceReadFromChan                                       0x18F8 // Specifies whether to read from the channel if it is a cold-junction compensation channel. By default, an NI-DAQmx Read function does not return data from cold-junction compensation channels.  Setting this property to TRUE forces read operations to return the cold-junction compensation channel data with the other channels in the task.
#define DAQmx_AI_Current_Units                                           0x0701 // Specifies the units to use to return current measurements from the channel.
#define DAQmx_AI_Strain_Units                                            0x0981 // Specifies the units to use to return strain measurements from the channel.
#define DAQmx_AI_StrainGage_GageFactor                                   0x0994 // Specifies the sensitivity of the strain gage.  Gage factor relates the change in electrical resistance to the change in strain. Refer to the sensor documentation for this value.
#define DAQmx_AI_StrainGage_PoissonRatio                                 0x0998 // Specifies the ratio of lateral strain to axial strain in the material you are measuring.
#define DAQmx_AI_StrainGage_Cfg                                          0x0982 // Specifies the bridge configuration of the strain gages.
#define DAQmx_AI_Resistance_Units                                        0x0955 // Specifies the units to use to return resistance measurements.
#define DAQmx_AI_Freq_Units                                              0x0806 // Specifies the units to use to return frequency measurements from the channel.
#define DAQmx_AI_Freq_ThreshVoltage                                      0x0815 // Specifies the voltage level at which to recognize waveform repetitions. You should select a voltage level that occurs only once within the entire period of a waveform. You also can select a voltage that occurs only once while the voltage rises or falls.
#define DAQmx_AI_Freq_Hyst                                               0x0814 // Specifies in volts a window below Threshold Level. The input voltage must pass below Threshold Level minus this value before NI-DAQmx recognizes a waveform repetition at Threshold Level. Hysteresis can improve the measurement accuracy when the signal contains noise or jitter.
#define DAQmx_AI_LVDT_Units                                              0x0910 // Specifies the units to use to return linear position measurements from the channel.
#define DAQmx_AI_LVDT_Sensitivity                                        0x0939 // Specifies the sensitivity of the LVDT. This value is in the units you specify with Sensitivity Units. Refer to the sensor documentation to determine this value.
#define DAQmx_AI_LVDT_SensitivityUnits                                   0x219A // Specifies the units of Sensitivity.
#define DAQmx_AI_RVDT_Units                                              0x0877 // Specifies the units to use to return angular position measurements from the channel.
#define DAQmx_AI_RVDT_Sensitivity                                        0x0903 // Specifies the sensitivity of the RVDT. This value is in the units you specify with Sensitivity Units. Refer to the sensor documentation to determine this value.
#define DAQmx_AI_RVDT_SensitivityUnits                                   0x219B // Specifies the units of Sensitivity.
#define DAQmx_AI_Accel_Units                                             0x0673 // Specifies the units to use to return acceleration measurements from the channel.
#define DAQmx_AI_Accel_Sensitivity                                       0x0692 // Specifies the sensitivity of the accelerometer. This value is in the units you specify with Sensitivity Units. Refer to the sensor documentation to determine this value.
#define DAQmx_AI_Accel_SensitivityUnits                                  0x219C // Specifies the units of Sensitivity.
#define DAQmx_AI_Coupling                                                0x0064 // Specifies the coupling for the channel.
#define DAQmx_AI_Impedance                                               0x0062 // Specifies the input impedance of the channel.
#define DAQmx_AI_TermCfg                                                 0x1097 // Specifies the terminal configuration for the channel.
#define DAQmx_AI_ResistanceCfg                                           0x1881 // Specifies the resistance configuration for the channel. NI-DAQmx uses this value for any resistance-based measurements, including temperature measurement using a thermistor or RTD.
#define DAQmx_AI_LeadWireResistance                                      0x17EE // Specifies in ohms the resistance of the wires that lead to the sensor.
#define DAQmx_AI_Bridge_Cfg                                              0x0087 // Specifies the type of Wheatstone bridge that the sensor is.
#define DAQmx_AI_Bridge_NomResistance                                    0x17EC // Specifies in ohms the resistance across each arm of the bridge in an unloaded position.
#define DAQmx_AI_Bridge_InitialVoltage                                   0x17ED // Specifies in volts the output voltage of the bridge in the unloaded condition. NI-DAQmx subtracts this value from any measurements before applying scaling equations.
#define DAQmx_AI_Bridge_ShuntCal_Enable                                  0x0094 // Specifies whether to enable a shunt calibration switch. Use Shunt Cal Select to select the switch(es) to enable.
#define DAQmx_AI_Bridge_ShuntCal_Select                                  0x21D5 // Specifies which shunt calibration switch(es) to enable.  Use Shunt Cal Enable to enable the switch(es) you specify with this property.
#define DAQmx_AI_Bridge_ShuntCal_GainAdjust                              0x193F // Specifies the result of a shunt calibration. NI-DAQmx multiplies data read from the channel by the value of this property. This value should be close to 1.0.
#define DAQmx_AI_Bridge_Balance_CoarsePot                                0x17F1 // Specifies by how much to compensate for offset in the signal. This value can be between 0 and 127.
#define DAQmx_AI_Bridge_Balance_FinePot                                  0x18F4 // Specifies by how much to compensate for offset in the signal. This value can be between 0 and 4095.
#define DAQmx_AI_CurrentShunt_Loc                                        0x17F2 // Specifies the shunt resistor location for current measurements.
#define DAQmx_AI_CurrentShunt_Resistance                                 0x17F3 // Specifies in ohms the external shunt resistance for current measurements.
#define DAQmx_AI_Excit_Src                                               0x17F4 // Specifies the source of excitation.
#define DAQmx_AI_Excit_Val                                               0x17F5 // Specifies the amount of excitation that the sensor requires. If Voltage or Current is  DAQmx_Val_Voltage, this value is in volts. If Voltage or Current is  DAQmx_Val_Current, this value is in amperes.
#define DAQmx_AI_Excit_UseForScaling                                     0x17FC // Specifies if NI-DAQmx divides the measurement by the excitation. You should typically set this property to TRUE for ratiometric transducers. If you set this property to TRUE, set Maximum Value and Minimum Value to reflect the scaling.
#define DAQmx_AI_Excit_UseMultiplexed                                    0x2180 // Specifies if the SCXI-1122 multiplexes the excitation to the upper half of the channels as it advances through the scan list.
#define DAQmx_AI_Excit_ActualVal                                         0x1883 // Specifies the actual amount of excitation supplied by an internal excitation source.  If you read an internal excitation source more precisely with an external device, set this property to the value you read.  NI-DAQmx ignores this value for external excitation.
#define DAQmx_AI_Excit_DCorAC                                            0x17FB // Specifies if the excitation supply is DC or AC.
#define DAQmx_AI_Excit_VoltageOrCurrent                                  0x17F6 // Specifies if the channel uses current or voltage excitation.
#define DAQmx_AI_ACExcit_Freq                                            0x0101 // Specifies the AC excitation frequency in Hertz.
#define DAQmx_AI_ACExcit_SyncEnable                                      0x0102 // Specifies whether to synchronize the AC excitation source of the channel to that of another channel. Synchronize the excitation sources of multiple channels to use multichannel sensors. Set this property to FALSE for the master channel and to TRUE for the slave channels.
#define DAQmx_AI_ACExcit_WireMode                                        0x18CD // Specifies the number of leads on the LVDT or RVDT. Some sensors require you to tie leads together to create a four- or five- wire sensor. Refer to the sensor documentation for more information.
#define DAQmx_AI_Atten                                                   0x1801 // Specifies the amount of attenuation to use.
#define DAQmx_AI_Lowpass_Enable                                          0x1802 // Specifies whether to enable the lowpass filter of the channel.
#define DAQmx_AI_Lowpass_CutoffFreq                                      0x1803 // Specifies the frequency in Hertz that corresponds to the -3dB cutoff of the filter.
#define DAQmx_AI_Lowpass_SwitchCap_ClkSrc                                0x1884 // Specifies the source of the filter clock. If you need a higher resolution for the filter, you can supply an external clock to increase the resolution. Refer to the SCXI-1141/1142/1143 User Manual for more information.
#define DAQmx_AI_Lowpass_SwitchCap_ExtClkFreq                            0x1885 // Specifies the frequency of the external clock when you set Clock Source to DAQmx_Val_External.  NI-DAQmx uses this frequency to set the pre- and post- filters on the SCXI-1141, SCXI-1142, and SCXI-1143. On those devices, NI-DAQmx determines the filter cutoff by using the equation f/(100*n), where f is the external frequency, and n is the external clock divisor. Refer to the SCXI-1141/1142/1143 User Manual for more...
#define DAQmx_AI_Lowpass_SwitchCap_ExtClkDiv                             0x1886 // Specifies the divisor for the external clock when you set Clock Source to DAQmx_Val_External. On the SCXI-1141, SCXI-1142, and SCXI-1143, NI-DAQmx determines the filter cutoff by using the equation f/(100*n), where f is the external frequency, and n is the external clock divisor. Refer to the SCXI-1141/1142/1143 User Manual for more information.
#define DAQmx_AI_Lowpass_SwitchCap_OutClkDiv                             0x1887 // Specifies the divisor for the output clock.  NI-DAQmx uses the cutoff frequency to determine the output clock frequency. Refer to the SCXI-1141/1142/1143 User Manual for more information.
#define DAQmx_AI_ResolutionUnits                                         0x1764 // Indicates the units of Resolution Value.
#define DAQmx_AI_Resolution                                              0x1765 // Indicates the resolution of the analog-to-digital converter of the channel. This value is in the units you specify with Resolution Units.
#define DAQmx_AI_Dither_Enable                                           0x0068 // Specifies whether to enable dithering.  Dithering adds Gaussian noise to the input signal. You can use dithering to achieve higher resolution measurements by over sampling the input signal and averaging the results.
#define DAQmx_AI_Rng_High                                                0x1815 // Specifies the upper limit of the input range of the device. This value is in the native units of the device. On E Series devices, for example, the native units is volts.
#define DAQmx_AI_Rng_Low                                                 0x1816 // Specifies the lower limit of the input range of the device. This value is in the native units of the device. On E Series devices, for example, the native units is volts.
#define DAQmx_AI_Gain                                                    0x1818 // Specifies a gain factor to apply to the channel.
#define DAQmx_AI_SampAndHold_Enable                                      0x181A // Specifies whether to enable the sample and hold circuitry of the device. When you disable sample and hold circuitry, a small voltage offset might be introduced into the signal.  You can eliminate this offset by using Auto Zero Mode to perform an auto zero on the channel.
#define DAQmx_AI_AutoZeroMode                                            0x1760 // Specifies when to measure ground. NI-DAQmx subtracts the measured ground voltage from every sample.
#define DAQmx_AI_DataXferMech                                            0x1821 // Specifies the data transfer mode for the device.
#define DAQmx_AI_DataXferReqCond                                         0x188B // Specifies under what condition to transfer data from the onboard memory of the device to the buffer.
#define DAQmx_AI_MemMapEnable                                            0x188C // Specifies for NI-DAQmx to map hardware registers to the memory space of the customer process, if possible. Mapping to the memory space of the customer process increases performance. However, memory mapping can adversely affect the operation of the device and possibly result in a system crash if software in the process unintentionally accesses the mapped registers.
#define DAQmx_AI_DevScalingCoeff                                         0x1930 // Indicates the coefficients of a polynomial equation that NI-DAQmx uses to scale values from the native format of the device to volts. Each element of the array corresponds to a term of the equation. For example, if index two of the array is 4, the third term of the equation is 4x^2. Scaling coefficients do not account for any custom scales or sensors contained by the channel.
#define DAQmx_AO_Max                                                     0x1186 // Specifies the maximum value you expect to generate. The value is in the units you specify with a units property. If you try to write a value larger than the maximum value, NI-DAQmx generates an error. NI-DAQmx might coerce this value to a smaller value if other task settings restrict the device from generating the desired maximum.
#define DAQmx_AO_Min                                                     0x1187 // Specifies the minimum value you expect to generate. The value is in the units you specify with a units property. If you try to write a value smaller than the minimum value, NI-DAQmx generates an error. NI-DAQmx might coerce this value to a larger value if other task settings restrict the device from generating the desired minimum.
#define DAQmx_AO_CustomScaleName                                         0x1188 // Specifies the name of a custom scale for the channel.
#define DAQmx_AO_OutputType                                              0x1108 // Indicates whether the channel generates voltage or current.
#define DAQmx_AO_Voltage_Units                                           0x1184 // Specifies in what units to generate voltage on the channel. Write data to the channel in the units you select.
#define DAQmx_AO_Current_Units                                           0x1109 // Specifies in what units to generate current on the channel. Write data to the channel is in the units you select.
#define DAQmx_AO_OutputImpedance                                         0x1490 // Specifies in ohms the impedance of the analog output stage of the device.
#define DAQmx_AO_LoadImpedance                                           0x0121 // Specifies in ohms the load impedance connected to the analog output channel.
#define DAQmx_AO_ResolutionUnits                                         0x182B // Specifies the units of Resolution Value.
#define DAQmx_AO_Resolution                                              0x182C // Indicates the resolution of the digital-to-analog converter of the channel. This value is in the units you specify with Resolution Units.
#define DAQmx_AO_DAC_Rng_High                                            0x182E // Specifies the upper limit of the output range of the device. This value is in the native units of the device. On E Series devices, for example, the native units is volts.
#define DAQmx_AO_DAC_Rng_Low                                             0x182D // Specifies the lower limit of the output range of the device. This value is in the native units of the device. On E Series devices, for example, the native units is volts.
#define DAQmx_AO_DAC_Ref_ConnToGnd                                       0x0130 // Specifies whether to ground the internal DAC reference. Grounding the internal DAC reference has the effect of grounding all analog output channels and stopping waveform generation across all analog output channels regardless of whether the channels belong to the current task. You can ground the internal DAC reference only when Source is DAQmx_Val_Internal and Allow Connecting DAC Reference to Ground at Runtime is...
#define DAQmx_AO_DAC_Ref_AllowConnToGnd                                  0x1830 // Specifies whether to allow grounding the internal DAC reference at run time. You must set this property to TRUE and set Source to DAQmx_Val_Internal before you can set Connect DAC Reference to Ground to TRUE.
#define DAQmx_AO_DAC_Ref_Src                                             0x0132 // Specifies the source of the DAC reference voltage.  The value of this voltage source determines the full-scale value of the DAC.
#define DAQmx_AO_DAC_Ref_Val                                             0x1832 // Specifies in volts the value of the DAC reference voltage. This voltage determines the full-scale range of the DAC. Smaller reference voltages result in smaller ranges, but increased resolution.
#define DAQmx_AO_ReglitchEnable                                          0x0133 // Specifies whether to enable reglitching.  The output of a DAC normally glitches whenever the DAC is updated with a new value. The amount of glitching differs from code to code and is generally largest at major code transitions.  Reglitching generates uniform glitch energy at each code transition and provides for more uniform glitches.  Uniform glitch energy makes it easier to filter out the noise introduced from g...
#define DAQmx_AO_UseOnlyOnBrdMem                                         0x183A // Specifies whether to write samples directly to the onboard memory of the device, bypassing the memory buffer. Generally, you cannot update onboard memory after you start the task. Onboard memory includes data FIFOs.
#define DAQmx_AO_DataXferMech                                            0x0134 // Specifies the data transfer mode for the device.
#define DAQmx_AO_DataXferReqCond                                         0x183C // Specifies under what condition to transfer data from the buffer to the onboard memory of the device.
#define DAQmx_AO_MemMapEnable                                            0x188F // Specifies if NI-DAQmx maps hardware registers to the memory space of the customer process, if possible. Mapping to the memory space of the customer process increases performance. However, memory mapping can adversely affect the operation of the device and possibly result in a system crash if software in the process unintentionally accesses the mapped registers.
#define DAQmx_AO_DevScalingCoeff                                         0x1931 // Indicates the coefficients of a linear equation that NI-DAQmx uses to scale values from a voltage to the native format of the device.  Each element of the array corresponds to a term of the equation. For example, if index two of the array is 4, the third term of the equation is 4x^2.  Scaling coefficients do not account for any custom scales that may be applied to the channel.
#define DAQmx_DI_InvertLines                                             0x0793 // Specifies whether to invert the lines in the channel. If you set this property to TRUE, the lines are at high logic when off and at low logic when on.
#define DAQmx_DI_NumLines                                                0x2178 // Indicates the number of digital lines in the channel.
#define DAQmx_DI_DigFltr_Enable                                          0x21D6 // Specifies whether to enable the digital filter for the line(s) or port(s). You can enable the filter on a line-by-line basis. You do not have to enable the filter for all lines in a channel.
#define DAQmx_DI_DigFltr_MinPulseWidth                                   0x21D7 // Specifies in seconds the minimum pulse width the filter recognizes as a valid high or low state transition.
#define DAQmx_DO_InvertLines                                             0x1133 // Specifies whether to invert the lines in the channel. If you set this property to TRUE, the lines are at high logic when off and at low logic when on.
#define DAQmx_DO_NumLines                                                0x2179 // Indicates the number of digital lines in the channel.
#define DAQmx_DO_Tristate                                                0x18F3 // Specifies whether to stop driving the channel and set it to a Hi-Z state.
#define DAQmx_CI_Max                                                     0x189C // Specifies the maximum value you expect to measure. This value is in the units you specify with a units property. When you query this property, it returns the coerced maximum value that the hardware can measure with the current settings.
#define DAQmx_CI_Min                                                     0x189D // Specifies the minimum value you expect to measure. This value is in the units you specify with a units property. When you query this property, it returns the coerced minimum value that the hardware can measure with the current settings.
#define DAQmx_CI_CustomScaleName                                         0x189E // Specifies the name of a custom scale for the channel.
#define DAQmx_CI_MeasType                                                0x18A0 // Indicates the measurement to take with the channel.
#define DAQmx_CI_Freq_Units                                              0x18A1 // Specifies the units to use to return frequency measurements.
#define DAQmx_CI_Freq_Term                                               0x18A2 // Specifies the input terminal of the signal to measure.
#define DAQmx_CI_Freq_StartingEdge                                       0x0799 // Specifies between which edges to measure the frequency of the signal.
#define DAQmx_CI_Freq_MeasMeth                                           0x0144 // Specifies the method to use to measure the frequency of the signal.
#define DAQmx_CI_Freq_MeasTime                                           0x0145 // Specifies in seconds the length of time to measure the frequency of the signal if Method is DAQmx_Val_HighFreq2Ctr. Measurement accuracy increases with increased measurement time and with increased signal frequency. If you measure a high-frequency signal for too long, however, the count register could roll over, which results in an incorrect measurement.
#define DAQmx_CI_Freq_Div                                                0x0147 // Specifies the value by which to divide the input signal if  Method is DAQmx_Val_LargeRng2Ctr. The larger the divisor, the more accurate the measurement. However, too large a value could cause the count register to roll over, which results in an incorrect measurement.
#define DAQmx_CI_Period_Units                                            0x18A3 // Specifies the unit to use to return period measurements.
#define DAQmx_CI_Period_Term                                             0x18A4 // Specifies the input terminal of the signal to measure.
#define DAQmx_CI_Period_StartingEdge                                     0x0852 // Specifies between which edges to measure the period of the signal.
#define DAQmx_CI_Period_MeasMeth                                         0x192C // Specifies the method to use to measure the period of the signal.
#define DAQmx_CI_Period_MeasTime                                         0x192D // Specifies in seconds the length of time to measure the period of the signal if Method is DAQmx_Val_HighFreq2Ctr. Measurement accuracy increases with increased measurement time and with increased signal frequency. If you measure a high-frequency signal for too long, however, the count register could roll over, which results in an incorrect measurement.
#define DAQmx_CI_Period_Div                                              0x192E // Specifies the value by which to divide the input signal if Method is DAQmx_Val_LargeRng2Ctr. The larger the divisor, the more accurate the measurement. However, too large a value could cause the count register to roll over, which results in an incorrect measurement.
#define DAQmx_CI_CountEdges_Term                                         0x18C7 // Specifies the input terminal of the signal to measure.
#define DAQmx_CI_CountEdges_Dir                                          0x0696 // Specifies whether to increment or decrement the counter on each edge.
#define DAQmx_CI_CountEdges_DirTerm                                      0x21E1 // Specifies the source terminal of the digital signal that controls the count direction if Direction is DAQmx_Val_ExtControlled.
#define DAQmx_CI_CountEdges_InitialCnt                                   0x0698 // Specifies the starting value from which to count.
#define DAQmx_CI_CountEdges_ActiveEdge                                   0x0697 // Specifies on which edges to increment or decrement the counter.
#define DAQmx_CI_AngEncoder_Units                                        0x18A6 // Specifies the units to use to return angular position measurements from the channel.
#define DAQmx_CI_AngEncoder_PulsesPerRev                                 0x0875 // Specifies the number of pulses the encoder generates per revolution. This value is the number of pulses on either signal A or signal B, not the total number of pulses on both signal A and signal B.
#define DAQmx_CI_AngEncoder_InitialAngle                                 0x0881 // Specifies the starting angle of the encoder. This value is in the units you specify with Units.
#define DAQmx_CI_LinEncoder_Units                                        0x18A9 // Specifies the units to use to return linear encoder measurements from the channel.
#define DAQmx_CI_LinEncoder_DistPerPulse                                 0x0911 // Specifies the distance to measure for each pulse the encoder generates on signal A or signal B. This value is in the units you specify with Units.
#define DAQmx_CI_LinEncoder_InitialPos                                   0x0915 // Specifies the position of the encoder when the measurement begins. This value is in the units you specify with Units.
#define DAQmx_CI_Encoder_DecodingType                                    0x21E6 // Specifies how to count and interpret the pulses the encoder generates on signal A and signal B. DAQmx_Val_X1, DAQmx_Val_X2, and DAQmx_Val_X4 are valid for quadrature encoders only. DAQmx_Val_TwoPulseCounting is valid for two-pulse encoders only.
#define DAQmx_CI_Encoder_AInputTerm                                      0x219D // Specifies the terminal to which signal A is connected.
#define DAQmx_CI_Encoder_BInputTerm                                      0x219E // Specifies the terminal to which signal B is connected.
#define DAQmx_CI_Encoder_ZInputTerm                                      0x219F // Specifies the terminal to which signal Z is connected.
#define DAQmx_CI_Encoder_ZIndexEnable                                    0x0890 // Specifies whether to use Z indexing for the channel.
#define DAQmx_CI_Encoder_ZIndexVal                                       0x0888 // Specifies the value to which to reset the measurement when signal Z is high and signal A and signal B are at the states you specify with Z Index Phase. Specify this value in the units of the measurement.
#define DAQmx_CI_Encoder_ZIndexPhase                                     0x0889 // Specifies the states at which signal A and signal B must be while signal Z is high for NI-DAQmx to reset the measurement. If signal Z is never high while signal A and signal B are high, for example, you must choose a phase other than DAQmx_Val_AHighBHigh.
#define DAQmx_CI_PulseWidth_Units                                        0x0823 // Specifies the units to use to return pulse width measurements.
#define DAQmx_CI_PulseWidth_Term                                         0x18AA // Specifies the input terminal of the signal to measure.
#define DAQmx_CI_PulseWidth_StartingEdge                                 0x0825 // Specifies on which edge of the input signal to begin each pulse width measurement.
#define DAQmx_CI_TwoEdgeSep_Units                                        0x18AC // Specifies the units to use to return two-edge separation measurements from the channel.
#define DAQmx_CI_TwoEdgeSep_FirstTerm                                    0x18AD // Specifies the source terminal of the digital signal that starts each measurement.
#define DAQmx_CI_TwoEdgeSep_FirstEdge                                    0x0833 // Specifies on which edge of the first signal to start each measurement.
#define DAQmx_CI_TwoEdgeSep_SecondTerm                                   0x18AE // Specifies the source terminal of the digital signal that stops each measurement.
#define DAQmx_CI_TwoEdgeSep_SecondEdge                                   0x0834 // Specifies on which edge of the second signal to stop each measurement.
#define DAQmx_CI_SemiPeriod_Units                                        0x18AF // Specifies the units to use to return semi-period measurements.
#define DAQmx_CI_SemiPeriod_Term                                         0x18B0 // Specifies the input terminal of the signal to measure.
#define DAQmx_CI_CtrTimebaseSrc                                          0x0143 // Specifies the terminal of the timebase to use for the counter.
#define DAQmx_CI_CtrTimebaseRate                                         0x18B2 // Specifies in Hertz the frequency of the counter timebase. Specifying the rate of a counter timebase allows you to take measurements in terms of time or frequency rather than in ticks of the timebase. If you use an external timebase and do not specify the rate, you can take measurements only in terms of ticks of the timebase.
#define DAQmx_CI_CtrTimebaseActiveEdge                                   0x0142 // Specifies whether a timebase cycle is from rising edge to rising edge or from falling edge to falling edge.
#define DAQmx_CI_Count                                                   0x0148 // Indicates the current value of the count register.
#define DAQmx_CI_OutputState                                             0x0149 // Indicates the current state of the out terminal of the counter.
#define DAQmx_CI_TCReached                                               0x0150 // Indicates whether the counter rolled over. When you query this property, NI-DAQmx resets it to FALSE.
#define DAQmx_CI_CtrTimebaseMasterTimebaseDiv                            0x18B3 // Specifies the divisor for an external counter timebase. You can divide the counter timebase in order to measure slower signals without causing the count register to roll over.
#define DAQmx_CI_DataXferMech                                            0x0200 // Specifies the data transfer mode for the channel.
#define DAQmx_CI_NumPossiblyInvalidSamps                                 0x193C // Indicates the number of samples that the device might have overwritten before it could transfer them to the buffer.
#define DAQmx_CI_DupCountPrevent                                         0x21AC // Specifies whether to enable duplicate count prevention for the channel.
#define DAQmx_CO_OutputType                                              0x18B5 // Indicates how to define pulses generated on the channel.
#define DAQmx_CO_Pulse_IdleState                                         0x1170 // Specifies the resting state of the output terminal.
#define DAQmx_CO_Pulse_Term                                              0x18E1 // Specifies on which terminal to generate pulses.
#define DAQmx_CO_Pulse_Time_Units                                        0x18D6 // Specifies the units in which to define high and low pulse time.
#define DAQmx_CO_Pulse_HighTime                                          0x18BA // Specifies the amount of time that the pulse is at a high voltage. This value is in the units you specify with Units or when you create the channel.
#define DAQmx_CO_Pulse_LowTime                                           0x18BB // Specifies the amount of time that the pulse is at a low voltage. This value is in the units you specify with Units or when you create the channel.
#define DAQmx_CO_Pulse_Time_InitialDelay                                 0x18BC // Specifies in seconds the amount of time to wait before generating the first pulse.
#define DAQmx_CO_Pulse_DutyCyc                                           0x1176 // Specifies the duty cycle of the pulses. The duty cycle of a signal is the width of the pulse divided by period. NI-DAQmx uses this ratio and the pulse frequency to determine the width of the pulses and the delay between pulses.
#define DAQmx_CO_Pulse_Freq_Units                                        0x18D5 // Specifies the units in which to define pulse frequency.
#define DAQmx_CO_Pulse_Freq                                              0x1178 // Specifies the frequency of the pulses to generate. This value is in the units you specify with Units or when you create the channel.
#define DAQmx_CO_Pulse_Freq_InitialDelay                                 0x0299 // Specifies in seconds the amount of time to wait before generating the first pulse.
#define DAQmx_CO_Pulse_HighTicks                                         0x1169 // Specifies the number of ticks the pulse is high.
#define DAQmx_CO_Pulse_LowTicks                                          0x1171 // Specifies the number of ticks the pulse is low.
#define DAQmx_CO_Pulse_Ticks_InitialDelay                                0x0298 // Specifies the number of ticks to wait before generating the first pulse.
#define DAQmx_CO_CtrTimebaseSrc                                          0x0339 // Specifies the terminal of the timebase to use for the counter. Typically, NI-DAQmx uses one of the internal counter timebases when generating pulses. Use this property to specify an external timebase and produce custom pulse widths that are not possible using the internal timebases.
#define DAQmx_CO_CtrTimebaseRate                                         0x18C2 // Specifies in Hertz the frequency of the counter timebase. Specifying the rate of a counter timebase allows you to define output pulses in seconds rather than in ticks of the timebase. If you use an external timebase and do not specify the rate, you can define output pulses only in ticks of the timebase.
#define DAQmx_CO_CtrTimebaseActiveEdge                                   0x0341 // Specifies whether a timebase cycle is from rising edge to rising edge or from falling edge to falling edge.
#define DAQmx_CO_Count                                                   0x0293 // Indicates the current value of the count register.
#define DAQmx_CO_OutputState                                             0x0294 // Indicates the current state of the output terminal of the counter.
#define DAQmx_CO_AutoIncrCnt                                             0x0295 // Specifies a number of timebase ticks by which to increment each successive pulse.
#define DAQmx_CO_CtrTimebaseMasterTimebaseDiv                            0x18C3 // Specifies the divisor for an external counter timebase. You can divide the counter timebase in order to generate slower signals without causing the count register to roll over.
#define DAQmx_CO_PulseDone                                               0x190E // Indicates if the task completed pulse generation. Use this value for retriggerable pulse generation when you need to determine if the device generated the current pulse. When you query this property, NI-DAQmx resets it to FALSE.

//********** Export Signal Attributes **********
#define DAQmx_Exported_AIConvClk_OutputTerm                              0x1687 // Specifies the terminal to which to route the AI Convert Clock.
#define DAQmx_Exported_AIConvClk_Pulse_Polarity                          0x1688 // Indicates the polarity of the exported AI Convert Clock. The polarity is fixed and independent of the active edge of the source of the AI Convert Clock.
#define DAQmx_Exported_20MHzTimebase_OutputTerm                          0x1657 // Specifies the terminal to which to route the 20MHz Timebase.
#define DAQmx_Exported_SampClk_OutputBehavior                            0x186B // Specifies whether the exported Sample Clock issues a pulse at the beginning of a sample or changes to a high state for the duration of the sample.
#define DAQmx_Exported_SampClk_OutputTerm                                0x1663 // Specifies the terminal to which to route the Sample Clock.
#define DAQmx_Exported_AdvTrig_OutputTerm                                0x1645 // Specifies the terminal to which to route the Advance Trigger.
#define DAQmx_Exported_AdvTrig_Pulse_Polarity                            0x1646 // Indicates the polarity of the exported Advance Trigger.
#define DAQmx_Exported_AdvTrig_Pulse_WidthUnits                          0x1647 // Specifies the units of Width Value.
#define DAQmx_Exported_AdvTrig_Pulse_Width                               0x1648 // Specifies the width of an exported Advance Trigger pulse. Specify this value in the units you specify with Width Units.
#define DAQmx_Exported_RefTrig_OutputTerm                                0x0590 // Specifies the terminal to which to route the Reference Trigger.
#define DAQmx_Exported_StartTrig_OutputTerm                              0x0584 // Specifies the terminal to which to route the Start Trigger.
#define DAQmx_Exported_AdvCmpltEvent_OutputTerm                          0x1651 // Specifies the terminal to which to route the Advance Complete Event.
#define DAQmx_Exported_AdvCmpltEvent_Delay                               0x1757 // Specifies the output signal delay in periods of the sample clock.
#define DAQmx_Exported_AdvCmpltEvent_Pulse_Polarity                      0x1652 // Specifies the polarity of the exported Advance Complete Event.
#define DAQmx_Exported_AdvCmpltEvent_Pulse_Width                         0x1654 // Specifies the width of the exported Advance Complete Event pulse.
#define DAQmx_Exported_AIHoldCmpltEvent_OutputTerm                       0x18ED // Specifies the terminal to which to route the AI Hold Complete Event.
#define DAQmx_Exported_AIHoldCmpltEvent_PulsePolarity                    0x18EE // Specifies the polarity of an exported AI Hold Complete Event pulse.
#define DAQmx_Exported_ChangeDetectEvent_OutputTerm                      0x2197 // Specifies the terminal to which to route the Change Detection Event.
#define DAQmx_Exported_CtrOutEvent_OutputTerm                            0x1717 // Specifies the terminal to which to route the Counter Output Event.
#define DAQmx_Exported_CtrOutEvent_OutputBehavior                        0x174F // Specifies whether the exported Counter Output Event pulses or changes from one state to the other when the counter reaches terminal count.
#define DAQmx_Exported_CtrOutEvent_Pulse_Polarity                        0x1718 // Specifies the polarity of the pulses at the output terminal of the counter when Output Behavior is DAQmx_Val_Pulse. NI-DAQmx ignores this property if Output Behavior is DAQmx_Val_Toggle.
#define DAQmx_Exported_CtrOutEvent_Toggle_IdleState                      0x186A // Specifies the initial state of the output terminal of the counter when Output Behavior is DAQmx_Val_Toggle. The terminal enters this state when NI-DAQmx commits the task.
#define DAQmx_Exported_WatchdogExpiredEvent_OutputTerm                   0x21AA // Specifies the terminal  to which to route the Watchdog Timer Expired Event.

//********** Device Attributes **********
#define DAQmx_Dev_ProductType                                            0x0631 // Indicates the product name of the device.
#define DAQmx_Dev_SerialNum                                              0x0632 // Indicates the serial number of the device. This value is zero if the device does not have a serial number.

//********** Read Attributes **********
#define DAQmx_Read_RelativeTo                                            0x190A // Specifies the point in the buffer at which to begin a read operation. If you also specify an offset with Offset, the read operation begins at that offset relative to the point you select with this property. The default value is DAQmx_Val_CurrReadPos unless you configure a Reference Trigger for the task. If you configure a Reference Trigger, the default value is DAQmx_Val_FirstPretrigSamp.
#define DAQmx_Read_Offset                                                0x190B // Specifies an offset in samples per channel at which to begin a read operation. This offset is relative to the location you specify with RelativeTo.
#define DAQmx_Read_ChannelsToRead                                        0x1823 // Specifies a subset of channels in the task from which to read.
#define DAQmx_Read_ReadAllAvailSamp                                      0x1215 // Specifies whether subsequent read operations read all samples currently available in the buffer or wait for the buffer to become full before reading. NI-DAQmx uses this setting for finite acquisitions and only when the number of samples to read is -1. For continuous acquisitions when the number of samples to read is -1, a read operation always reads all samples currently available in the buffer.
#define DAQmx_Read_AutoStart                                             0x1826 // Specifies if an NI-DAQmx Read function automatically starts the task  if you did not start the task explicitly by using DAQmxStartTask(). The default value is TRUE. When  an NI-DAQmx Read function starts a finite acquisition task, it also stops the task after reading the last sample.
#define DAQmx_Read_OverWrite                                             0x1211 // Specifies whether to overwrite samples in the buffer that you have not yet read.
#define DAQmx_Read_CurrReadPos                                           0x1221 // Indicates in samples per channel the current position in the buffer.
#define DAQmx_Read_AvailSampPerChan                                      0x1223 // Indicates the number of samples available to read per channel. This value is the same for all channels in the task.
#define DAQmx_Read_TotalSampPerChanAcquired                              0x192A // Indicates the total number of samples acquired by each channel. NI-DAQmx returns a single value because this value is the same for all channels.
#define DAQmx_Read_ChangeDetect_HasOverflowed                            0x2194 // Indicates if samples were missed because change detection events occurred faster than the device could handle them.
#define DAQmx_Read_RawDataWidth                                          0x217A // Indicates in bytes the size of a raw sample from the task.
#define DAQmx_Read_NumChans                                              0x217B // Indicates the number of channels that an NI-DAQmx Read function reads from the task. This value is the number of channels in the task or the number of channels you specify with Channels to Read.
#define DAQmx_Read_DigitalLines_BytesPerChan                             0x217C // Indicates the number of bytes per channel that NI-DAQmx returns in a sample for line-based reads. If a channel has fewer lines than this number, the extra bytes are FALSE.

//********** Switch Channel Attributes **********
#define DAQmx_SwitchChan_Usage                                           0x18E4 // Specifies how you can use the channel. Using this property acts as a safety mechanism to prevent you from connecting two source channels, for example.
#define DAQmx_SwitchChan_MaxACCarryCurrent                               0x0648 // Indicates in amperes the maximum AC current that the device can carry.
#define DAQmx_SwitchChan_MaxACSwitchCurrent                              0x0646 // Indicates in amperes the maximum AC current that the device can switch. This current is always against an RMS voltage level.
#define DAQmx_SwitchChan_MaxACCarryPwr                                   0x0642 // Indicates in watts the maximum AC power that the device can carry.
#define DAQmx_SwitchChan_MaxACSwitchPwr                                  0x0644 // Indicates in watts the maximum AC power that the device can switch.
#define DAQmx_SwitchChan_MaxDCCarryCurrent                               0x0647 // Indicates in amperes the maximum DC current that the device can carry.
#define DAQmx_SwitchChan_MaxDCSwitchCurrent                              0x0645 // Indicates in amperes the maximum DC current that the device can switch. This current is always against a DC voltage level.
#define DAQmx_SwitchChan_MaxDCCarryPwr                                   0x0643 // Indicates in watts the maximum DC power that the device can carry.
#define DAQmx_SwitchChan_MaxDCSwitchPwr                                  0x0649 // Indicates in watts the maximum DC power that the device can switch.
#define DAQmx_SwitchChan_MaxACVoltage                                    0x0651 // Indicates in volts the maximum AC RMS voltage that the device can switch.
#define DAQmx_SwitchChan_MaxDCVoltage                                    0x0650 // Indicates in volts the maximum DC voltage that the device can switch.
#define DAQmx_SwitchChan_WireMode                                        0x18E5 // Indicates the number of wires that the channel switches.
#define DAQmx_SwitchChan_Bandwidth                                       0x0640 // Indicates in Hertz the maximum frequency of a signal that can pass through the switch without significant deterioration.
#define DAQmx_SwitchChan_Impedance                                       0x0641 // Indicates in ohms the switch impedance. This value is important in the RF domain and should match the impedance of the sources and loads.

//********** Switch Device Attributes **********
#define DAQmx_SwitchDev_SettlingTime                                     0x1244 // Specifies in seconds the amount of time to wait for the switch to settle (or debounce). Refer to device documentation for supported settling times.
#define DAQmx_SwitchDev_AutoConnAnlgBus                                  0x17DA // Specifies if NI-DAQmx routes multiplexed channels to the analog bus backplane. Only the SCXI-1127 and SCXI-1128 support this property.
#define DAQmx_SwitchDev_Settled                                          0x1243 // Indicates when Settling Time expires.
#define DAQmx_SwitchDev_RelayList                                        0x17DC // Indicates a comma-delimited list of relay names.
#define DAQmx_SwitchDev_NumRelays                                        0x18E6 // Indicates the number of relays on the device. This value matches the number of relay names in Relay List.
#define DAQmx_SwitchDev_SwitchChanList                                   0x18E7 // Indicates a comma-delimited list of channel names for the current topology of the device.
#define DAQmx_SwitchDev_NumSwitchChans                                   0x18E8 // Indicates the number of switch channels for the current topology of the device. This value matches the number of channel names in Switch Channel List.
#define DAQmx_SwitchDev_NumRows                                          0x18E9 // Indicates the number of rows on a device in a matrix switch topology. Indicates the number of multiplexed channels on a device in a mux topology.
#define DAQmx_SwitchDev_NumColumns                                       0x18EA // Indicates the number of columns on a device in a matrix switch topology. This value is always 1 if the device is in a mux topology.
#define DAQmx_SwitchDev_Topology                                         0x193D // Indicates the current topology of the device. This value is one of the topology options in DAQmxSwitchSetTopologyAndReset().

//********** Switch Scan Attributes **********
#define DAQmx_SwitchScan_BreakMode                                       0x1247 // Specifies the break mode between each entry in a scan list.
#define DAQmx_SwitchScan_RepeatMode                                      0x1248 // Specifies if the task advances through the scan list multiple times.
#define DAQmx_SwitchScan_WaitingForAdv                                   0x17D9 // Indicates if the switch hardware is waiting for an  Advance Trigger. If the hardware is waiting, it completed the previous entry in the scan list.

//********** Scale Attributes **********
#define DAQmx_Scale_Descr                                                0x1226 // Specifies a description for the scale.
#define DAQmx_Scale_ScaledUnits                                          0x191B // Specifies the units to use for scaled values. You can use an arbitrary string.
#define DAQmx_Scale_PreScaledUnits                                       0x18F7 // Specifies the units of the values that you want to scale.
#define DAQmx_Scale_Type                                                 0x1929 // Indicates the method or equation form that the custom scale uses.
#define DAQmx_Scale_Lin_Slope                                            0x1227 // Specifies the slope, m, in the equation y=mx+b.
#define DAQmx_Scale_Lin_YIntercept                                       0x1228 // Specifies the y-intercept, b, in the equation y=mx+b.
#define DAQmx_Scale_Map_ScaledMax                                        0x1229 // Specifies the largest value in the range of scaled values. NI-DAQmx maps this value to Pre-Scaled Maximum Value. Reads clip samples that are larger than this value. Writes generate errors for samples that are larger than this value.
#define DAQmx_Scale_Map_PreScaledMax                                     0x1231 // Specifies the largest value in the range of pre-scaled values. NI-DAQmx maps this value to Scaled Maximum Value.
#define DAQmx_Scale_Map_ScaledMin                                        0x1230 // Specifies the smallest value in the range of scaled values. NI-DAQmx maps this value to Pre-Scaled Minimum Value. Reads clip samples that are smaller than this value. Writes generate errors for samples that are smaller than this value.
#define DAQmx_Scale_Map_PreScaledMin                                     0x1232 // Specifies the smallest value in the range of pre-scaled values. NI-DAQmx maps this value to Scaled Minimum Value.
#define DAQmx_Scale_Poly_ForwardCoeff                                    0x1234 // Specifies an array of coefficients for the polynomial that converts pre-scaled values to scaled values. Each element of the array corresponds to a term of the equation. For example, if index three of the array is 9, the fourth term of the equation is 9x^3.
#define DAQmx_Scale_Poly_ReverseCoeff                                    0x1235 // Specifies an array of coefficients for the polynomial that converts scaled values to pre-scaled values. Each element of the array corresponds to a term of the equation. For example, if index three of the array is 9, the fourth term of the equation is 9y^3.
#define DAQmx_Scale_Table_ScaledVals                                     0x1236 // Specifies an array of scaled values. These values map directly to the values in Pre-Scaled Values.
#define DAQmx_Scale_Table_PreScaledVals                                  0x1237 // Specifies an array of pre-scaled values. These values map directly to the values in Scaled Values.

//********** System Attributes **********
#define DAQmx_Sys_GlobalChans                                            0x1265 // Indicates an array that contains the names of all global channels saved on the system.
#define DAQmx_Sys_Scales                                                 0x1266 // Indicates an array that contains the names of all custom scales saved on the system.
#define DAQmx_Sys_Tasks                                                  0x1267 // Indicates an array that contains the names of all tasks saved on the system.
#define DAQmx_Sys_DevNames                                               0x193B // Indicates an array that contains the names of all devices installed in the system.
#define DAQmx_Sys_NIDAQMajorVersion                                      0x1272 // Indicates the major portion of the installed version of NI-DAQ, such as 7 for version 7.0.
#define DAQmx_Sys_NIDAQMinorVersion                                      0x1923 // Indicates the minor portion of the installed version of NI-DAQ, such as 0 for version 7.0.

//********** Task Attributes **********
#define DAQmx_Task_Name                                                  0x1276 // Indicates the name of the task.
#define DAQmx_Task_Channels                                              0x1273 // Indicates the names of all virtual channels in the task.
#define DAQmx_Task_NumChans                                              0x2181 // Indicates the number of virtual channels in the task.
#define DAQmx_Task_Complete                                              0x1274 // Indicates whether the task completed execution.

//********** Timing Attributes **********
#define DAQmx_SampQuant_SampMode                                         0x1300 // Specifies if a task acquires or generates a finite number of samples or if it continuously acquires or generates samples.
#define DAQmx_SampQuant_SampPerChan                                      0x1310 // Specifies the number of samples to acquire or generate for each channel if Sample Mode is finite.
#define DAQmx_SampTimingType                                             0x1347 // Specifies the type of sample timing to use for the task.
#define DAQmx_SampClk_Rate                                               0x1344 // Specifies the sampling rate in samples per channel per second. If you use an external source for the Sample Clock, set this input to the maximum expected rate of that clock.
#define DAQmx_SampClk_Src                                                0x1852 // Specifies the terminal of the signal to use as the Sample Clock.
#define DAQmx_SampClk_ActiveEdge                                         0x1301 // Specifies on which edge of a clock pulse sampling takes place. This property is useful primarily when the signal you use as the Sample Clock is not a periodic clock.
#define DAQmx_SampClk_TimebaseDiv                                        0x18EB // Specifies the number of Sample Clock Timebase pulses needed to produce a single Sample Clock pulse.
#define DAQmx_SampClk_Timebase_Rate                                      0x1303 // Specifies the rate of the Sample Clock Timebase. When the signal you use as the Sample Clock Timebase is not a clock, NI-DAQmx might require the rate to calculate other timing parameters. If this is the case, setting this property to an approximation is preferable to not setting it at all.
#define DAQmx_SampClk_Timebase_Src                                       0x1308 // Specifies the terminal of the signal to use as the Sample Clock Timebase.
#define DAQmx_SampClk_Timebase_ActiveEdge                                0x18EC // Specifies on which edge to recognize a Sample Clock Timebase pulse. This property is useful primarily when the signal you use as the Sample Clock Timebase is not a periodic clock.
#define DAQmx_SampClk_Timebase_MasterTimebaseDiv                         0x1305 // Specifies the number of pulses of the Master Timebase needed to produce a single pulse of the Sample Clock Timebase.
#define DAQmx_ChangeDetect_DI_RisingEdgePhysicalChans                    0x2195 // Specifies the names of the digital lines or ports on which to detect rising edges. The lines or ports must be used by virtual channels in the task. You also can specify a string that contains a list or range of digital lines or ports.
#define DAQmx_ChangeDetect_DI_FallingEdgePhysicalChans                   0x2196 // Specifies the names of the digital lines or ports on which to detect rising edges. The lines or ports must be used by virtual channels in the task. You also can specify a string that contains a list or range of digital lines or ports.
#define DAQmx_OnDemand_SimultaneousAOEnable                              0x21A0 // Specifies whether to update all channels in the task simultaneously, rather than updating channels independently when you write a sample to that channel.
#define DAQmx_AIConv_Rate                                                0x1848 // Specifies the rate at which to clock the analog-to-digital converter. This clock is specific to the analog input section of an E Series device.
#define DAQmx_AIConv_Src                                                 0x1502 // Specifies the terminal of the signal to use as the AI Convert Clock.
#define DAQmx_AIConv_ActiveEdge                                          0x1853 // Specifies on which edge of the clock pulse an analog-to-digital conversion takes place.
#define DAQmx_AIConv_TimebaseDiv                                         0x1335 // Specifies the number of AI Convert Clock Timebase pulses needed to produce a single AI Convert Clock pulse.
#define DAQmx_AIConv_Timebase_Src                                        0x1339 // Specifies the terminal  of the signal to use as the AI Convert Clock Timebase.
#define DAQmx_MasterTimebase_Rate                                        0x1495 // Specifies the rate of the Master Timebase.
#define DAQmx_MasterTimebase_Src                                         0x1343 // Specifies the terminal of the signal to use as the Master Timebase. On an E Series device, you can choose only between the onboard 20MHz Timebase or the RTSI7 terminal.
#define DAQmx_DelayFromSampClk_DelayUnits                                0x1304 // Specifies the units of Delay.
#define DAQmx_DelayFromSampClk_Delay                                     0x1317 // Specifies the amount of time to wait after receiving a Sample Clock edge before beginning to acquire the sample. This value is in the units you specify with Delay Units.

//********** Trigger Attributes **********
#define DAQmx_StartTrig_Type                                             0x1393 // Specifies the type of trigger to use to start a task.
#define DAQmx_DigEdge_StartTrig_Src                                      0x1407 // Specifies the name of a terminal where there is a digital signal to use as the source of the Start Trigger.
#define DAQmx_DigEdge_StartTrig_Edge                                     0x1404 // Specifies on which edge of a digital pulse to start acquiring or generating samples.
#define DAQmx_AnlgEdge_StartTrig_Src                                     0x1398 // Specifies the name of a virtual channel or terminal where there is an analog signal to use as the source of the Start Trigger.
#define DAQmx_AnlgEdge_StartTrig_Slope                                   0x1397 // Specifies on which slope of the trigger signal to start acquiring or generating samples.
#define DAQmx_AnlgEdge_StartTrig_Lvl                                     0x1396 // Specifies at what threshold in the units of the measurement or generation to start acquiring or generating samples. Use Slope to specify on which slope to trigger on this threshold.
#define DAQmx_AnlgEdge_StartTrig_Hyst                                    0x1395 // Specifies a hysteresis level in the units of the measurement or generation. If Slope is DAQmx_Val_RisingSlope, the trigger does not deassert until the source signal passes below  Level minus the hysteresis. If Slope is DAQmx_Val_FallingSlope, the trigger does not deassert until the source signal passes above Level plus the hysteresis.
#define DAQmx_AnlgWin_StartTrig_Src                                      0x1400 // Specifies the name of a virtual channel or terminal where there is an analog signal to use as the source of the Start Trigger.
#define DAQmx_AnlgWin_StartTrig_When                                     0x1401 // Specifies whether the task starts acquiring or generating samples when the signal enters or leaves the window you specify with Bottom and Top.
#define DAQmx_AnlgWin_StartTrig_Top                                      0x1403 // Specifies the upper limit of the window. Specify this value in the units of the measurement or generation.
#define DAQmx_AnlgWin_StartTrig_Btm                                      0x1402 // Specifies the lower limit of the window. Specify this value in the units of the measurement or generation.
#define DAQmx_StartTrig_Delay                                            0x1856 // Specifies an amount of time to wait after the Start Trigger is received before acquiring or generating the first sample. This value is in the units you specify with Delay Units.
#define DAQmx_StartTrig_DelayUnits                                       0x18C8 // Specifies the units of Delay.
#define DAQmx_StartTrig_Retriggerable                                    0x190F // Specifies whether to enable retriggerable counter pulse generation. When you set this property to TRUE, the device generates pulses each time it receives a trigger. The device ignores a trigger if it is in the process of generating pulses.
#define DAQmx_RefTrig_Type                                               0x1419 // Specifies the type of trigger to use to mark a reference point for the measurement.
#define DAQmx_RefTrig_PretrigSamples                                     0x1445 // Specifies the minimum number of pretrigger samples to acquire from each channel before recognizing the reference trigger. Post-trigger samples per channel are equal to Samples Per Channel minus the number of pretrigger samples per channel.
#define DAQmx_DigEdge_RefTrig_Src                                        0x1434 // Specifies the name of a terminal where there is a digital signal to use as the source of the Reference Trigger.
#define DAQmx_DigEdge_RefTrig_Edge                                       0x1430 // Specifies on what edge of a digital pulse the Reference Trigger occurs.
#define DAQmx_AnlgEdge_RefTrig_Src                                       0x1424 // Specifies the name of a virtual channel or terminal where there is an analog signal to use as the source of the Reference Trigger.
#define DAQmx_AnlgEdge_RefTrig_Slope                                     0x1423 // Specifies on which slope of the source signal the Reference Trigger occurs.
#define DAQmx_AnlgEdge_RefTrig_Lvl                                       0x1422 // Specifies in the units of the measurement the threshold at which the Reference Trigger occurs.  Use Slope to specify on which slope to trigger at this threshold.
#define DAQmx_AnlgEdge_RefTrig_Hyst                                      0x1421 // Specifies a hysteresis level in the units of the measurement. If Slope is DAQmx_Val_RisingSlope, the trigger does not deassert until the source signal passes below Level minus the hysteresis. If Slope is DAQmx_Val_FallingSlope, the trigger does not deassert until the source signal passes above Level plus the hysteresis.
#define DAQmx_AnlgWin_RefTrig_Src                                        0x1426 // Specifies the name of a virtual channel or terminal where there is an analog signal to use as the source of the Reference Trigger.
#define DAQmx_AnlgWin_RefTrig_When                                       0x1427 // Specifies whether the Reference Trigger occurs when the source signal enters the window or when it leaves the window. Use Bottom and Top to specify the window.
#define DAQmx_AnlgWin_RefTrig_Top                                        0x1429 // Specifies the upper limit of the window. Specify this value in the units of the measurement.
#define DAQmx_AnlgWin_RefTrig_Btm                                        0x1428 // Specifies the lower limit of the window. Specify this value in the units of the measurement.
#define DAQmx_AdvTrig_Type                                               0x1365 // Specifies the type of trigger to use to advance to the next entry in a scan list.
#define DAQmx_DigEdge_AdvTrig_Src                                        0x1362 // Specifies the name of a terminal where there is a digital signal to use as the source of the Advance Trigger.
#define DAQmx_DigEdge_AdvTrig_Edge                                       0x1360 // Specifies on which edge of a digital signal to advance to the next entry in a scan list.
#define DAQmx_PauseTrig_Type                                             0x1366 // Specifies the type of trigger to use to pause a task.
#define DAQmx_AnlgLvl_PauseTrig_Src                                      0x1370 // Specifies the name of a virtual channel or terminal where there is an analog signal to use as the source of the trigger.
#define DAQmx_AnlgLvl_PauseTrig_When                                     0x1371 // Specifies whether the task pauses above or below the threshold you specify with Level.
#define DAQmx_AnlgLvl_PauseTrig_Lvl                                      0x1369 // Specifies the threshold at which to pause the task. Specify this value in the units of the measurement or generation. Use Pause When to specify whether the task pauses above or below this threshold.
#define DAQmx_AnlgLvl_PauseTrig_Hyst                                     0x1368 // Specifies a hysteresis level in the units of the measurement or generation. If Pause When is DAQmx_Val_AboveLvl, the trigger does not deassert until the source signal passes below Level minus the hysteresis. If Pause When is DAQmx_Val_BelowLvl, the trigger does not deassert until the source signal passes above Level plus the hysteresis.
#define DAQmx_AnlgWin_PauseTrig_Src                                      0x1373 // Specifies the name of a virtual channel or terminal where there is an analog signal to use as the source of the trigger.
#define DAQmx_AnlgWin_PauseTrig_When                                     0x1374 // Specifies whether the task pauses while the trigger signal is inside or outside the window you specify with Bottom and Top.
#define DAQmx_AnlgWin_PauseTrig_Top                                      0x1376 // Specifies the upper limit of the window. Specify this value in the units of the measurement or generation.
#define DAQmx_AnlgWin_PauseTrig_Btm                                      0x1375 // Specifies the lower limit of the window. Specify this value in the units of the measurement or generation.
#define DAQmx_DigLvl_PauseTrig_Src                                       0x1379 // Specifies the name of a terminal where there is a digital signal to use as the source of the Pause Trigger.
#define DAQmx_DigLvl_PauseTrig_When                                      0x1380 // Specifies whether the task pauses while the signal is high or low.
#define DAQmx_ArmStartTrig_Type                                          0x1414 // Specifies the type of trigger to use to arm the task for a Start Trigger. If you configure an Arm Start Trigger, the task does not respond to a Start Trigger until the device receives the Arm Start Trigger.
#define DAQmx_DigEdge_ArmStartTrig_Src                                   0x1417 // Specifies the name of a terminal where there is a digital signal to use as the source of the Arm Start Trigger.
#define DAQmx_DigEdge_ArmStartTrig_Edge                                  0x1415 // Specifies on which edge of a digital signal to arm the task for a Start Trigger.

//********** Watchdog Attributes **********
#define DAQmx_Watchdog_Timeout                                           0x21A9 // Specifies in seconds the amount of time until the watchdog timer expires. A value of -1 means the internal timer never expires. Set this input to -1 if you use an Expiration Trigger to expire the watchdog task.
#define DAQmx_WatchdogExpirTrig_Type                                     0x21A3 // Specifies the type of trigger to use to expire a watchdog task.
#define DAQmx_DigEdge_WatchdogExpirTrig_Src                              0x21A4 // Specifies the name of a terminal where a digital signal exists to use as the source of the Expiration Trigger.
#define DAQmx_DigEdge_WatchdogExpirTrig_Edge                             0x21A5 // Specifies on which edge of a digital signal to expire the watchdog task.
#define DAQmx_Watchdog_DO_ExpirState                                     0x21A7 // Specifies the state to which to set the digital physical channels when the watchdog task expires.  You cannot modify the expiration state of dedicated digital input physical channels.
#define DAQmx_Watchdog_HasExpired                                        0x21A8 // Indicates if the watchdog timer expired. You can read this property only while the task is running.

//********** Write Attributes **********
#define DAQmx_Write_RelativeTo                                           0x190C // Specifies the point in the buffer at which to write data. If you also specify an offset with Offset, the write operation begins at that offset relative to this point you select with this property.
#define DAQmx_Write_Offset                                               0x190D // Specifies in samples per channel an offset at which a write operation begins. This offset is relative to the location you specify with Relative To.
#define DAQmx_Write_RegenMode                                            0x1453 // Specifies whether to allow NI-DAQmx to generate the same data multiple times.
#define DAQmx_Write_CurrWritePos                                         0x1458 // Indicates the number of the next sample for the device to generate. This value is identical for all channels in the task.
#define DAQmx_Write_SpaceAvail                                           0x1460 // Indicates in samples per channel the amount of available space in the buffer.
#define DAQmx_Write_TotalSampPerChanGenerated                            0x192B // Indicates the total number of samples generated by each channel in the task. This value is identical for all channels in the task.
#define DAQmx_Write_RawDataWidth                                         0x217D // Indicates in bytes the required size of a raw sample to write to the task.
#define DAQmx_Write_NumChans                                             0x217E // Indicates the number of channels that an NI-DAQmx Write function writes to the task. This value is the number of channels in the task.
#define DAQmx_Write_DigitalLines_BytesPerChan                            0x217F // Indicates the number of bytes expected per channel in a sample for line-based writes. If a channel has fewer lines than this number, NI-DAQmx ignores the extra bytes.


/******************************************************************************
 *** NI-DAQmx Values **********************************************************
 ******************************************************************************/

/******************************************************/
/***    Non-Attribute Function Parameter Values     ***/
/******************************************************/

//*** Values for the Mode parameter of DAQmxTaskControl ***
#define DAQmx_Val_Task_Start                                              0   // Start
#define DAQmx_Val_Task_Stop                                               1   // Stop
#define DAQmx_Val_Task_Verify                                             2   // Verify
#define DAQmx_Val_Task_Commit                                             3   // Commit
#define DAQmx_Val_Task_Reserve                                            4   // Reserve
#define DAQmx_Val_Task_Unreserve                                          5   // Unreserve
#define DAQmx_Val_Task_Abort                                              6   // Abort

//*** Values for the Action parameter of DAQmxControlWatchdogTask ***
#define DAQmx_Val_ResetTimer                                              0   // Reset Timer
#define DAQmx_Val_ClearExpiration                                         1   // Clear Expiration

//*** Values for the Line Grouping parameter of DAQmxCreateDIChan and DAQmxCreateDOChan ***
#define DAQmx_Val_ChanPerLine                                             0   // One Channel For Each Line
#define DAQmx_Val_ChanForAllLines                                         1   // One Channel For All Lines

//*** Values for the Fill Mode parameter of DAQmxReadAnalogF64, DAQmxReadBinaryI16, DAQmxReadBinaryU16, DAQmxReadDigitalU8, DAQmxReadDigitalU32, DAQmxReadDigitalLines ***
//*** Values for the Data Layout parameter of DAQmxWriteAnalogF64, DAQmxWriteBinaryI16, DAQmxWriteDigitalU8, DAQmxWriteDigitalU32, DAQmxWriteDigitalLines ***
#define DAQmx_Val_GroupByChannel                                          0   // Group by Channel
#define DAQmx_Val_GroupByScanNumber                                       1   // Group by Scan Number

//*** Values for the Signal Modifiers parameter of DAQmxConnectTerms ***/
#define DAQmx_Val_DoNotInvertPolarity                                     0   // Do not invert polarity
#define DAQmx_Val_InvertPolarity                                          1   // Invert polarity

//*** Values for the Action paramter of DAQmxCloseExtCal ***
#define DAQmx_Val_Action_Commit                                           0   // Commit
#define DAQmx_Val_Action_Cancel                                           1   // Cancel

//*** Values for the Trigger ID parameter of DAQmxSendSoftwareTrigger ***
#define DAQmx_Val_AdvanceTrigger                                          12488 // Advance Trigger

//*** Value set for the Signal ID parameter of DAQmxExportSignal ***
#define DAQmx_Val_AIConvertClock                                          12484 // AI Convert Clock
#define DAQmx_Val_20MHzTimebaseClock                                      12486 // 20MHz Timebase Clock
#define DAQmx_Val_SampleClock                                             12487 // Sample Clock
#define DAQmx_Val_AdvanceTrigger                                          12488 // Advance Trigger
#define DAQmx_Val_ReferenceTrigger                                        12490 // Reference Trigger
#define DAQmx_Val_StartTrigger                                            12491 // Start Trigger
#define DAQmx_Val_AdvCmpltEvent                                           12492 // Advance Complete Event
#define DAQmx_Val_AIHoldCmpltEvent                                        12493 // AI Hold Complete Event
#define DAQmx_Val_CounterOutputEvent                                      12494 // Counter Output Event
#define DAQmx_Val_ChangeDetectionEvent                                    12511 // Change Detection Event
#define DAQmx_Val_WDTExpiredEvent                                         12512 // Watchdog Timer Expired Event

//*** Value set for the ActiveEdge parameter of DAQmxCfgSampClkTiming ***
#define DAQmx_Val_Rising                                                  10280 // Rising
#define DAQmx_Val_Falling                                                 10171 // Falling

//*** Value set SwitchPathType ***
//*** Value set for the output Path Status parameter of DAQmxSwitchFindPath ***
#define DAQmx_Val_PathStatus_Available                                    10431 // Path Available
#define DAQmx_Val_PathStatus_AlreadyExists                                10432 // Path Already Exists
#define DAQmx_Val_PathStatus_Unsupported                                  10433 // Path Unsupported
#define DAQmx_Val_PathStatus_ChannelInUse                                 10434 // Channel In Use
#define DAQmx_Val_PathStatus_SourceChannelConflict                        10435 // Channel Source Conflict
#define DAQmx_Val_PathStatus_ChannelReservedForRouting                    10436 // Channel Reserved for Routing

//*** Value set for the Units parameter of DAQmxCreateAIThrmcplChan, DAQmxCreateAIRTDChan, DAQmxCreateAIThrmstrChanIex, DAQmxCreateAIThrmstrChanVex and DAQmxCreateAITempBuiltInSensorChan ***
#define DAQmx_Val_DegC                                                    10143 // Deg C
#define DAQmx_Val_DegF                                                    10144 // Deg F
#define DAQmx_Val_Kelvins                                                 10325 // Kelvins
#define DAQmx_Val_DegR                                                    10145 // Deg R

//*** Value set for the state parameter of DAQmxSetDigitalPowerUpStates ***
#define DAQmx_Val_High                                                    10192 // High
#define DAQmx_Val_Low                                                     10214 // Low
#define DAQmx_Val_Tristate                                                10310 // Tristate

//*** Value set RelayPos ***
//*** Value set for the state parameter of DAQmxSwitchGetSingleRelayPos and DAQmxSwitchGetMultiRelayPos ***
#define DAQmx_Val_Open                                                    10437 // Open
#define DAQmx_Val_Closed                                                  10438 // Closed

//*** Value for the Terminal Config parameter of DAQmxCreateAIVoltageChan, DAQmxCreateAICurrentChan and DAQmxCreateAIVoltageChanWithExcit ***
#define DAQmx_Val_Cfg_Default                                             -1 // Default

//*** Value for the Timeout parameter of DAQmxWaitUntilTaskDone
#define DAQmx_Val_WaitInfinitely                                          -1.0

//*** Value for the Number of Samples per Channel parameter of DAQmxReadAnalogF64, DAQmxReadBinaryI16, DAQmxReadBinaryU16, DAQmxReadDigitalU8, DAQmxReadDigitalU32,
//    DAQmxReadDigitalLines, DAQmxReadCounterF64, DAQmxReadCounterU32 and DAQmxReadRaw ***
#define DAQmx_Val_Auto                                                    -1

/******************************************************/
/***              Attribute Values                  ***/
/******************************************************/

//*** Values for DAQmx_AI_ACExcit_WireMode ***
//*** Value set ACExcitWireMode ***
#define DAQmx_Val_4Wire                                                       4 // 4-Wire
#define DAQmx_Val_5Wire                                                       5 // 5-Wire

//*** Values for DAQmx_AI_MeasType ***
//*** Value set AIMeasurementType ***
#define DAQmx_Val_Voltage                                                 10322 // Voltage
#define DAQmx_Val_Current                                                 10134 // Current
#define DAQmx_Val_Voltage_CustomWithExcitation                            10323 // More:Voltage:Custom with Excitation
#define DAQmx_Val_Freq_Voltage                                            10181 // Frequency
#define DAQmx_Val_Resistance                                              10278 // Resistance
#define DAQmx_Val_Temp_TC                                                 10303 // Temperature:Thermocouple
#define DAQmx_Val_Temp_Thrmstr                                            10302 // Temperature:Thermistor
#define DAQmx_Val_Temp_RTD                                                10301 // Temperature:RTD
#define DAQmx_Val_Temp_BuiltInSensor                                      10311 // Temperature:Built-in Sensor
#define DAQmx_Val_Strain_Gage                                             10300 // Strain Gage
#define DAQmx_Val_Position_LVDT                                           10352 // Position:LVDT
#define DAQmx_Val_Position_RVDT                                           10353 // Position:RVDT
#define DAQmx_Val_Accelerometer                                           10356 // Accelerometer

//*** Values for DAQmx_AO_OutputType ***
//*** Value set AOOutputChannelType ***
#define DAQmx_Val_Voltage                                                 10322 // Voltage
#define DAQmx_Val_Current                                                 10134 // Current

//*** Values for DAQmx_AI_Accel_SensitivityUnits ***
//*** Value set AccelSensitivityUnits1 ***
#define DAQmx_Val_mVoltsPerG                                              12509 // mVolts/g
#define DAQmx_Val_VoltsPerG                                               12510 // Volts/g

//*** Values for DAQmx_AI_Accel_Units ***
//*** Value set AccelUnits2 ***
#define DAQmx_Val_AccelUnit_g                                             10186 // g
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale

//*** Values for DAQmx_SampQuant_SampMode ***
//*** Value set AcquisitionType ***
#define DAQmx_Val_FiniteSamps                                             10178 // Finite Samples
#define DAQmx_Val_ContSamps                                               10123 // Continuous Samples

//*** Values for DAQmx_AnlgLvl_PauseTrig_When ***
//*** Value set ActiveLevel ***
#define DAQmx_Val_AboveLvl                                                10093 // Above Level
#define DAQmx_Val_BelowLvl                                                10107 // Below Level

//*** Values for DAQmx_AI_RVDT_Units ***
//*** Value set AngleUnits1 ***
#define DAQmx_Val_Degrees                                                 10146 // Degrees
#define DAQmx_Val_Radians                                                 10273 // Radians
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale

//*** Values for DAQmx_CI_AngEncoder_Units ***
//*** Value set AngleUnits2 ***
#define DAQmx_Val_Degrees                                                 10146 // Degrees
#define DAQmx_Val_Radians                                                 10273 // Radians
#define DAQmx_Val_Ticks                                                   10304 // Ticks
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale

//*** Values for DAQmx_AI_AutoZeroMode ***
//*** Value set AutoZeroType1 ***
#define DAQmx_Val_None                                                    10230 // None
#define DAQmx_Val_Once                                                    10244 // Once

//*** Values for DAQmx_SwitchScan_BreakMode ***
//*** Value set BreakMode ***
#define DAQmx_Val_NoAction                                                10227 // No Action
#define DAQmx_Val_BreakBeforeMake                                         10110 // Break Before Make

//*** Values for DAQmx_AI_Bridge_Cfg ***
//*** Value set BridgeConfiguration1 ***
#define DAQmx_Val_FullBridge                                              10182 // Full Bridge
#define DAQmx_Val_HalfBridge                                              10187 // Half Bridge
#define DAQmx_Val_QuarterBridge                                           10270 // Quarter Bridge
#define DAQmx_Val_NoBridge                                                10228 // No Bridge

//*** Values for DAQmx_CI_MeasType ***
//*** Value set CIMeasurementType ***
#define DAQmx_Val_CountEdges                                              10125 // Count Edges
#define DAQmx_Val_Freq                                                    10179 // Frequency
#define DAQmx_Val_Period                                                  10256 // Period
#define DAQmx_Val_PulseWidth                                              10359 // Pulse Width
#define DAQmx_Val_SemiPeriod                                              10289 // Semi Period
#define DAQmx_Val_Position_AngEncoder                                     10360 // Position:Angular Encoder
#define DAQmx_Val_Position_LinEncoder                                     10361 // Position:Linear Encoder
#define DAQmx_Val_TwoEdgeSep                                              10267 // Two Edge Separation

//*** Values for DAQmx_AI_Thrmcpl_CJCSrc ***
//*** Value set CJCSource1 ***
#define DAQmx_Val_BuiltIn                                                 10200 // Built-In
#define DAQmx_Val_ConstVal                                                10116 // Constant Value
#define DAQmx_Val_Chan                                                    10113 // Channel

//*** Values for DAQmx_CO_OutputType ***
//*** Value set COOutputType ***
#define DAQmx_Val_Pulse_Time                                              10269 // Pulse:Time
#define DAQmx_Val_Pulse_Freq                                              10119 // Pulse:Frequency
#define DAQmx_Val_Pulse_Ticks                                             10268 // Pulse:Ticks

//*** Values for DAQmx_ChanType ***
//*** Value set ChannelType ***
#define DAQmx_Val_AI                                                      10100 // Analog Input
#define DAQmx_Val_AO                                                      10102 // Analog Output
#define DAQmx_Val_DI                                                      10151 // Digital Input
#define DAQmx_Val_DO                                                      10153 // Digital Output
#define DAQmx_Val_CI                                                      10131 // Counter Input
#define DAQmx_Val_CO                                                      10132 // Counter Output

//*** Values for DAQmx_CI_CountEdges_Dir ***
//*** Value set CountDirection1 ***
#define DAQmx_Val_CountUp                                                 10128 // Count Up
#define DAQmx_Val_CountDown                                               10124 // Count Down
#define DAQmx_Val_ExtControlled                                           10326 // Externally Controlled

//*** Values for DAQmx_CI_Freq_MeasMeth ***
//*** Values for DAQmx_CI_Period_MeasMeth ***
//*** Value set CounterFrequencyMethod ***
#define DAQmx_Val_LowFreq1Ctr                                             10105 // Low Frequency with 1 Counter
#define DAQmx_Val_HighFreq2Ctr                                            10157 // High Frequency with 2 Counters
#define DAQmx_Val_LargeRng2Ctr                                            10205 // Large Range with 2 Counters

//*** Values for DAQmx_AI_Coupling ***
//*** Value set Coupling1 ***
#define DAQmx_Val_AC                                                      10045 // AC
#define DAQmx_Val_DC                                                      10050 // DC
#define DAQmx_Val_GND                                                     10066 // GND

//*** Values for DAQmx_AI_CurrentShunt_Loc ***
//*** Value set CurrentShuntResistorLocation1 ***
#define DAQmx_Val_Internal                                                10200 // Internal
#define DAQmx_Val_External                                                10167 // External

//*** Values for DAQmx_AI_Current_Units ***
//*** Values for DAQmx_AO_Current_Units ***
//*** Value set CurrentUnits1 ***
#define DAQmx_Val_Amps                                                    10342 // Amps
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale

//*** Values for DAQmx_AI_DataXferMech ***
//*** Values for DAQmx_AO_DataXferMech ***
//*** Values for DAQmx_CI_DataXferMech ***
//*** Value set DataTransferMechanism ***
#define DAQmx_Val_DMA                                                     10054 // DMA
#define DAQmx_Val_Interrupts                                              10204 // Interrupts
#define DAQmx_Val_ProgrammedIO                                            10264 // Programmed I/O

//*** Values for DAQmx_Watchdog_DO_ExpirState ***
//*** Value set DigitalLineState ***
#define DAQmx_Val_High                                                    10192 // High
#define DAQmx_Val_Low                                                     10214 // Low
#define DAQmx_Val_Tristate                                                10310 // Tristate
#define DAQmx_Val_NoChange                                                10160 // No Change

//*** Values for DAQmx_StartTrig_DelayUnits ***
//*** Value set DigitalWidthUnits1 ***
#define DAQmx_Val_SampClkPeriods                                          10286 // Sample Clock Periods
#define DAQmx_Val_Seconds                                                 10364 // Seconds
#define DAQmx_Val_Ticks                                                   10304 // Ticks

//*** Values for DAQmx_DelayFromSampClk_DelayUnits ***
//*** Value set DigitalWidthUnits2 ***
#define DAQmx_Val_Seconds                                                 10364 // Seconds
#define DAQmx_Val_Ticks                                                   10304 // Ticks

//*** Values for DAQmx_Exported_AdvTrig_Pulse_WidthUnits ***
//*** Value set DigitalWidthUnits3 ***
#define DAQmx_Val_Seconds                                                 10364 // Seconds

//*** Values for DAQmx_CI_Freq_StartingEdge ***
//*** Values for DAQmx_CI_Period_StartingEdge ***
//*** Values for DAQmx_CI_CountEdges_ActiveEdge ***
//*** Values for DAQmx_CI_PulseWidth_StartingEdge ***
//*** Values for DAQmx_CI_TwoEdgeSep_FirstEdge ***
//*** Values for DAQmx_CI_TwoEdgeSep_SecondEdge ***
//*** Values for DAQmx_CI_CtrTimebaseActiveEdge ***
//*** Values for DAQmx_CO_CtrTimebaseActiveEdge ***
//*** Values for DAQmx_SampClk_ActiveEdge ***
//*** Values for DAQmx_SampClk_Timebase_ActiveEdge ***
//*** Values for DAQmx_AIConv_ActiveEdge ***
//*** Values for DAQmx_DigEdge_StartTrig_Edge ***
//*** Values for DAQmx_DigEdge_RefTrig_Edge ***
//*** Values for DAQmx_DigEdge_AdvTrig_Edge ***
//*** Values for DAQmx_DigEdge_ArmStartTrig_Edge ***
//*** Values for DAQmx_DigEdge_WatchdogExpirTrig_Edge ***
//*** Value set Edge1 ***
#define DAQmx_Val_Rising                                                  10280 // Rising
#define DAQmx_Val_Falling                                                 10171 // Falling

//*** Values for DAQmx_CI_Encoder_DecodingType ***
//*** Value set EncoderType2 ***
#define DAQmx_Val_X1                                                      10090 // X1
#define DAQmx_Val_X2                                                      10091 // X2
#define DAQmx_Val_X4                                                      10092 // X4
#define DAQmx_Val_TwoPulseCounting                                        10313 // Two Pulse Counting

//*** Values for DAQmx_CI_Encoder_ZIndexPhase ***
//*** Value set EncoderZIndexPhase1 ***
#define DAQmx_Val_AHighBHigh                                              10040 // A High B High
#define DAQmx_Val_AHighBLow                                               10041 // A High B Low
#define DAQmx_Val_ALowBHigh                                               10042 // A Low B High
#define DAQmx_Val_ALowBLow                                                10043 // A Low B Low

//*** Values for DAQmx_AI_Excit_DCorAC ***
//*** Value set ExcitationDCorAC ***
#define DAQmx_Val_DC                                                      10050 // DC
#define DAQmx_Val_AC                                                      10045 // AC

//*** Values for DAQmx_AI_Excit_Src ***
//*** Value set ExcitationSource ***
#define DAQmx_Val_Internal                                                10200 // Internal
#define DAQmx_Val_External                                                10167 // External
#define DAQmx_Val_None                                                    10230 // None

//*** Values for DAQmx_AI_Excit_VoltageOrCurrent ***
//*** Value set ExcitationVoltageOrCurrent ***
#define DAQmx_Val_Voltage                                                 10322 // Voltage
#define DAQmx_Val_Current                                                 10134 // Current

//*** Values for DAQmx_Exported_CtrOutEvent_OutputBehavior ***
//*** Value set ExportActions2 ***
#define DAQmx_Val_Pulse                                                   10265 // Pulse
#define DAQmx_Val_Toggle                                                  10307 // Toggle

//*** Values for DAQmx_Exported_SampClk_OutputBehavior ***
//*** Value set ExportActions3 ***
#define DAQmx_Val_Pulse                                                   10265 // Pulse
#define DAQmx_Val_Lvl                                                     10210 // Level

//*** Values for DAQmx_AI_Freq_Units ***
//*** Value set FrequencyUnits ***
#define DAQmx_Val_Hz                                                      10373 // Hz
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale

//*** Values for DAQmx_CO_Pulse_Freq_Units ***
//*** Value set FrequencyUnits2 ***
#define DAQmx_Val_Hz                                                      10373 // Hz

//*** Values for DAQmx_CI_Freq_Units ***
//*** Value set FrequencyUnits3 ***
#define DAQmx_Val_Hz                                                      10373 // Hz
#define DAQmx_Val_Ticks                                                   10304 // Ticks
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale


//*** Values for DAQmx_AI_DataXferReqCond ***
//*** Value set InputDataTransferCondition ***
#define DAQmx_Val_OnBrdMemMoreThanHalfFull                                10237 // On Board Memory More than Half Full
#define DAQmx_Val_OnBrdMemNotEmpty                                        10241 // On Board Memory Not Empty

//*** Values for DAQmx_AI_TermCfg ***
//*** Value set InputTermCfg ***
#define DAQmx_Val_RSE                                                     10083 // RSE
#define DAQmx_Val_NRSE                                                    10078 // NRSE
#define DAQmx_Val_Diff                                                    10106 // Differential

//*** Values for DAQmx_AI_LVDT_SensitivityUnits ***
//*** Value set LVDTSensitivityUnits1 ***
#define DAQmx_Val_mVoltsPerVoltPerMillimeter                              12506 // mVolts/Volt/mMeter
#define DAQmx_Val_mVoltsPerVoltPerMilliInch                               12505 // mVolts/Volt/0.001 Inch

//*** Values for DAQmx_AI_LVDT_Units ***
//*** Value set LengthUnits2 ***
#define DAQmx_Val_Meters                                                  10219 // Meters
#define DAQmx_Val_Inches                                                  10379 // Inches
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale

//*** Values for DAQmx_CI_LinEncoder_Units ***
//*** Value set LengthUnits3 ***
#define DAQmx_Val_Meters                                                  10219 // Meters
#define DAQmx_Val_Inches                                                  10379 // Inches
#define DAQmx_Val_Ticks                                                   10304 // Ticks
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale

//*** Values for DAQmx_CI_OutputState ***
//*** Values for DAQmx_CO_Pulse_IdleState ***
//*** Values for DAQmx_CO_OutputState ***
//*** Values for DAQmx_Exported_CtrOutEvent_Toggle_IdleState ***
//*** Values for DAQmx_DigLvl_PauseTrig_When ***
//*** Value set Level1 ***
#define DAQmx_Val_High                                                    10192 // High
#define DAQmx_Val_Low                                                     10214 // Low

//*** Values for DAQmx_AIConv_Timebase_Src ***
//*** Value set MIOAIConvertTbSrc ***
#define DAQmx_Val_SameAsSampTimebase                                      10284 // Same as Sample Timebase
#define DAQmx_Val_SameAsMasterTimebase                                    10282 // Same as Master Timebase

//*** Values for DAQmx_AO_DataXferReqCond ***
//*** Value set OutputDataTransferCondition ***
#define DAQmx_Val_OnBrdMemEmpty                                           10235 // On Board Memory Empty
#define DAQmx_Val_OnBrdMemHalfFullOrLess                                  10239 // On Board Memory Half Full or Less
#define DAQmx_Val_OnBrdMemNotFull                                         10242 // On Board Memory Less than Full

//*** Values for DAQmx_Read_OverWrite ***
//*** Value set OverwriteMode1 ***
#define DAQmx_Val_OverwriteUnreadSamps                                    10252 // Overwrite Unread Samples
#define DAQmx_Val_DoNotOverwriteUnreadSamps                               10159 // Do Not Overwrite Unread Samples

//*** Values for DAQmx_Exported_AIConvClk_Pulse_Polarity ***
//*** Values for DAQmx_Exported_AdvTrig_Pulse_Polarity ***
//*** Values for DAQmx_Exported_AdvCmpltEvent_Pulse_Polarity ***
//*** Values for DAQmx_Exported_AIHoldCmpltEvent_PulsePolarity ***
//*** Values for DAQmx_Exported_CtrOutEvent_Pulse_Polarity ***
//*** Value set Polarity2 ***
#define DAQmx_Val_ActiveHigh                                              10095 // Active High
#define DAQmx_Val_ActiveLow                                               10096 // Active Low

//*** Values for DAQmx_AI_RTD_Type ***
//*** Value set RTDType1 ***
#define DAQmx_Val_Pt3750                                                  12481 // Pt3750
#define DAQmx_Val_Pt3851                                                  10071 // Pt3851
#define DAQmx_Val_Pt3911                                                  12482 // Pt3911
#define DAQmx_Val_Pt3916                                                  10069 // Pt3916
#define DAQmx_Val_Pt3920                                                  10053 // Pt3920
#define DAQmx_Val_Pt3928                                                  12483 // Pt3928
#define DAQmx_Val_Custom                                                  10137 // Custom

//*** Values for DAQmx_AI_RVDT_SensitivityUnits ***
//*** Value set RVDTSensitivityUnits1 ***
#define DAQmx_Val_mVoltsPerVoltPerDegree                                  12507 // mVolts/Volt/Degree
#define DAQmx_Val_mVoltsPerVoltPerRadian                                  12508 // mVolts/Volt/Radian

//*** Values for DAQmx_Read_RelativeTo ***
//*** Value set ReadRelativeTo ***
#define DAQmx_Val_FirstSample                                             10424 // First Sample
#define DAQmx_Val_CurrReadPos                                             10425 // Current Read Position
#define DAQmx_Val_RefTrig                                                 10426 // Reference Trigger
#define DAQmx_Val_FirstPretrigSamp                                        10427 // First Pretrigger Sample
#define DAQmx_Val_MostRecentSamp                                          10428 // Most Recent Sample


//*** Values for DAQmx_Write_RegenMode ***
//*** Value set RegenerationMode1 ***
#define DAQmx_Val_AllowRegen                                              10097 // Allow Regeneration
#define DAQmx_Val_DoNotAllowRegen                                         10158 // Do Not Allow Regeneration

//*** Values for DAQmx_AI_ResistanceCfg ***
//*** Value set ResistanceConfiguration ***
#define DAQmx_Val_2Wire                                                       2 // 2-Wire
#define DAQmx_Val_3Wire                                                       3 // 3-Wire
#define DAQmx_Val_4Wire                                                       4 // 4-Wire

//*** Values for DAQmx_AI_Resistance_Units ***
//*** Value set ResistanceUnits1 ***
#define DAQmx_Val_Ohms                                                    10384 // Ohms
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale

//*** Values for DAQmx_AI_ResolutionUnits ***
//*** Values for DAQmx_AO_ResolutionUnits ***
//*** Value set ResolutionType1 ***
#define DAQmx_Val_Bits                                                    10109 // Bits

//*** Values for DAQmx_SampTimingType ***
//*** Value set SampleTimingType ***
#define DAQmx_Val_SampClk                                                 10388 // Sample Clock
#define DAQmx_Val_Handshake                                               10389 // Handshake
#define DAQmx_Val_Implicit                                                10451 // Implicit
#define DAQmx_Val_OnDemand                                                10390 // On Demand
#define DAQmx_Val_ChangeDetection                                         12504 // Change Detection

//*** Values for DAQmx_Scale_Type ***
//*** Value set ScaleType ***
#define DAQmx_Val_Linear                                                  10447 // Linear
#define DAQmx_Val_MapRanges                                               10448 // Map Ranges
#define DAQmx_Val_Polynomial                                              10449 // Polynomial
#define DAQmx_Val_Table                                                   10450 // Table

//*** Values for DAQmx_AI_Bridge_ShuntCal_Select ***
//*** Value set ShuntCalSelect ***
#define DAQmx_Val_A                                                       12513 // A
#define DAQmx_Val_B                                                       12514 // B
#define DAQmx_Val_AandB                                                   12515 // A and B

//*** Values for DAQmx_AnlgEdge_StartTrig_Slope ***
//*** Values for DAQmx_AnlgEdge_RefTrig_Slope ***
//*** Value set Slope1 ***
#define DAQmx_Val_RisingSlope                                             10280 // Rising
#define DAQmx_Val_FallingSlope                                            10171 // Falling

//*** Values for DAQmx_AI_Lowpass_SwitchCap_ClkSrc ***
//*** Values for DAQmx_AO_DAC_Ref_Src ***
//*** Value set SourceSelection ***
#define DAQmx_Val_Internal                                                10200 // Internal
#define DAQmx_Val_External                                                10167 // External

//*** Values for DAQmx_AI_StrainGage_Cfg ***
//*** Value set StrainGageBridgeType1 ***
#define DAQmx_Val_FullBridgeI                                             10183 // Full Bridge I
#define DAQmx_Val_FullBridgeII                                            10184 // Full Bridge II
#define DAQmx_Val_FullBridgeIII                                           10185 // Full Bridge III
#define DAQmx_Val_HalfBridgeI                                             10188 // Half Bridge I
#define DAQmx_Val_HalfBridgeII                                            10189 // Half Bridge II
#define DAQmx_Val_QuarterBridgeI                                          10271 // Quarter Bridge I
#define DAQmx_Val_QuarterBridgeII                                         10272 // Quarter Bridge II

//*** Values for DAQmx_AI_Strain_Units ***
//*** Value set StrainUnits1 ***
#define DAQmx_Val_Strain                                                  10299 // Strain
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale

//*** Values for DAQmx_SwitchScan_RepeatMode ***
//*** Value set SwitchScanRepeatMode ***
#define DAQmx_Val_Finite                                                  10172 // Finite
#define DAQmx_Val_Cont                                                    10117 // Continuous

//*** Values for DAQmx_SwitchChan_Usage ***
//*** Value set SwitchUsageTypes ***
#define DAQmx_Val_Source                                                  10439 // Source
#define DAQmx_Val_Load                                                    10440 // Load
#define DAQmx_Val_ReservedForRouting                                      10441 // Reserved for Routing

//*** Values for DAQmx_AI_Temp_Units ***
//*** Value set TemperatureUnits1 ***
#define DAQmx_Val_DegC                                                    10143 // Deg C
#define DAQmx_Val_DegF                                                    10144 // Deg F
#define DAQmx_Val_Kelvins                                                 10325 // Kelvins
#define DAQmx_Val_DegR                                                    10145 // Deg R
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale

//*** Values for DAQmx_AI_Thrmcpl_Type ***
//*** Value set ThermocoupleType1 ***
#define DAQmx_Val_J_Type_TC                                               10072 // J
#define DAQmx_Val_K_Type_TC                                               10073 // K
#define DAQmx_Val_N_Type_TC                                               10077 // N
#define DAQmx_Val_R_Type_TC                                               10082 // R
#define DAQmx_Val_S_Type_TC                                               10085 // S
#define DAQmx_Val_T_Type_TC                                               10086 // T
#define DAQmx_Val_B_Type_TC                                               10047 // B
#define DAQmx_Val_E_Type_TC                                               10055 // E

//*** Values for DAQmx_CO_Pulse_Time_Units ***
//*** Value set TimeUnits2 ***
#define DAQmx_Val_Seconds                                                 10364 // Seconds

//*** Values for DAQmx_CI_Period_Units ***
//*** Values for DAQmx_CI_PulseWidth_Units ***
//*** Values for DAQmx_CI_TwoEdgeSep_Units ***
//*** Values for DAQmx_CI_SemiPeriod_Units ***
//*** Value set TimeUnits3 ***
#define DAQmx_Val_Seconds                                                 10364 // Seconds
#define DAQmx_Val_Ticks                                                   10304 // Ticks
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale

//*** Values for DAQmx_RefTrig_Type ***
//*** Value set TriggerType1 ***
#define DAQmx_Val_AnlgEdge                                                10099 // Analog Edge
#define DAQmx_Val_DigEdge                                                 10150 // Digital Edge
#define DAQmx_Val_AnlgWin                                                 10103 // Analog Window
#define DAQmx_Val_None                                                    10230 // None

//*** Values for DAQmx_ArmStartTrig_Type ***
//*** Values for DAQmx_WatchdogExpirTrig_Type ***
//*** Value set TriggerType4 ***
#define DAQmx_Val_DigEdge                                                 10150 // Digital Edge
#define DAQmx_Val_None                                                    10230 // None

//*** Values for DAQmx_AdvTrig_Type ***
//*** Value set TriggerType5 ***
#define DAQmx_Val_DigEdge                                                 10150 // Digital Edge
#define DAQmx_Val_Software                                                10292 // Software
#define DAQmx_Val_None                                                    10230 // None

//*** Values for DAQmx_PauseTrig_Type ***
//*** Value set TriggerType6 ***
#define DAQmx_Val_AnlgLvl                                                 10101 // Analog Level
#define DAQmx_Val_AnlgWin                                                 10103 // Analog Window
#define DAQmx_Val_DigLvl                                                  10152 // Digital Level
#define DAQmx_Val_None                                                    10230 // None

//*** Values for DAQmx_StartTrig_Type ***
//*** Value set TriggerType8 ***
#define DAQmx_Val_AnlgEdge                                                10099 // Analog Edge
#define DAQmx_Val_DigEdge                                                 10150 // Digital Edge
#define DAQmx_Val_AnlgWin                                                 10103 // Analog Window
#define DAQmx_Val_None                                                    10230 // None

//*** Values for DAQmx_Scale_PreScaledUnits ***
//*** Value set UnitsPreScaled ***
#define DAQmx_Val_Volts                                                   10348 // Volts
#define DAQmx_Val_Amps                                                    10342 // Amps
#define DAQmx_Val_DegF                                                    10144 // Deg F
#define DAQmx_Val_DegC                                                    10143 // Deg C
#define DAQmx_Val_DegR                                                    10145 // Deg R
#define DAQmx_Val_Kelvins                                                 10325 // Kelvins
#define DAQmx_Val_Strain                                                  10299 // Strain
#define DAQmx_Val_Ohms                                                    10384 // Ohms
#define DAQmx_Val_Hz                                                      10373 // Hz
#define DAQmx_Val_Seconds                                                 10364 // Seconds
#define DAQmx_Val_Meters                                                  10219 // Meters
#define DAQmx_Val_Inches                                                  10379 // Inches
#define DAQmx_Val_Degrees                                                 10146 // Degrees
#define DAQmx_Val_Radians                                                 10273 // Radians
#define DAQmx_Val_g                                                       10186 // g

//*** Values for DAQmx_AI_Voltage_Units ***
//*** Value set VoltageUnits1 ***
#define DAQmx_Val_Volts                                                   10348 // Volts
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale

//*** Values for DAQmx_AO_Voltage_Units ***
//*** Value set VoltageUnits2 ***
#define DAQmx_Val_Volts                                                   10348 // Volts
#define DAQmx_Val_FromCustomScale                                         10065 // From Custom Scale

//*** Values for DAQmx_AnlgWin_StartTrig_When ***
//*** Values for DAQmx_AnlgWin_RefTrig_When ***
//*** Value set WindowTriggerCondition1 ***
#define DAQmx_Val_EnteringWin                                             10163 // Entering Window
#define DAQmx_Val_LeavingWin                                              10208 // Leaving Window

//*** Values for DAQmx_AnlgWin_PauseTrig_When ***
//*** Value set WindowTriggerCondition2 ***
#define DAQmx_Val_InsideWin                                               10199 // Inside Window
#define DAQmx_Val_OutsideWin                                              10251 // Outside Window

//*** Values for DAQmx_Write_RelativeTo ***
//*** Value set WriteRelativeTo ***
#define DAQmx_Val_FirstSample                                             10424 // First Sample
#define DAQmx_Val_CurrWritePos                                            10430 // Current Write Position


/******************************************************************************
 *** NI-DAQBasemx Function Declarations ***************************************
 ******************************************************************************/

/******************************************************/
/***         Task Configuration/Control             ***/
/******************************************************/


int32 DllExport __CFUNC     DAQmxBaseLoadTask            (const char taskName[], TaskHandle *taskHandle);
int32 DllExport __CFUNC     DAQmxBaseCreateTask          (const char taskName[], TaskHandle *taskHandle);
// Channel Names must be valid channels already available in MAX. They are not created.
//int32 DllExport __CFUNC     DAQmxBaseAddGlobalChansToTask(TaskHandle taskHandle, const char channelNames[]);

int32 DllExport __CFUNC     DAQmxBaseStartTask           (TaskHandle taskHandle);
int32 DllExport __CFUNC     DAQmxBaseStopTask            (TaskHandle taskHandle);

int32 DllExport __CFUNC     DAQmxBaseClearTask           (TaskHandle taskHandle);

//int32 DllExport __CFUNC     DAQmxBaseWaitUntilTaskDone   (TaskHandle taskHandle, float64 timeToWait);
int32 DllExport __CFUNC     DAQmxBaseIsTaskDone          (TaskHandle taskHandle, bool32 *isTaskDone);

//int32 DllExport __CFUNC     DAQmxBaseTaskControl         (TaskHandle taskHandle, int32 action);

//int32 DllExport __CFUNC     DAQmxBaseGetNthTaskChannel   (TaskHandle taskHandle, uInt32 index, char buffer[], int32 bufferSize);

//int32 DllExport __CFUNC_C   DAQmxBaseGetTaskAttribute    (TaskHandle taskHandle, int32 attribute, void *value, ...);


/******************************************************/
/***        Channel Configuration/Creation          ***/
/******************************************************/


int32 DllExport __CFUNC     DAQmxBaseCreateAIVoltageChan          (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], int32 terminalConfig, float64 minVal, float64 maxVal, int32 units, const char customScaleName[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateAICurrentChan          (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], int32 terminalConfig, float64 minVal, float64 maxVal, int32 units, int32 shuntResistorLoc, float64 extShuntResistorVal, const char customScaleName[]);
int32 DllExport __CFUNC     DAQmxBaseCreateAIThrmcplChan          (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, int32 thermocoupleType, int32 cjcSource, float64 cjcVal, const char cjcChannel[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateAIRTDChan              (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, int32 rtdType, int32 resistanceConfig, int32 currentExcitSource, float64 currentExcitVal, float64 r0);
//int32 DllExport __CFUNC     DAQmxBaseCreateAIThrmstrChanIex       (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, int32 resistanceConfig, int32 currentExcitSource, float64 currentExcitVal, float64 a, float64 b, float64 c);
//int32 DllExport __CFUNC     DAQmxBaseCreateAIThrmstrChanVex       (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, int32 resistanceConfig, int32 voltageExcitSource, float64 voltageExcitVal, float64 a, float64 b, float64 c, float64 r1);
//int32 DllExport __CFUNC     DAQmxBaseCreateAIFreqVoltageChan      (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, float64 thresholdLevel, float64 hysteresis, const char customScaleName[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateAIResistanceChan       (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, int32 resistanceConfig, int32 currentExcitSource, float64 currentExcitVal, const char customScaleName[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateAIStrainGageChan       (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, int32 strainConfig, int32 voltageExcitSource, float64 voltageExcitVal, float64 gageFactor, float64 initialBridgeVoltage, float64 nominalGageResistance, float64 poissonRatio, float64 leadWireResistance, const char customScaleName[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateAIVoltageChanWithExcit (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], int32 terminalConfig, float64 minVal, float64 maxVal, int32 units, int32 bridgeConfig, int32 voltageExcitSource, float64 voltageExcitVal, bool32 useExcitForScaling, const char customScaleName[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateAITempBuiltInSensorChan(TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], int32 units);
//int32 DllExport __CFUNC     DAQmxBaseCreateAIAccelChan            (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], int32 terminalConfig, float64 minVal, float64 maxVal, int32 units, float64 sensitivity, int32 sensitivityUnits, int32 currentExcitSource, float64 currentExcitVal, const char customScaleName[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateAIPosLVDTChan          (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, float64 sensitivity, int32 sensitivityUnits, int32 voltageExcitSource, float64 voltageExcitVal, float64 voltageExcitFreq, int32 ACExcitWireMode, const char customScaleName[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateAIPosRVDTChan          (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, float64 sensitivity, int32 sensitivityUnits, int32 voltageExcitSource, float64 voltageExcitVal, float64 voltageExcitFreq, int32 ACExcitWireMode, const char customScaleName[]);
// Function DAQmxBaseCreateAIDeviceTempChan is obsolete and has been replaced by DAQmxBaseCreateAITempBuiltInSensorChan
//int32 DllExport __CFUNC     DAQmxBaseCreateAIDeviceTempChan       (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], int32 units);

int32 DllExport __CFUNC     DAQmxBaseCreateAOVoltageChan          (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, const char customScaleName[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateAOCurrentChan          (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, const char customScaleName[]);

int32 DllExport __CFUNC     DAQmxBaseCreateDIChan                 (TaskHandle taskHandle, const char lines[], const char nameToAssignToLines[], int32 lineGrouping);

int32 DllExport __CFUNC     DAQmxBaseCreateDOChan                 (TaskHandle taskHandle, const char lines[], const char nameToAssignToLines[], int32 lineGrouping);

//int32 DllExport __CFUNC     DAQmxBaseCreateCIFreqChan             (TaskHandle taskHandle, const char counter[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, int32 edge, int32 measMethod, float64 measTime, uInt32 divisor, const char customScaleName[]);
int32 DllExport __CFUNC     DAQmxBaseCreateCIPeriodChan           (TaskHandle taskHandle, const char counter[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, int32 edge, int32 measMethod, float64 measTime, uInt32 divisor, const char customScaleName[]);
int32 DllExport __CFUNC     DAQmxBaseCreateCICountEdgesChan       (TaskHandle taskHandle, const char counter[], const char nameToAssignToChannel[], int32 edge, uInt32 initialCount, int32 countDirection);
int32 DllExport __CFUNC     DAQmxBaseCreateCIPulseWidthChan       (TaskHandle taskHandle, const char counter[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, int32 startingEdge, const char customScaleName[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateCISemiPeriodChan       (TaskHandle taskHandle, const char counter[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, const char customScaleName[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateCITwoEdgeSepChan       (TaskHandle taskHandle, const char counter[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, int32 firstEdge, int32 secondEdge, const char customScaleName[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateCILinEncoderChan       (TaskHandle taskHandle, const char counter[], const char nameToAssignToChannel[], int32 decodingType, bool32 ZidxEnable, float64 ZidxVal, int32 ZidxPhase, int32 units, float64 distPerPulse, float64 initialPos, const char customScaleName[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateCIAngEncoderChan       (TaskHandle taskHandle, const char counter[], const char nameToAssignToChannel[], int32 decodingType, bool32 ZidxEnable, float64 ZidxVal, int32 ZidxPhase, int32 units, uInt32 pulsesPerRev, float64 initialAngle, const char customScaleName[]);

int32 DllExport __CFUNC     DAQmxBaseCreateCOPulseChanFreq        (TaskHandle taskHandle, const char counter[], const char nameToAssignToChannel[], int32 units, int32 idleState, float64 initialDelay, float64 freq, float64 dutyCycle);
//int32 DllExport __CFUNC     DAQmxBaseCreateCOPulseChanTime        (TaskHandle taskHandle, const char counter[], const char nameToAssignToChannel[], int32 units, int32 idleState, float64 initialDelay, float64 lowTime, float64 highTime);
//int32 DllExport __CFUNC     DAQmxBaseCreateCOPulseChanTicks       (TaskHandle taskHandle, const char counter[], const char nameToAssignToChannel[], const char sourceTerminal[], int32 idleState, int32 initialDelay, int32 lowTicks, int32 highTicks);

//int32 DllExport __CFUNC_C   DAQmxBaseGetChanAttribute             (TaskHandle taskHandle, const char channel[], int32 attribute, void *value, ...);
//int32 DllExport __CFUNC_C   DAQmxBaseSetChanAttribute             (TaskHandle taskHandle, const char channel[], int32 attribute, ...);
//int32 DllExport __CFUNC     DAQmxBaseResetChanAttribute           (TaskHandle taskHandle, const char channel[], int32 attribute);


/******************************************************/
/***                    Timing                      ***/
/******************************************************/


// (Analog/Counter Timing)
int32 DllExport __CFUNC     DAQmxBaseCfgSampClkTiming          (TaskHandle taskHandle, const char source[], float64 rate, int32 activeEdge, int32 sampleMode, uInt64 sampsPerChan);
// (Digital Timing)
//int32 DllExport __CFUNC     DAQmxBaseCfgHandshakingTiming      (TaskHandle taskHandle, int32 sampleMode, uInt64 sampsPerChan);
//int32 DllExport __CFUNC     DAQmxBaseCfgChangeDetectionTiming  (TaskHandle taskHandle, const char risingEdgeChan[], const char fallingEdgeChan[], int32 sampleMode, uInt64 sampsPerChan);
// (Counter Timing)
int32 DllExport __CFUNC     DAQmxBaseCfgImplicitTiming         (TaskHandle taskHandle, int32 sampleMode, uInt64 sampsPerChan);

//int32 DllExport __CFUNC_C   DAQmxBaseGetTimingAttribute        (TaskHandle taskHandle, int32 attribute, void *value, ...);
//int32 DllExport __CFUNC_C   DAQmxBaseSetTimingAttribute        (TaskHandle taskHandle, int32 attribute, ...);
//int32 DllExport __CFUNC     DAQmxBaseResetTimingAttribute      (TaskHandle taskHandle, int32 attribute);


/******************************************************/
/***                  Triggering                    ***/
/******************************************************/


int32 DllExport __CFUNC     DAQmxBaseDisableStartTrig      (TaskHandle taskHandle);
int32 DllExport __CFUNC     DAQmxBaseCfgDigEdgeStartTrig   (TaskHandle taskHandle, const char triggerSource[], int32 triggerEdge);
int32 DllExport __CFUNC     DAQmxBaseCfgAnlgEdgeStartTrig  (TaskHandle taskHandle, const char triggerSource[], int32 triggerSlope, float64 triggerLevel);
//int32 DllExport __CFUNC     DAQmxBaseCfgAnlgWindowStartTrig(TaskHandle taskHandle, const char triggerSource[], int32 triggerWhen, float64 windowTop, float64 windowBottom);

int32 DllExport __CFUNC     DAQmxBaseDisableRefTrig        (TaskHandle taskHandle);
int32 DllExport __CFUNC     DAQmxBaseCfgDigEdgeRefTrig     (TaskHandle taskHandle, const char triggerSource[], int32 triggerEdge, uInt32 pretriggerSamples);
int32 DllExport __CFUNC     DAQmxBaseCfgAnlgEdgeRefTrig    (TaskHandle taskHandle, const char triggerSource[], int32 triggerSlope, float64 triggerLevel, uInt32 pretriggerSamples);
//int32 DllExport __CFUNC     DAQmxBaseCfgAnlgWindowRefTrig  (TaskHandle taskHandle, const char triggerSource[], int32 triggerWhen, float64 windowTop, float64 windowBottom, uInt32 pretriggerSamples);

//int32 DllExport __CFUNC     DAQmxBaseDisableAdvTrig        (TaskHandle taskHandle);
//int32 DllExport __CFUNC     DAQmxBaseCfgDigEdgeAdvTrig     (TaskHandle taskHandle, const char triggerSource[], int32 triggerEdge);

//int32 DllExport __CFUNC_C   DAQmxBaseGetTrigAttribute      (TaskHandle taskHandle, int32 attribute, void *value, ...);
//int32 DllExport __CFUNC_C   DAQmxBaseSetTrigAttribute      (TaskHandle taskHandle, int32 attribute, ...);
//int32 DllExport __CFUNC     DAQmxBaseResetTrigAttribute    (TaskHandle taskHandle, int32 attribute);

//int32 DllExport __CFUNC     DAQmxBaseSendSoftwareTrigger   (TaskHandle taskHandle, int32 triggerID);


/******************************************************/
/***                 Read Data                      ***/
/******************************************************/


int32 DllExport __CFUNC     DAQmxBaseReadAnalogF64         (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, float64 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved);
//int32 DllExport __CFUNC     DAQmxBaseReadAnalogScalarF64   (TaskHandle taskHandle, float64 timeout, float64 *value, bool32 *reserved);

int32 DllExport __CFUNC     DAQmxBaseReadBinaryI16         (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, int16 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved);

//int32 DllExport __CFUNC     DAQmxBaseReadBinaryU16         (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, uInt16 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved);

int32 DllExport __CFUNC     DAQmxReadBinaryI32             (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, int32 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved);

//int32 DllExport __CFUNC     DAQmxReadBinaryU32             (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, uInt32 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved);

int32 DllExport __CFUNC     DAQmxBaseReadDigitalU8         (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, uInt8 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved);
int32 DllExport __CFUNC     DAQmxBaseReadDigitalU32        (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, uInt32 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved);
int32 DllExport __CFUNC     DAQmxBaseReadDigitalScalarU32  (TaskHandle taskHandle, float64 timeout, uInt32 *value, bool32 *reserved);
//int32 DllExport __CFUNC       DAQmxBaseReadDigitalLines      (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, uInt8 readArray[], uInt32 arraySizeInBytes, int32 *sampsPerChanRead, int32 *numBytesPerSamp, bool32 *reserved);

int32 DllExport __CFUNC     DAQmxBaseReadCounterF64        (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, float64 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved);
int32 DllExport __CFUNC     DAQmxBaseReadCounterU32        (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, uInt32 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved);
int32 DllExport __CFUNC     DAQmxBaseReadCounterScalarF64  (TaskHandle taskHandle, float64 timeout, float64 *value, bool32 *reserved);
int32 DllExport __CFUNC     DAQmxBaseReadCounterScalarU32  (TaskHandle taskHandle, float64 timeout, uInt32 *value, bool32 *reserved);

//int32 DllExport __CFUNC     DAQmxBaseReadRaw               (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, void *readArray, uInt32 arraySizeInBytes, int32 *sampsRead, int32 *numBytesPerSamp, bool32 *reserved);

//int32 DllExport __CFUNC     DAQmxBaseGetNthTaskReadChannel (TaskHandle taskHandle, uInt32 index, char buffer[], int32 bufferSize);

//int32 DllExport __CFUNC_C   DAQmxBaseGetReadAttribute      (TaskHandle taskHandle, int32 attribute, void *value, ...);
//int32 DllExport __CFUNC_C   DAQmxBaseSetReadAttribute      (TaskHandle taskHandle, int32 attribute, ...);
//int32 DllExport __CFUNC     DAQmxBaseResetReadAttribute    (TaskHandle taskHandle, int32 attribute);


/******************************************************/
/***                 Write Data                     ***/
/******************************************************/


int32 DllExport __CFUNC     DAQmxBaseWriteAnalogF64          (TaskHandle taskHandle, int32 numSampsPerChan, bool32 autoStart, float64 timeout, bool32 dataLayout, float64 writeArray[], int32 *sampsPerChanWritten, bool32 *reserved);
//int32 DllExport __CFUNC     DAQmxBaseWriteAnalogScalarF64    (TaskHandle taskHandle, bool32 autoStart, float64 timeout, float64 value, bool32 *reserved);

//int32 DllExport __CFUNC     DAQmxBaseWriteBinaryI16          (TaskHandle taskHandle, int32 numSampsPerChan, bool32 autoStart, float64 timeout, bool32 dataLayout, int16 writeArray[], int32 *sampsPerChanWritten, bool32 *reserved);
//int32 DllExport __CFUNC     DAQmxBaseWriteBinaryU16          (TaskHandle taskHandle, int32 numSampsPerChan, bool32 autoStart, float64 timeout, bool32 dataLayout, uInt16 writeArray[], int32 *sampsPerChanWritten, bool32 *reserved);

int32 DllExport __CFUNC     DAQmxBaseWriteDigitalU8          (TaskHandle taskHandle, int32 numSampsPerChan, bool32 autoStart, float64 timeout, bool32 dataLayout, uInt8 writeArray[], int32 *sampsPerChanWritten, bool32 *reserved);
int32 DllExport __CFUNC     DAQmxBaseWriteDigitalU32         (TaskHandle taskHandle, int32 numSampsPerChan, bool32 autoStart, float64 timeout, bool32 dataLayout, uInt32 writeArray[], int32 *sampsPerChanWritten, bool32 *reserved);
int32 DllExport __CFUNC     DAQmxBaseWriteDigitalScalarU32   (TaskHandle taskHandle, bool32 autoStart, float64 timeout, uInt32 value, bool32 *reserved);
//int32 DllExport __CFUNC     DAQmxBaseWriteDigitalLines       (TaskHandle taskHandle, int32 numSampsPerChan, bool32 autoStart, float64 timeout, bool32 dataLayout, uInt8 writeArray[], int32 *sampsPerChanWritten, bool32 *reserved);

//int32 DllExport __CFUNC     DAQmxBaseWriteRaw                (TaskHandle taskHandle, int32 numSamps, bool32 autoStart, float64 timeout, void *writeArray, int32 *sampsPerChanWritten, bool32 *reserved);

//int32 DllExport __CFUNC_C   DAQmxBaseGetWriteAttribute       (TaskHandle taskHandle, int32 attribute, void *value);
//int32 DllExport __CFUNC_C   DAQmxBaseSetWriteAttribute       (TaskHandle taskHandle, int32 attribute, ...);
//int32 DllExport __CFUNC     DAQmxBaseResetWriteAttribute     (TaskHandle taskHandle, int32 attribute);


/******************************************************/
/***               Events & Signals                 ***/
/******************************************************/

// Terminology:  For hardware, "signals" comprise "clocks," "triggers," and (output) "events".
// Software signals or events are not presently supported.

//int32 DllExport __CFUNC     DAQmxBaseExportSignal                (TaskHandle taskHandle, int32 signalID, const char outputTerminal[]);

//int32 DllExport __CFUNC_C   DAQmxBaseGetExportedSignalAttribute  (TaskHandle taskHandle, int32 attribute, void *value, ...);
//int32 DllExport __CFUNC_C   DAQmxBaseSetExportedSignalAttribute  (TaskHandle taskHandle, int32 attribute, ...);
//int32 DllExport __CFUNC     DAQmxBaseResetExportedSignalAttribute(TaskHandle taskHandle, int32 attribute);


/******************************************************/
/***              Scale Configurations              ***/
/******************************************************/


//int32 DllExport __CFUNC     DAQmxBaseCreateLinScale             (const char name[], float64 slope, float64 yIntercept, int32 preScaledUnits, const char scaledUnits[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateMapScale             (const char name[], float64 prescaledMin, float64 prescaledMax, float64 scaledMin, float64 scaledMax, int32 preScaledUnits, const char scaledUnits[]);
//int32 DllExport __CFUNC     DAQmxBaseCreatePolynomialScale      (const char name[], float64 forwardCoeffs[], uInt32 numForwardCoeffsIn, float64 reverseCoeffs[], uInt32 numReverseCoeffsIn, int32 preScaledUnits, const char scaledUnits[]);
//int32 DllExport __CFUNC     DAQmxBaseCreateTableScale           (const char name[], float64 prescaledVals[], uInt32 numPrescaledValsIn, float64 scaledVals[], uInt32 numScaledValsIn, int32 preScaledUnits, const char scaledUnits[]);
//int32 DllExport __CFUNC     DAQmxBaseCalculateReversePolyCoeff  (float64 forwardCoeffs[], uInt32 numForwardCoeffsIn, float64 minValX, float64 maxValX, int32 numPointsToCompute, int32 reversePolyOrder, float64 reverseCoeffs[]);

//int32 DllExport __CFUNC_C   DAQmxBaseGetScaleAttribute          (const char scaleName[], int32 attribute, void *value, ...);
//int32 DllExport __CFUNC_C   DAQmxBaseSetScaleAttribute          (const char scaleName[], int32 attribute, ...);


/******************************************************/
/***             Buffer Configurations              ***/
/******************************************************/


int32 DllExport __CFUNC     DAQmxBaseCfgInputBuffer   (TaskHandle taskHandle, uInt32 numSampsPerChan);
//int32 DllExport __CFUNC     DAQmxBaseCfgOutputBuffer  (TaskHandle taskHandle, uInt32 numSampsPerChan);


/******************************************************/
/***                Switch Functions                ***/
/******************************************************/


//int32 DllExport __CFUNC     DAQmxBaseSwitchCreateScanList      (const char scanList[], TaskHandle *taskHandle);

//int32 DllExport __CFUNC     DAQmxBaseSwitchConnect             (const char switchChannel1[], const char switchChannel2[], bool32 waitForSettling);
//int32 DllExport __CFUNC     DAQmxBaseSwitchConnectMulti        (const char connectionList[], bool32 waitForSettling);
//int32 DllExport __CFUNC     DAQmxBaseSwitchDisconnect          (const char switchChannel1[], const char switchChannel2[], bool32 waitForSettling);
//int32 DllExport __CFUNC     DAQmxBaseSwitchDisconnectMulti     (const char connectionList[], bool32 waitForSettling);
//int32 DllExport __CFUNC     DAQmxBaseSwitchDisconnectAll       (const char deviceName[], bool32 waitForSettling);

// Switch Topologies
#define DAQmx_Val_Switch_Topology_1127_1_Wire_64x1_Mux         "1127/1-Wire 64x1 Mux"           // 1127/1-Wire 64x1 Mux
#define DAQmx_Val_Switch_Topology_1127_2_Wire_32x1_Mux         "1127/2-Wire 32x1 Mux"           // 1127/2-Wire 32x1 Mux
#define DAQmx_Val_Switch_Topology_1127_2_Wire_4x8_Matrix       "1127/2-Wire 4x8 Matrix"         // 1127/2-Wire 4x8 Matrix
#define DAQmx_Val_Switch_Topology_1127_4_Wire_16x1_Mux         "1127/4-Wire 16x1 Mux"           // 1127/4-Wire 16x1 Mux
#define DAQmx_Val_Switch_Topology_1127_Independent             "1127/Independent"               // 1127/Independent
#define DAQmx_Val_Switch_Topology_1128_1_Wire_64x1_Mux         "1128/1-Wire 64x1 Mux"           // 1128/1-Wire 64x1 Mux
#define DAQmx_Val_Switch_Topology_1128_2_Wire_32x1_Mux         "1128/2-Wire 32x1 Mux"           // 1128/2-Wire 32x1 Mux
#define DAQmx_Val_Switch_Topology_1128_2_Wire_4x8_Matrix       "1128/2-Wire 4x8 Matrix"         // 1128/2-Wire 4x8 Matrix
#define DAQmx_Val_Switch_Topology_1128_4_Wire_16x1_Mux         "1128/4-Wire 16x1 Mux"           // 1128/4-Wire 16x1 Mux
#define DAQmx_Val_Switch_Topology_1128_Independent             "1128/Independent"               // 1128/Independent
#define DAQmx_Val_Switch_Topology_1129_2_Wire_16x16_Matrix     "1129/2-Wire 16x16 Matrix"       // 1129/2-Wire 16x16 Matrix
#define DAQmx_Val_Switch_Topology_1129_2_Wire_8x32_Matrix      "1129/2-Wire 8x32 Matrix"        // 1129/2-Wire 8x32 Matrix
#define DAQmx_Val_Switch_Topology_1129_2_Wire_4x64_Matrix      "1129/2-Wire 4x64 Matrix"        // 1129/2-Wire 4x64 Matrix
#define DAQmx_Val_Switch_Topology_1129_2_Wire_Dual_8x16_Matrix "1129/2-Wire Dual 8x16 Matrix"   // 1129/2-Wire Dual 8x16 Matrix
#define DAQmx_Val_Switch_Topology_1129_2_Wire_Dual_4x32_Matrix "1129/2-Wire Dual 4x32 Matrix"   // 1129/2-Wire Dual 4x32 Matrix
#define DAQmx_Val_Switch_Topology_1129_2_Wire_Quad_4x16_Matrix "1129/2-Wire Quad 4x16 Matrix"   // 1129/2-Wire Quad 4x16 Matrix
#define DAQmx_Val_Switch_Topology_1130_1_Wire_256x1_Mux        "1130/1-Wire 256x1 Mux"          // 1130/1-Wire 256x1 Mux
#define DAQmx_Val_Switch_Topology_1130_2_Wire_128x1_Mux        "1130/2-Wire 128x1 Mux"          // 1130/2-Wire 128x1 Mux
#define DAQmx_Val_Switch_Topology_1130_4_Wire_64x1_Mux         "1130/4-Wire 64x1 Mux"           // 1130/4-Wire 64x1 Mux
#define DAQmx_Val_Switch_Topology_1130_1_Wire_4x64_Matrix      "1130/1-Wire 4x64 Matrix"        // 1130/1-Wire 4x64 Matrix
#define DAQmx_Val_Switch_Topology_1130_1_Wire_8x32_Matrix      "1130/1-Wire 8x32 Matrix"        // 1130/1-Wire 8x32 Matrix
#define DAQmx_Val_Switch_Topology_1130_2_Wire_4x32_Matrix      "1130/2-Wire 4x32 Matrix"        // 1130/2-Wire 4x32 Matrix
#define DAQmx_Val_Switch_Topology_1130_Independent             "1130/Independent"               // 1130/Independent
#define DAQmx_Val_Switch_Topology_1160_16_SPDT                 "1160/16-SPDT"                   // 1160/16-SPDT
#define DAQmx_Val_Switch_Topology_1161_8_SPDT                  "1161/8-SPDT"                    // 1161/8-SPDT
#define DAQmx_Val_Switch_Topology_1163R_Octal_4x1_Mux          "1163R/Octal 4x1 Mux"            // 1163R/Octal 4x1 Mux
#define DAQmx_Val_Switch_Topology_1166_32_SPDT                 "1166/32-SPDT"                   // 1166/32-SPDT
#define DAQmx_Val_Switch_Topology_1167_Independent             "1167/Independent"               // 1167/Independent
#define DAQmx_Val_Switch_Topology_1190_Quad_4x1_Mux            "1190/Quad 4x1 Mux"              // 1190/Quad 4x1 Mux
#define DAQmx_Val_Switch_Topology_1191_Quad_4x1_Mux            "1191/Quad 4x1 Mux"              // 1191/Quad 4x1 Mux
#define DAQmx_Val_Switch_Topology_1192_8_SPDT                  "1192/8-SPDT"                    // 1192/8-SPDT
#define DAQmx_Val_Switch_Topology_1193_32x1_Mux                "1193/32x1 Mux"                  // 1193/32x1 Mux
#define DAQmx_Val_Switch_Topology_1193_Dual_16x1_Mux           "1193/Dual 16x1 Mux"             // 1193/Dual 16x1 Mux
#define DAQmx_Val_Switch_Topology_1193_Quad_8x1_Mux            "1193/Quad 8x1 Mux"              // 1193/Quad 8x1 Mux
#define DAQmx_Val_Switch_Topology_1193_16x1_Terminated_Mux     "1193/16x1 Terminated Mux"       // 1193/16x1 Terminated Mux
#define DAQmx_Val_Switch_Topology_1193_Dual_8x1_Terminated_Mux "1193/Dual 8x1 Terminated Mux"   // 1193/Dual 8x1 Terminated Mux
#define DAQmx_Val_Switch_Topology_1193_Quad_4x1_Terminated_Mux "1193/Quad 4x1 Terminated Mux"   // 1193/Quad 4x1 Terminated Mux
#define DAQmx_Val_Switch_Topology_1193_Independent             "1193/Independent"               // 1193/Independent
#define DAQmx_Val_Switch_Topology_2529_2_Wire_8x16_Matrix      "2529/2-Wire 8x16 Matrix"        // 2529/2-Wire 8x16 Matrix
#define DAQmx_Val_Switch_Topology_2529_2_Wire_4x32_Matrix      "2529/2-Wire 4x32 Matrix"        // 2529/2-Wire 4x32 Matrix
#define DAQmx_Val_Switch_Topology_2529_2_Wire_Dual_4x16_Matrix "2529/2-Wire Dual 4x16 Matrix"   // 2529/2-Wire Dual 4x16 Matrix
#define DAQmx_Val_Switch_Topology_2530_1_Wire_128x1_Mux        "2530/1-Wire 128x1 Mux"          // 2530/1-Wire 128x1 Mux
#define DAQmx_Val_Switch_Topology_2530_2_Wire_64x1_Mux         "2530/2-Wire 64x1 Mux"           // 2530/2-Wire 64x1 Mux
#define DAQmx_Val_Switch_Topology_2530_4_Wire_32x1_Mux         "2530/4-Wire 32x1 Mux"           // 2530/4-Wire 32x1 Mux
#define DAQmx_Val_Switch_Topology_2530_1_Wire_4x32_Matrix      "2530/1-Wire 4x32 Matrix"        // 2530/1-Wire 4x32 Matrix
#define DAQmx_Val_Switch_Topology_2530_1_Wire_8x16_Matrix      "2530/1-Wire 8x16 Matrix"        // 2530/1-Wire 8x16 Matrix
#define DAQmx_Val_Switch_Topology_2530_2_Wire_4x16_Matrix      "2530/2-Wire 4x16 Matrix"        // 2530/2-Wire 4x16 Matrix
#define DAQmx_Val_Switch_Topology_2530_Independent             "2530/Independent"               // 2530/Independent
#define DAQmx_Val_Switch_Topology_2566_16_SPDT                 "2566/16-SPDT"                   // 2566/16-SPDT
#define DAQmx_Val_Switch_Topology_2567_Independent             "2567/Independent"               // 2567/Independent
#define DAQmx_Val_Switch_Topology_2570_40_SPDT                 "2570/40-SPDT"                   // 2570/40-SPDT
#define DAQmx_Val_Switch_Topology_2593_16x1_Mux                "2593/16x1 Mux"                  // 2593/16x1 Mux
#define DAQmx_Val_Switch_Topology_2593_Dual_8x1_Mux            "2593/Dual 8x1 Mux"              // 2593/Dual 8x1 Mux
#define DAQmx_Val_Switch_Topology_2593_8x1_Terminated_Mux      "2593/8x1 Terminated Mux"        // 2593/8x1 Terminated Mux
#define DAQmx_Val_Switch_Topology_2593_Dual_4x1_Terminated_Mux "2593/Dual 4x1 Terminated Mux"   // 2593/Dual 4x1 Terminated Mux
#define DAQmx_Val_Switch_Topology_2593_Independent             "2593/Independent"               // 2593/Independent

//int32 DllExport __CFUNC     DAQmxBaseSwitchSetTopologyAndReset (const char deviceName[], const char newTopology[]);

// For possible values of the output parameter pathStatus see value set SwitchPathType in Values section above.
//int32 DllExport __CFUNC     DAQmxBaseSwitchFindPath            (const char switchChannel1[], const char switchChannel2[], char path[], uInt32 pathBufferSize, int32 *pathStatus);

//int32 DllExport __CFUNC     DAQmxBaseSwitchOpenRelays          (const char relayList[], bool32 waitForSettling);
//int32 DllExport __CFUNC     DAQmxBaseSwitchCloseRelays         (const char relayList[], bool32 waitForSettling);

//int32 DllExport __CFUNC     DAQmxBaseSwitchGetSingleRelayCount (const char relayName[], uInt32 *count);
//int32 DllExport __CFUNC     DAQmxBaseSwitchGetMultiRelayCount  (const char relayList[], uInt32 count[], uInt32 countArraySize, uInt32 *numRelayCountsRead);
// For possible values of the output parameter relayPos see value set RelayPos in Values section above.
//int32 DllExport __CFUNC     DAQmxBaseSwitchGetSingleRelayPos   (const char relayName[], uInt32 *relayPos);
// For possible values in the output array relayPos see value set RelayPos in Values section above.
//int32 DllExport __CFUNC     DAQmxBaseSwitchGetMultiRelayPos    (const char relayList[], uInt32 relayPos[], uInt32 relayPosArraySize, uInt32 *numRelayPossRead);

//int32 DllExport __CFUNC     DAQmxBaseSwitchWaitForSettling     (const char deviceName[]);

//int32 DllExport __CFUNC_C   DAQmxBaseGetSwitchChanAttribute    (const char switchChannelName[], int32 attribute, void *value);
//int32 DllExport __CFUNC_C   DAQmxBaseSetSwitchChanAttribute    (const char switchChannelName[], int32 attribute, ...);

//int32 DllExport __CFUNC_C   DAQmxBaseGetSwitchDeviceAttribute  (const char deviceName[], int32 attribute, void *value, ...);
//int32 DllExport __CFUNC_C   DAQmxBaseSetSwitchDeviceAttribute  (const char deviceName[], int32 attribute, ...);

//int32 DllExport __CFUNC_C   DAQmxBaseGetSwitchScanAttribute    (TaskHandle taskHandle, int32 attribute, void *value);
//int32 DllExport __CFUNC_C   DAQmxBaseSetSwitchScanAttribute    (TaskHandle taskHandle, int32 attribute, ...);
//int32 DllExport __CFUNC     DAQmxBaseResetSwitchScanAttribute  (TaskHandle taskHandle, int32 attribute);


/******************************************************/
/***                Signal Routing                  ***/
/******************************************************/


//int32 DllExport __CFUNC     DAQmxBaseConnectTerms         (const char sourceTerminal[], const char destinationTerminal[], int32 signalModifiers);
//int32 DllExport __CFUNC     DAQmxBaseDisconnectTerms      (const char sourceTerminal[], const char destinationTerminal[]);
//int32 DllExport __CFUNC     DAQmxBaseTristateOutputTerm   (const char outputTerminal[]);


/******************************************************/
/***                Device Control                  ***/
/******************************************************/


int32 DllExport __CFUNC     DAQmxBaseResetDevice              (const char deviceName[]);

//int32 DllExport __CFUNC_C   DAQmxBaseGetDeviceAttribute       (const char deviceName[], int32 attribute, void *value, ...);

/******************************************************/
/***              Watchdog Timer                    ***/
/******************************************************/


//int32 DllExport __CFUNC_C   DAQmxBaseCreateWatchdogTimerTask    (const char deviceName[], const char taskName[], TaskHandle *taskHandle, float64 timeout, const char lines[], int32 expState, ...);
//int32 DllExport __CFUNC     DAQmxBaseControlWatchdogTask        (TaskHandle taskHandle, int32 action);

//int32 DllExport __CFUNC_C  DAQmxBaseGetWatchdogAttribute        (TaskHandle taskHandle, const char lines[], int32 attribute, void *value, ...);
//int32 DllExport __CFUNC_C  DAQmxBaseSetWatchdogAttribute        (TaskHandle taskHandle, const char lines[], int32 attribute, ...);
//int32 DllExport __CFUNC    DAQmxBaseResetWatchdogAttribute      (TaskHandle taskHandle, const char lines[], int32 attribute);


/******************************************************/
/***                 Calibration                    ***/
/******************************************************/


//int32 DllExport __CFUNC     DAQmxBaseSelfCal                    (const char deviceName[]);
//int32 DllExport __CFUNC     DAQmxBaseGetSelfCalLastDateAndTime  (const char deviceName[], uInt32 *year, uInt32 *month, uInt32 *day, uInt32 *hour, uInt32 *minute);
//int32 DllExport __CFUNC     DAQmxBaseGetExtCalLastDateAndTime   (const char deviceName[], uInt32 *year, uInt32 *month, uInt32 *day, uInt32 *hour, uInt32 *minute);
//int32 DllExport __CFUNC     DAQmxBaseRestoreLastExtCalConst     (const char deviceName[]);

//int32 DllExport __CFUNC     DAQmxBaseESeriesCalAdjust           (uInt32 calHandle, float64 referenceVoltage);
//int32 DllExport __CFUNC     DAQmxBaseSCBaseboardCalAdjust       (uInt32 calHandle, float64 referenceVoltage);
//int32 DllExport __CFUNC     DAQmxBaseAOSeriesCalAdjust          (uInt32 calHandle, float64 referenceVoltage);

//int32 DllExport __CFUNC     DAQmxBaseInitExtCal                 (const char deviceName[], const char password[], uInt32 *calHandle);
//int32 DllExport __CFUNC     DAQmxBaseCloseExtCal                (uInt32 calHandle, int32 action);
//int32 DllExport __CFUNC     DAQmxBaseChangeExtCalPassword       (const char deviceName[], const char password[], const char newPassword[]);

//int32 DllExport __CFUNC     DAQmxBaseDeviceSupportsCal          (const char deviceName[], bool32 *calSupported);

//int32 DllExport __CFUNC_C   DAQmxBaseGetCalInfoAttribute        (const char deviceName[], int32 attribute, void *value, ...);
//int32 DllExport __CFUNC_C   DAQmxBaseSetCalInfoAttribute        (const char deviceName[], int32 attribute, ...);

/******************************************************/
/***              System Configuration              ***/
/******************************************************/

//int32 DllExport __CFUNC_C   DAQmxBaseGetSystemInfoAttribute (int32 attribute, void *value, ...);
//int32 DllExport __CFUNC_C   DAQmxBaseSetDigitalPowerUpStates(const char deviceName[], const char channelNames[], int32 state, ...);

/******************************************************/
/***                 Error Handling                 ***/
/******************************************************/


//int32 DllExport __CFUNC     DAQmxBaseGetErrorString       (int32 errorCode, char errorString[], uInt32 bufferSize);
int32 DllExport __CFUNC     DAQmxBaseGetExtendedErrorInfo (char errorString[], uInt32 bufferSize);


/******************************************************************************
 *** NI-DAQmxBase Specific Attribute Get/Set/Reset Function Declarations **********
 ******************************************************************************/

//********** Calibration Info **********
//*** Set/Get functions for DAQmxBase_SelfCal_Supported ***
//int32 DllExport __CFUNC DAQmxBaseGetSelfCalSupported(const char deviceName[], bool32 *data);
//*** Set/Get functions for DAQmxBase_SelfCal_LastTemp ***
//int32 DllExport __CFUNC DAQmxBaseGetSelfCalLastTemp(const char deviceName[], float64 *data);
//*** Set/Get functions for DAQmxBase_ExtCal_RecommendedInterval ***
//int32 DllExport __CFUNC DAQmxBaseGetExtCalRecommendedInterval(const char deviceName[], uInt32 *data);
//*** Set/Get functions for DAQmxBase_ExtCal_LastTemp ***
//int32 DllExport __CFUNC DAQmxBaseGetExtCalLastTemp(const char deviceName[], float64 *data);
//*** Set/Get functions for DAQmxBase_Cal_UserDefinedInfo ***
//int32 DllExport __CFUNC DAQmxBaseGetCalUserDefinedInfo(const char deviceName[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCalUserDefinedInfo(const char deviceName[], const char *data);
//*** Set/Get functions for DAQmxBase_Cal_UserDefinedInfo_MaxSize ***
//int32 DllExport __CFUNC DAQmxBaseGetCalUserDefinedInfoMaxSize(const char deviceName[], uInt32 *data);

//********** Channel **********
//*** Set/Get functions for DAQmxBase_ChanType ***
// Uses value set ChannelType
//int32 DllExport __CFUNC DAQmxBaseGetChanType(TaskHandle taskHandle, const char channel[], int32 *data);
//*** Set/Get functions for DAQmxBase_PhysicalChanName ***
//int32 DllExport __CFUNC DAQmxBaseGetPhysicalChanName(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//*** Set/Get functions for DAQmxBase_ChanDescr ***
//int32 DllExport __CFUNC DAQmxBaseGetChanDescr(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetChanDescr(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetChanDescr(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Max ***
//int32 DllExport __CFUNC DAQmxBaseGetAIMax(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIMax(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIMax(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Min ***
//int32 DllExport __CFUNC DAQmxBaseGetAIMin(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIMin(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIMin(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_CustomScaleName ***
//int32 DllExport __CFUNC DAQmxBaseGetAICustomScaleName(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetAICustomScaleName(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetAICustomScaleName(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_MeasType ***
// Uses value set AIMeasurementType
//int32 DllExport __CFUNC DAQmxBaseGetAIMeasType(TaskHandle taskHandle, const char channel[], int32 *data);
//*** Set/Get functions for DAQmxBase_AI_Voltage_Units ***
// Uses value set VoltageUnits1
//int32 DllExport __CFUNC DAQmxBaseGetAIVoltageUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIVoltageUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIVoltageUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Temp_Units ***
// Uses value set TemperatureUnits1
//int32 DllExport __CFUNC DAQmxBaseGetAITempUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAITempUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAITempUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Thrmcpl_Type ***
// Uses value set ThermocoupleType1
//int32 DllExport __CFUNC DAQmxBaseGetAIThrmcplType(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIThrmcplType(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIThrmcplType(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Thrmcpl_CJCSrc ***
// Uses value set CJCSource1
//int32 DllExport __CFUNC DAQmxBaseGetAIThrmcplCJCSrc(TaskHandle taskHandle, const char channel[], int32 *data);
//*** Set/Get functions for DAQmxBase_AI_Thrmcpl_CJCVal ***
//int32 DllExport __CFUNC DAQmxBaseGetAIThrmcplCJCVal(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIThrmcplCJCVal(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIThrmcplCJCVal(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Thrmcpl_CJCChan ***
//int32 DllExport __CFUNC DAQmxBaseGetAIThrmcplCJCChan(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//*** Set/Get functions for DAQmxBase_AI_RTD_Type ***
// Uses value set RTDType1
//int32 DllExport __CFUNC DAQmxBaseGetAIRTDType(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIRTDType(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIRTDType(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_RTD_R0 ***
//int32 DllExport __CFUNC DAQmxBaseGetAIRTDR0(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIRTDR0(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIRTDR0(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_RTD_A ***
//int32 DllExport __CFUNC DAQmxBaseGetAIRTDA(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIRTDA(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIRTDA(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_RTD_B ***
//int32 DllExport __CFUNC DAQmxBaseGetAIRTDB(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIRTDB(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIRTDB(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_RTD_C ***
//int32 DllExport __CFUNC DAQmxBaseGetAIRTDC(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIRTDC(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIRTDC(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Thrmstr_A ***
//int32 DllExport __CFUNC DAQmxBaseGetAIThrmstrA(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIThrmstrA(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIThrmstrA(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Thrmstr_B ***
//int32 DllExport __CFUNC DAQmxBaseGetAIThrmstrB(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIThrmstrB(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIThrmstrB(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Thrmstr_C ***
//int32 DllExport __CFUNC DAQmxBaseGetAIThrmstrC(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIThrmstrC(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIThrmstrC(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Thrmstr_R1 ***
//int32 DllExport __CFUNC DAQmxBaseGetAIThrmstrR1(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIThrmstrR1(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIThrmstrR1(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_ForceReadFromChan ***
//int32 DllExport __CFUNC DAQmxBaseGetAIForceReadFromChan(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIForceReadFromChan(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIForceReadFromChan(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Current_Units ***
// Uses value set CurrentUnits1
//int32 DllExport __CFUNC DAQmxBaseGetAICurrentUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAICurrentUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAICurrentUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Strain_Units ***
// Uses value set StrainUnits1
//int32 DllExport __CFUNC DAQmxBaseGetAIStrainUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIStrainUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIStrainUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_StrainGage_GageFactor ***
//int32 DllExport __CFUNC DAQmxBaseGetAIStrainGageGageFactor(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIStrainGageGageFactor(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIStrainGageGageFactor(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_StrainGage_PoissonRatio ***
//int32 DllExport __CFUNC DAQmxBaseGetAIStrainGagePoissonRatio(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIStrainGagePoissonRatio(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIStrainGagePoissonRatio(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_StrainGage_Cfg ***
// Uses value set StrainGageBridgeType1
//int32 DllExport __CFUNC DAQmxBaseGetAIStrainGageCfg(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIStrainGageCfg(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIStrainGageCfg(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Resistance_Units ***
// Uses value set ResistanceUnits1
//int32 DllExport __CFUNC DAQmxBaseGetAIResistanceUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIResistanceUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIResistanceUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Freq_Units ***
// Uses value set FrequencyUnits
//int32 DllExport __CFUNC DAQmxBaseGetAIFreqUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIFreqUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIFreqUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Freq_ThreshVoltage ***
//int32 DllExport __CFUNC DAQmxBaseGetAIFreqThreshVoltage(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIFreqThreshVoltage(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIFreqThreshVoltage(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Freq_Hyst ***
//int32 DllExport __CFUNC DAQmxBaseGetAIFreqHyst(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIFreqHyst(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIFreqHyst(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_LVDT_Units ***
// Uses value set LengthUnits2
//int32 DllExport __CFUNC DAQmxBaseGetAILVDTUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAILVDTUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAILVDTUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_LVDT_Sensitivity ***
//int32 DllExport __CFUNC DAQmxBaseGetAILVDTSensitivity(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAILVDTSensitivity(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAILVDTSensitivity(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_LVDT_SensitivityUnits ***
// Uses value set LVDTSensitivityUnits1
//int32 DllExport __CFUNC DAQmxBaseGetAILVDTSensitivityUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAILVDTSensitivityUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAILVDTSensitivityUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_RVDT_Units ***
// Uses value set AngleUnits1
//int32 DllExport __CFUNC DAQmxBaseGetAIRVDTUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIRVDTUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIRVDTUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_RVDT_Sensitivity ***
//int32 DllExport __CFUNC DAQmxBaseGetAIRVDTSensitivity(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIRVDTSensitivity(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIRVDTSensitivity(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_RVDT_SensitivityUnits ***
// Uses value set RVDTSensitivityUnits1
//int32 DllExport __CFUNC DAQmxBaseGetAIRVDTSensitivityUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIRVDTSensitivityUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIRVDTSensitivityUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Accel_Units ***
// Uses value set AccelUnits2
//int32 DllExport __CFUNC DAQmxBaseGetAIAccelUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIAccelUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIAccelUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Accel_Sensitivity ***
//int32 DllExport __CFUNC DAQmxBaseGetAIAccelSensitivity(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIAccelSensitivity(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIAccelSensitivity(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Accel_SensitivityUnits ***
// Uses value set AccelSensitivityUnits1
//int32 DllExport __CFUNC DAQmxBaseGetAIAccelSensitivityUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIAccelSensitivityUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIAccelSensitivityUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Coupling ***
// Uses value set Coupling1
//int32 DllExport __CFUNC DAQmxBaseGetAICoupling(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAICoupling(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAICoupling(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Impedance ***
// Uses value set Impedance1
//int32 DllExport __CFUNC DAQmxBaseGetAIImpedance(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIImpedance(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIImpedance(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_TermCfg ***
// Uses value set InputTermCfg
//int32 DllExport __CFUNC DAQmxBaseGetAITermCfg(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAITermCfg(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAITermCfg(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_ResistanceCfg ***
// Uses value set ResistanceConfiguration
//int32 DllExport __CFUNC DAQmxBaseGetAIResistanceCfg(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIResistanceCfg(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIResistanceCfg(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_LeadWireResistance ***
//int32 DllExport __CFUNC DAQmxBaseGetAILeadWireResistance(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAILeadWireResistance(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAILeadWireResistance(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Bridge_Cfg ***
// Uses value set BridgeConfiguration1
//int32 DllExport __CFUNC DAQmxBaseGetAIBridgeCfg(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIBridgeCfg(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIBridgeCfg(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Bridge_NomResistance ***
//int32 DllExport __CFUNC DAQmxBaseGetAIBridgeNomResistance(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIBridgeNomResistance(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIBridgeNomResistance(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Bridge_InitialVoltage ***
//int32 DllExport __CFUNC DAQmxBaseGetAIBridgeInitialVoltage(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIBridgeInitialVoltage(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIBridgeInitialVoltage(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Bridge_ShuntCal_Enable ***
//int32 DllExport __CFUNC DAQmxBaseGetAIBridgeShuntCalEnable(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIBridgeShuntCalEnable(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIBridgeShuntCalEnable(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Bridge_ShuntCal_Select ***
// Uses value set ShuntCalSelect
//int32 DllExport __CFUNC DAQmxBaseGetAIBridgeShuntCalSelect(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIBridgeShuntCalSelect(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIBridgeShuntCalSelect(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Bridge_ShuntCal_GainAdjust ***
//int32 DllExport __CFUNC DAQmxBaseGetAIBridgeShuntCalGainAdjust(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIBridgeShuntCalGainAdjust(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIBridgeShuntCalGainAdjust(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Bridge_Balance_CoarsePot ***
//int32 DllExport __CFUNC DAQmxBaseGetAIBridgeBalanceCoarsePot(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIBridgeBalanceCoarsePot(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIBridgeBalanceCoarsePot(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Bridge_Balance_FinePot ***
//int32 DllExport __CFUNC DAQmxBaseGetAIBridgeBalanceFinePot(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIBridgeBalanceFinePot(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIBridgeBalanceFinePot(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_CurrentShunt_Loc ***
// Uses value set CurrentShuntResistorLocation1
//int32 DllExport __CFUNC DAQmxBaseGetAICurrentShuntLoc(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAICurrentShuntLoc(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAICurrentShuntLoc(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_CurrentShunt_Resistance ***
//int32 DllExport __CFUNC DAQmxBaseGetAICurrentShuntResistance(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAICurrentShuntResistance(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAICurrentShuntResistance(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Excit_Src ***
// Uses value set ExcitationSource
//int32 DllExport __CFUNC DAQmxBaseGetAIExcitSrc(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIExcitSrc(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIExcitSrc(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Excit_Val ***
//int32 DllExport __CFUNC DAQmxBaseGetAIExcitVal(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIExcitVal(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIExcitVal(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Excit_UseForScaling ***
//int32 DllExport __CFUNC DAQmxBaseGetAIExcitUseForScaling(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIExcitUseForScaling(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIExcitUseForScaling(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Excit_UseMultiplexed ***
//int32 DllExport __CFUNC DAQmxBaseGetAIExcitUseMultiplexed(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIExcitUseMultiplexed(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIExcitUseMultiplexed(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Excit_ActualVal ***
//int32 DllExport __CFUNC DAQmxBaseGetAIExcitActualVal(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIExcitActualVal(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIExcitActualVal(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Excit_DCorAC ***
// Uses value set ExcitationDCorAC
//int32 DllExport __CFUNC DAQmxBaseGetAIExcitDCorAC(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIExcitDCorAC(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIExcitDCorAC(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Excit_VoltageOrCurrent ***
// Uses value set ExcitationVoltageOrCurrent
//int32 DllExport __CFUNC DAQmxBaseGetAIExcitVoltageOrCurrent(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIExcitVoltageOrCurrent(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIExcitVoltageOrCurrent(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_ACExcit_Freq ***
//int32 DllExport __CFUNC DAQmxBaseGetAIACExcitFreq(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIACExcitFreq(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIACExcitFreq(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_ACExcit_SyncEnable ***
//int32 DllExport __CFUNC DAQmxBaseGetAIACExcitSyncEnable(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIACExcitSyncEnable(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIACExcitSyncEnable(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_ACExcit_WireMode ***
// Uses value set ACExcitWireMode
//int32 DllExport __CFUNC DAQmxBaseGetAIACExcitWireMode(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIACExcitWireMode(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIACExcitWireMode(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Atten ***
//int32 DllExport __CFUNC DAQmxBaseGetAIAtten(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIAtten(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIAtten(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Lowpass_Enable ***
//int32 DllExport __CFUNC DAQmxBaseGetAILowpassEnable(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAILowpassEnable(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAILowpassEnable(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Lowpass_CutoffFreq ***
//int32 DllExport __CFUNC DAQmxBaseGetAILowpassCutoffFreq(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAILowpassCutoffFreq(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAILowpassCutoffFreq(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Lowpass_SwitchCap_ClkSrc ***
// Uses value set SourceSelection
//int32 DllExport __CFUNC DAQmxBaseGetAILowpassSwitchCapClkSrc(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAILowpassSwitchCapClkSrc(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAILowpassSwitchCapClkSrc(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Lowpass_SwitchCap_ExtClkFreq ***
//int32 DllExport __CFUNC DAQmxBaseGetAILowpassSwitchCapExtClkFreq(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAILowpassSwitchCapExtClkFreq(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAILowpassSwitchCapExtClkFreq(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Lowpass_SwitchCap_ExtClkDiv ***
//int32 DllExport __CFUNC DAQmxBaseGetAILowpassSwitchCapExtClkDiv(TaskHandle taskHandle, const char channel[], uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAILowpassSwitchCapExtClkDiv(TaskHandle taskHandle, const char channel[], uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAILowpassSwitchCapExtClkDiv(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Lowpass_SwitchCap_OutClkDiv ***
//int32 DllExport __CFUNC DAQmxBaseGetAILowpassSwitchCapOutClkDiv(TaskHandle taskHandle, const char channel[], uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAILowpassSwitchCapOutClkDiv(TaskHandle taskHandle, const char channel[], uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAILowpassSwitchCapOutClkDiv(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_ResolutionUnits ***
// Uses value set ResolutionType1
//int32 DllExport __CFUNC DAQmxBaseGetAIResolutionUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//*** Set/Get functions for DAQmxBase_AI_Resolution ***
//int32 DllExport __CFUNC DAQmxBaseGetAIResolution(TaskHandle taskHandle, const char channel[], float64 *data);
//*** Set/Get functions for DAQmxBase_AI_Dither_Enable ***
//int32 DllExport __CFUNC DAQmxBaseGetAIDitherEnable(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIDitherEnable(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIDitherEnable(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Rng_High ***
//int32 DllExport __CFUNC DAQmxBaseGetAIRngHigh(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIRngHigh(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIRngHigh(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Rng_Low ***
//int32 DllExport __CFUNC DAQmxBaseGetAIRngLow(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIRngLow(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIRngLow(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_Gain ***
//int32 DllExport __CFUNC DAQmxBaseGetAIGain(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIGain(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIGain(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_SampAndHold_Enable ***
//int32 DllExport __CFUNC DAQmxBaseGetAISampAndHoldEnable(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAISampAndHoldEnable(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAISampAndHoldEnable(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_AutoZeroMode ***
// Uses value set AutoZeroType1
//int32 DllExport __CFUNC DAQmxBaseGetAIAutoZeroMode(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIAutoZeroMode(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIAutoZeroMode(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_DataXferMech ***
// Uses value set DataTransferMechanism
//int32 DllExport __CFUNC DAQmxBaseGetAIDataXferMech(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIDataXferMech(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIDataXferMech(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_DataXferReqCond ***
// Uses value set InputDataTransferCondition
//int32 DllExport __CFUNC DAQmxBaseGetAIDataXferReqCond(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIDataXferReqCond(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIDataXferReqCond(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_MemMapEnable ***
//int32 DllExport __CFUNC DAQmxBaseGetAIMemMapEnable(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIMemMapEnable(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIMemMapEnable(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AI_DevScalingCoeff ***
//int32 DllExport __CFUNC DAQmxBaseGetAIDevScalingCoeff(TaskHandle taskHandle, const char channel[], float64 *data, uInt32 arraySizeInSamples);
//*** Set/Get functions for DAQmxBase_AO_Max ***
//int32 DllExport __CFUNC DAQmxBaseGetAOMax(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAOMax(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAOMax(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_Min ***
//int32 DllExport __CFUNC DAQmxBaseGetAOMin(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAOMin(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAOMin(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_CustomScaleName ***
//int32 DllExport __CFUNC DAQmxBaseGetAOCustomScaleName(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetAOCustomScaleName(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetAOCustomScaleName(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_OutputType ***
// Uses value set AOOutputChannelType
//int32 DllExport __CFUNC DAQmxBaseGetAOOutputType(TaskHandle taskHandle, const char channel[], int32 *data);
//*** Set/Get functions for DAQmxBase_AO_Voltage_Units ***
// Uses value set VoltageUnits2
//int32 DllExport __CFUNC DAQmxBaseGetAOVoltageUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAOVoltageUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAOVoltageUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_Current_Units ***
// Uses value set CurrentUnits1
//int32 DllExport __CFUNC DAQmxBaseGetAOCurrentUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAOCurrentUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAOCurrentUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_OutputImpedance ***
//int32 DllExport __CFUNC DAQmxBaseGetAOOutputImpedance(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAOOutputImpedance(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAOOutputImpedance(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_LoadImpedance ***
//int32 DllExport __CFUNC DAQmxBaseGetAOLoadImpedance(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAOLoadImpedance(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAOLoadImpedance(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_ResolutionUnits ***
// Uses value set ResolutionType1
//int32 DllExport __CFUNC DAQmxBaseGetAOResolutionUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAOResolutionUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAOResolutionUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_Resolution ***
//int32 DllExport __CFUNC DAQmxBaseGetAOResolution(TaskHandle taskHandle, const char channel[], float64 *data);
//*** Set/Get functions for DAQmxBase_AO_DAC_Rng_High ***
//int32 DllExport __CFUNC DAQmxBaseGetAODACRngHigh(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAODACRngHigh(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAODACRngHigh(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_DAC_Rng_Low ***
//int32 DllExport __CFUNC DAQmxBaseGetAODACRngLow(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAODACRngLow(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAODACRngLow(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_DAC_Ref_ConnToGnd ***
//int32 DllExport __CFUNC DAQmxBaseGetAODACRefConnToGnd(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAODACRefConnToGnd(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAODACRefConnToGnd(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_DAC_Ref_AllowConnToGnd ***
//int32 DllExport __CFUNC DAQmxBaseGetAODACRefAllowConnToGnd(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAODACRefAllowConnToGnd(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAODACRefAllowConnToGnd(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_DAC_Ref_Src ***
// Uses value set SourceSelection
//int32 DllExport __CFUNC DAQmxBaseGetAODACRefSrc(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAODACRefSrc(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAODACRefSrc(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_DAC_Ref_Val ***
//int32 DllExport __CFUNC DAQmxBaseGetAODACRefVal(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAODACRefVal(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAODACRefVal(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_ReglitchEnable ***
//int32 DllExport __CFUNC DAQmxBaseGetAOReglitchEnable(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAOReglitchEnable(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAOReglitchEnable(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_UseOnlyOnBrdMem ***
//int32 DllExport __CFUNC DAQmxBaseGetAOUseOnlyOnBrdMem(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAOUseOnlyOnBrdMem(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAOUseOnlyOnBrdMem(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_DataXferMech ***
// Uses value set DataTransferMechanism
//int32 DllExport __CFUNC DAQmxBaseGetAODataXferMech(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAODataXferMech(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAODataXferMech(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_DataXferReqCond ***
// Uses value set OutputDataTransferCondition
//int32 DllExport __CFUNC DAQmxBaseGetAODataXferReqCond(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAODataXferReqCond(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAODataXferReqCond(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_MemMapEnable ***
//int32 DllExport __CFUNC DAQmxBaseGetAOMemMapEnable(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAOMemMapEnable(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAOMemMapEnable(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_AO_DevScalingCoeff ***
//int32 DllExport __CFUNC DAQmxBaseGetAODevScalingCoeff(TaskHandle taskHandle, const char channel[], float64 *data, uInt32 arraySizeInSamples);
//*** Set/Get functions for DAQmxBase_DI_InvertLines ***
//int32 DllExport __CFUNC DAQmxBaseGetDIInvertLines(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetDIInvertLines(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetDIInvertLines(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_DI_NumLines ***
//int32 DllExport __CFUNC DAQmxBaseGetDINumLines(TaskHandle taskHandle, const char channel[], uInt32 *data);
//*** Set/Get functions for DAQmxBase_DI_DigFltr_Enable ***
//int32 DllExport __CFUNC DAQmxBaseGetDIDigFltrEnable(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetDIDigFltrEnable(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetDIDigFltrEnable(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_DI_DigFltr_MinPulseWidth ***
//int32 DllExport __CFUNC DAQmxBaseGetDIDigFltrMinPulseWidth(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetDIDigFltrMinPulseWidth(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetDIDigFltrMinPulseWidth(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_DO_InvertLines ***
//int32 DllExport __CFUNC DAQmxBaseGetDOInvertLines(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetDOInvertLines(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetDOInvertLines(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_DO_NumLines ***
//int32 DllExport __CFUNC DAQmxBaseGetDONumLines(TaskHandle taskHandle, const char channel[], uInt32 *data);
//*** Set/Get functions for DAQmxBase_DO_Tristate ***
//int32 DllExport __CFUNC DAQmxBaseGetDOTristate(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetDOTristate(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetDOTristate(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Max ***
//int32 DllExport __CFUNC DAQmxBaseGetCIMax(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIMax(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIMax(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Min ***
//int32 DllExport __CFUNC DAQmxBaseGetCIMin(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIMin(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIMin(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_CustomScaleName ***
//int32 DllExport __CFUNC DAQmxBaseGetCICustomScaleName(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCICustomScaleName(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCICustomScaleName(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_MeasType ***
// Uses value set CIMeasurementType
//int32 DllExport __CFUNC DAQmxBaseGetCIMeasType(TaskHandle taskHandle, const char channel[], int32 *data);
//*** Set/Get functions for DAQmxBase_CI_Freq_Units ***
// Uses value set FrequencyUnits3
//int32 DllExport __CFUNC DAQmxBaseGetCIFreqUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIFreqUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIFreqUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Freq_Term ***
//int32 DllExport __CFUNC DAQmxBaseGetCIFreqTerm(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCIFreqTerm(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCIFreqTerm(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Freq_StartingEdge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetCIFreqStartingEdge(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIFreqStartingEdge(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIFreqStartingEdge(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Freq_MeasMeth ***
// Uses value set CounterFrequencyMethod
//int32 DllExport __CFUNC DAQmxBaseGetCIFreqMeasMeth(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIFreqMeasMeth(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIFreqMeasMeth(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Freq_MeasTime ***
//int32 DllExport __CFUNC DAQmxBaseGetCIFreqMeasTime(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIFreqMeasTime(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIFreqMeasTime(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Freq_Div ***
//int32 DllExport __CFUNC DAQmxBaseGetCIFreqDiv(TaskHandle taskHandle, const char channel[], uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIFreqDiv(TaskHandle taskHandle, const char channel[], uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIFreqDiv(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Period_Units ***
// Uses value set TimeUnits3
//int32 DllExport __CFUNC DAQmxBaseGetCIPeriodUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIPeriodUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIPeriodUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Period_Term ***
//int32 DllExport __CFUNC DAQmxBaseGetCIPeriodTerm(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCIPeriodTerm(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCIPeriodTerm(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Period_StartingEdge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetCIPeriodStartingEdge(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIPeriodStartingEdge(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIPeriodStartingEdge(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Period_MeasMeth ***
// Uses value set CounterFrequencyMethod
//int32 DllExport __CFUNC DAQmxBaseGetCIPeriodMeasMeth(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIPeriodMeasMeth(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIPeriodMeasMeth(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Period_MeasTime ***
//int32 DllExport __CFUNC DAQmxBaseGetCIPeriodMeasTime(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIPeriodMeasTime(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIPeriodMeasTime(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Period_Div ***
//int32 DllExport __CFUNC DAQmxBaseGetCIPeriodDiv(TaskHandle taskHandle, const char channel[], uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIPeriodDiv(TaskHandle taskHandle, const char channel[], uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIPeriodDiv(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_CountEdges_Term ***
//int32 DllExport __CFUNC DAQmxBaseGetCICountEdgesTerm(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCICountEdgesTerm(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCICountEdgesTerm(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_CountEdges_Dir ***
// Uses value set CountDirection1
//int32 DllExport __CFUNC DAQmxBaseGetCICountEdgesDir(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCICountEdgesDir(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCICountEdgesDir(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_CountEdges_DirTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetCICountEdgesDirTerm(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCICountEdgesDirTerm(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCICountEdgesDirTerm(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_CountEdges_InitialCnt ***
//int32 DllExport __CFUNC DAQmxBaseGetCICountEdgesInitialCnt(TaskHandle taskHandle, const char channel[], uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCICountEdgesInitialCnt(TaskHandle taskHandle, const char channel[], uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCICountEdgesInitialCnt(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_CountEdges_ActiveEdge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetCICountEdgesActiveEdge(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCICountEdgesActiveEdge(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCICountEdgesActiveEdge(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_AngEncoder_Units ***
// Uses value set AngleUnits2
//int32 DllExport __CFUNC DAQmxBaseGetCIAngEncoderUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIAngEncoderUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIAngEncoderUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_AngEncoder_PulsesPerRev ***
//int32 DllExport __CFUNC DAQmxBaseGetCIAngEncoderPulsesPerRev(TaskHandle taskHandle, const char channel[], uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIAngEncoderPulsesPerRev(TaskHandle taskHandle, const char channel[], uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIAngEncoderPulsesPerRev(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_AngEncoder_InitialAngle ***
//int32 DllExport __CFUNC DAQmxBaseGetCIAngEncoderInitialAngle(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIAngEncoderInitialAngle(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIAngEncoderInitialAngle(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_LinEncoder_Units ***
// Uses value set LengthUnits3
//int32 DllExport __CFUNC DAQmxBaseGetCILinEncoderUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCILinEncoderUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCILinEncoderUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_LinEncoder_DistPerPulse ***
//int32 DllExport __CFUNC DAQmxBaseGetCILinEncoderDistPerPulse(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCILinEncoderDistPerPulse(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCILinEncoderDistPerPulse(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_LinEncoder_InitialPos ***
//int32 DllExport __CFUNC DAQmxBaseGetCILinEncoderInitialPos(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCILinEncoderInitialPos(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCILinEncoderInitialPos(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Encoder_DecodingType ***
// Uses value set EncoderType2
//int32 DllExport __CFUNC DAQmxBaseGetCIEncoderDecodingType(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIEncoderDecodingType(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIEncoderDecodingType(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Encoder_AInputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetCIEncoderAInputTerm(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCIEncoderAInputTerm(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCIEncoderAInputTerm(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Encoder_BInputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetCIEncoderBInputTerm(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCIEncoderBInputTerm(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCIEncoderBInputTerm(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Encoder_ZInputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetCIEncoderZInputTerm(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCIEncoderZInputTerm(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCIEncoderZInputTerm(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Encoder_ZIndexEnable ***
//int32 DllExport __CFUNC DAQmxBaseGetCIEncoderZIndexEnable(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIEncoderZIndexEnable(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIEncoderZIndexEnable(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Encoder_ZIndexVal ***
//int32 DllExport __CFUNC DAQmxBaseGetCIEncoderZIndexVal(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIEncoderZIndexVal(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIEncoderZIndexVal(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Encoder_ZIndexPhase ***
// Uses value set EncoderZIndexPhase1
//int32 DllExport __CFUNC DAQmxBaseGetCIEncoderZIndexPhase(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIEncoderZIndexPhase(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIEncoderZIndexPhase(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_PulseWidth_Units ***
// Uses value set TimeUnits3
//int32 DllExport __CFUNC DAQmxBaseGetCIPulseWidthUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIPulseWidthUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIPulseWidthUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_PulseWidth_Term ***
//int32 DllExport __CFUNC DAQmxBaseGetCIPulseWidthTerm(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCIPulseWidthTerm(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCIPulseWidthTerm(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_PulseWidth_StartingEdge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetCIPulseWidthStartingEdge(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIPulseWidthStartingEdge(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIPulseWidthStartingEdge(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_TwoEdgeSep_Units ***
// Uses value set TimeUnits3
//int32 DllExport __CFUNC DAQmxBaseGetCITwoEdgeSepUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCITwoEdgeSepUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCITwoEdgeSepUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_TwoEdgeSep_FirstTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetCITwoEdgeSepFirstTerm(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCITwoEdgeSepFirstTerm(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCITwoEdgeSepFirstTerm(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_TwoEdgeSep_FirstEdge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetCITwoEdgeSepFirstEdge(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCITwoEdgeSepFirstEdge(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCITwoEdgeSepFirstEdge(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_TwoEdgeSep_SecondTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetCITwoEdgeSepSecondTerm(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCITwoEdgeSepSecondTerm(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCITwoEdgeSepSecondTerm(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_TwoEdgeSep_SecondEdge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetCITwoEdgeSepSecondEdge(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCITwoEdgeSepSecondEdge(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCITwoEdgeSepSecondEdge(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_SemiPeriod_Units ***
// Uses value set TimeUnits3
//int32 DllExport __CFUNC DAQmxBaseGetCISemiPeriodUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCISemiPeriodUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCISemiPeriodUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_SemiPeriod_Term ***
//int32 DllExport __CFUNC DAQmxBaseGetCISemiPeriodTerm(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCISemiPeriodTerm(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCISemiPeriodTerm(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_CtrTimebaseSrc ***
//int32 DllExport __CFUNC DAQmxBaseGetCICtrTimebaseSrc(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCICtrTimebaseSrc(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCICtrTimebaseSrc(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_CtrTimebaseRate ***
//int32 DllExport __CFUNC DAQmxBaseGetCICtrTimebaseRate(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCICtrTimebaseRate(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCICtrTimebaseRate(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_CtrTimebaseActiveEdge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetCICtrTimebaseActiveEdge(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCICtrTimebaseActiveEdge(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCICtrTimebaseActiveEdge(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_Count ***
//int32 DllExport __CFUNC DAQmxBaseGetCICount(TaskHandle taskHandle, const char channel[], uInt32 *data);
//*** Set/Get functions for DAQmxBase_CI_OutputState ***
// Uses value set Level1
//int32 DllExport __CFUNC DAQmxBaseGetCIOutputState(TaskHandle taskHandle, const char channel[], int32 *data);
//*** Set/Get functions for DAQmxBase_CI_TCReached ***
//int32 DllExport __CFUNC DAQmxBaseGetCITCReached(TaskHandle taskHandle, const char channel[], bool32 *data);
//*** Set/Get functions for DAQmxBase_CI_CtrTimebaseMasterTimebaseDiv ***
// Uses value set RefClockMasterTimebaseDivisor
//int32 DllExport __CFUNC DAQmxBaseGetCICtrTimebaseMasterTimebaseDiv(TaskHandle taskHandle, const char channel[], uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCICtrTimebaseMasterTimebaseDiv(TaskHandle taskHandle, const char channel[], uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCICtrTimebaseMasterTimebaseDiv(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_DataXferMech ***
// Uses value set DataTransferMechanism
//int32 DllExport __CFUNC DAQmxBaseGetCIDataXferMech(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIDataXferMech(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIDataXferMech(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CI_NumPossiblyInvalidSamps ***
//int32 DllExport __CFUNC DAQmxBaseGetCINumPossiblyInvalidSamps(TaskHandle taskHandle, const char channel[], uInt32 *data);
//*** Set/Get functions for DAQmxBase_CI_DupCountPrevent ***
//int32 DllExport __CFUNC DAQmxBaseGetCIDupCountPrevent(TaskHandle taskHandle, const char channel[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCIDupCountPrevent(TaskHandle taskHandle, const char channel[], bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCIDupCountPrevent(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_OutputType ***
// Uses value set COOutputType
//int32 DllExport __CFUNC DAQmxBaseGetCOOutputType(TaskHandle taskHandle, const char channel[], int32 *data);
//*** Set/Get functions for DAQmxBase_CO_Pulse_IdleState ***
// Uses value set Level1
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseIdleState(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOPulseIdleState(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOPulseIdleState(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_Pulse_Term ***
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseTerm(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCOPulseTerm(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCOPulseTerm(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_Pulse_Time_Units ***
// Uses value set TimeUnits2
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseTimeUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOPulseTimeUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOPulseTimeUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_Pulse_HighTime ***
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseHighTime(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOPulseHighTime(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOPulseHighTime(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_Pulse_LowTime ***
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseLowTime(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOPulseLowTime(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOPulseLowTime(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_Pulse_Time_InitialDelay ***
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseTimeInitialDelay(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOPulseTimeInitialDelay(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOPulseTimeInitialDelay(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_Pulse_DutyCyc ***
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseDutyCyc(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOPulseDutyCyc(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOPulseDutyCyc(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_Pulse_Freq_Units ***
// Uses value set FrequencyUnits2
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseFreqUnits(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOPulseFreqUnits(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOPulseFreqUnits(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_Pulse_Freq ***
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseFreq(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOPulseFreq(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOPulseFreq(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_Pulse_Freq_InitialDelay ***
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseFreqInitialDelay(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOPulseFreqInitialDelay(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOPulseFreqInitialDelay(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_Pulse_HighTicks ***
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseHighTicks(TaskHandle taskHandle, const char channel[], uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOPulseHighTicks(TaskHandle taskHandle, const char channel[], uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOPulseHighTicks(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_Pulse_LowTicks ***
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseLowTicks(TaskHandle taskHandle, const char channel[], uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOPulseLowTicks(TaskHandle taskHandle, const char channel[], uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOPulseLowTicks(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_Pulse_Ticks_InitialDelay ***
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseTicksInitialDelay(TaskHandle taskHandle, const char channel[], uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOPulseTicksInitialDelay(TaskHandle taskHandle, const char channel[], uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOPulseTicksInitialDelay(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_CtrTimebaseSrc ***
//int32 DllExport __CFUNC DAQmxBaseGetCOCtrTimebaseSrc(TaskHandle taskHandle, const char channel[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetCOCtrTimebaseSrc(TaskHandle taskHandle, const char channel[], const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetCOCtrTimebaseSrc(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_CtrTimebaseRate ***
//int32 DllExport __CFUNC DAQmxBaseGetCOCtrTimebaseRate(TaskHandle taskHandle, const char channel[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOCtrTimebaseRate(TaskHandle taskHandle, const char channel[], float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOCtrTimebaseRate(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_CtrTimebaseActiveEdge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetCOCtrTimebaseActiveEdge(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOCtrTimebaseActiveEdge(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOCtrTimebaseActiveEdge(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_Count ***
//int32 DllExport __CFUNC DAQmxBaseGetCOCount(TaskHandle taskHandle, const char channel[], uInt32 *data);
//*** Set/Get functions for DAQmxBase_CO_OutputState ***
// Uses value set Level1
//int32 DllExport __CFUNC DAQmxBaseGetCOOutputState(TaskHandle taskHandle, const char channel[], int32 *data);
//*** Set/Get functions for DAQmxBase_CO_AutoIncrCnt ***
//int32 DllExport __CFUNC DAQmxBaseGetCOAutoIncrCnt(TaskHandle taskHandle, const char channel[], uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOAutoIncrCnt(TaskHandle taskHandle, const char channel[], uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOAutoIncrCnt(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_CtrTimebaseMasterTimebaseDiv ***
// Uses value set RefClockMasterTimebaseDivisor
//int32 DllExport __CFUNC DAQmxBaseGetCOCtrTimebaseMasterTimebaseDiv(TaskHandle taskHandle, const char channel[], uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetCOCtrTimebaseMasterTimebaseDiv(TaskHandle taskHandle, const char channel[], uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetCOCtrTimebaseMasterTimebaseDiv(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_CO_PulseDone ***
//int32 DllExport __CFUNC DAQmxBaseGetCOPulseDone(TaskHandle taskHandle, const char channel[], bool32 *data);

//********** Export Signal **********
//*** Set/Get functions for DAQmxBase_Exported_AIConvClk_OutputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetExportedAIConvClkOutputTerm(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetExportedAIConvClkOutputTerm(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedAIConvClkOutputTerm(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_AIConvClk_Pulse_Polarity ***
// Uses value set Polarity2
//int32 DllExport __CFUNC DAQmxBaseGetExportedAIConvClkPulsePolarity(TaskHandle taskHandle, int32 *data);
//*** Set/Get functions for DAQmxBase_Exported_20MHzTimebase_OutputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetExported20MHzTimebaseOutputTerm(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetExported20MHzTimebaseOutputTerm(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetExported20MHzTimebaseOutputTerm(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_SampClk_OutputBehavior ***
// Uses value set ExportActions3
//int32 DllExport __CFUNC DAQmxBaseGetExportedSampClkOutputBehavior(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetExportedSampClkOutputBehavior(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedSampClkOutputBehavior(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_SampClk_OutputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetExportedSampClkOutputTerm(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetExportedSampClkOutputTerm(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedSampClkOutputTerm(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_AdvTrig_OutputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetExportedAdvTrigOutputTerm(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetExportedAdvTrigOutputTerm(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedAdvTrigOutputTerm(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_AdvTrig_Pulse_Polarity ***
// Uses value set Polarity2
//int32 DllExport __CFUNC DAQmxBaseGetExportedAdvTrigPulsePolarity(TaskHandle taskHandle, int32 *data);
//*** Set/Get functions for DAQmxBase_Exported_AdvTrig_Pulse_WidthUnits ***
// Uses value set DigitalWidthUnits3
//int32 DllExport __CFUNC DAQmxBaseGetExportedAdvTrigPulseWidthUnits(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetExportedAdvTrigPulseWidthUnits(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedAdvTrigPulseWidthUnits(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_AdvTrig_Pulse_Width ***
//int32 DllExport __CFUNC DAQmxBaseGetExportedAdvTrigPulseWidth(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetExportedAdvTrigPulseWidth(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedAdvTrigPulseWidth(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_RefTrig_OutputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetExportedRefTrigOutputTerm(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetExportedRefTrigOutputTerm(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedRefTrigOutputTerm(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_StartTrig_OutputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetExportedStartTrigOutputTerm(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetExportedStartTrigOutputTerm(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedStartTrigOutputTerm(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_AdvCmpltEvent_OutputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetExportedAdvCmpltEventOutputTerm(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetExportedAdvCmpltEventOutputTerm(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedAdvCmpltEventOutputTerm(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_AdvCmpltEvent_Delay ***
//int32 DllExport __CFUNC DAQmxBaseGetExportedAdvCmpltEventDelay(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetExportedAdvCmpltEventDelay(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedAdvCmpltEventDelay(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_AdvCmpltEvent_Pulse_Polarity ***
// Uses value set Polarity2
//int32 DllExport __CFUNC DAQmxBaseGetExportedAdvCmpltEventPulsePolarity(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetExportedAdvCmpltEventPulsePolarity(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedAdvCmpltEventPulsePolarity(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_AdvCmpltEvent_Pulse_Width ***
//int32 DllExport __CFUNC DAQmxBaseGetExportedAdvCmpltEventPulseWidth(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetExportedAdvCmpltEventPulseWidth(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedAdvCmpltEventPulseWidth(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_AIHoldCmpltEvent_OutputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetExportedAIHoldCmpltEventOutputTerm(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetExportedAIHoldCmpltEventOutputTerm(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedAIHoldCmpltEventOutputTerm(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_AIHoldCmpltEvent_PulsePolarity ***
// Uses value set Polarity2
//int32 DllExport __CFUNC DAQmxBaseGetExportedAIHoldCmpltEventPulsePolarity(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetExportedAIHoldCmpltEventPulsePolarity(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedAIHoldCmpltEventPulsePolarity(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_ChangeDetectEvent_OutputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetExportedChangeDetectEventOutputTerm(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetExportedChangeDetectEventOutputTerm(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedChangeDetectEventOutputTerm(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_CtrOutEvent_OutputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetExportedCtrOutEventOutputTerm(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetExportedCtrOutEventOutputTerm(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedCtrOutEventOutputTerm(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_CtrOutEvent_OutputBehavior ***
// Uses value set ExportActions2
//int32 DllExport __CFUNC DAQmxBaseGetExportedCtrOutEventOutputBehavior(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetExportedCtrOutEventOutputBehavior(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedCtrOutEventOutputBehavior(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_CtrOutEvent_Pulse_Polarity ***
// Uses value set Polarity2
//int32 DllExport __CFUNC DAQmxBaseGetExportedCtrOutEventPulsePolarity(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetExportedCtrOutEventPulsePolarity(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedCtrOutEventPulsePolarity(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_CtrOutEvent_Toggle_IdleState ***
// Uses value set Level1
//int32 DllExport __CFUNC DAQmxBaseGetExportedCtrOutEventToggleIdleState(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetExportedCtrOutEventToggleIdleState(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedCtrOutEventToggleIdleState(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Exported_WatchdogExpiredEvent_OutputTerm ***
//int32 DllExport __CFUNC DAQmxBaseGetExportedWatchdogExpiredEventOutputTerm(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetExportedWatchdogExpiredEventOutputTerm(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetExportedWatchdogExpiredEventOutputTerm(TaskHandle taskHandle);

//********** Device **********
//*** Set/Get functions for DAQmxBase_Dev_ProductType ***
//int32 DllExport __CFUNC DAQmxBaseGetDevProductType(const char device[], char *data, uInt32 bufferSize);
//*** Set/Get functions for DAQmxBase_Dev_SerialNum ***
int32 DllExport __CFUNC DAQmxBaseGetDevSerialNum(const char device[], uInt32 *data);

//********** Read **********
//*** Set/Get functions for DAQmxBase_Read_RelativeTo ***
// Uses value set ReadRelativeTo
//int32 DllExport __CFUNC DAQmxBaseGetReadRelativeTo(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetReadRelativeTo(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetReadRelativeTo(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Read_Offset ***
//int32 DllExport __CFUNC DAQmxBaseGetReadOffset(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetReadOffset(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetReadOffset(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Read_ChannelsToRead ***
//int32 DllExport __CFUNC DAQmxBaseGetReadChannelsToRead(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetReadChannelsToRead(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetReadChannelsToRead(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Read_ReadAllAvailSamp ***
//int32 DllExport __CFUNC DAQmxBaseGetReadReadAllAvailSamp(TaskHandle taskHandle, bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetReadReadAllAvailSamp(TaskHandle taskHandle, bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetReadReadAllAvailSamp(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Read_AutoStart ***
//int32 DllExport __CFUNC DAQmxBaseGetReadAutoStart(TaskHandle taskHandle, bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetReadAutoStart(TaskHandle taskHandle, bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetReadAutoStart(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Read_OverWrite ***
// Uses value set OverwriteMode1
//int32 DllExport __CFUNC DAQmxBaseGetReadOverWrite(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetReadOverWrite(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetReadOverWrite(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Read_CurrReadPos ***
//int32 DllExport __CFUNC DAQmxBaseGetReadCurrReadPos(TaskHandle taskHandle, uInt64 *data);
//*** Set/Get functions for DAQmxBase_Read_AvailSampPerChan ***
//int32 DllExport __CFUNC DAQmxBaseGetReadAvailSampPerChan(TaskHandle taskHandle, uInt32 *data);
//*** Set/Get functions for DAQmxBase_Read_TotalSampPerChanAcquired ***
//int32 DllExport __CFUNC DAQmxBaseGetReadTotalSampPerChanAcquired(TaskHandle taskHandle, uInt64 *data);
//*** Set/Get functions for DAQmxBase_Read_ChangeDetect_HasOverflowed ***
//int32 DllExport __CFUNC DAQmxBaseGetReadChangeDetectHasOverflowed(TaskHandle taskHandle, bool32 *data);
//*** Set/Get functions for DAQmxBase_Read_RawDataWidth ***
//int32 DllExport __CFUNC DAQmxBaseGetReadRawDataWidth(TaskHandle taskHandle, uInt32 *data);
//*** Set/Get functions for DAQmxBase_Read_NumChans ***
//int32 DllExport __CFUNC DAQmxBaseGetReadNumChans(TaskHandle taskHandle, uInt32 *data);
//*** Set/Get functions for DAQmxBase_Read_DigitalLines_BytesPerChan ***
//int32 DllExport __CFUNC DAQmxBaseGetReadDigitalLinesBytesPerChan(TaskHandle taskHandle, uInt32 *data);

//********** Switch Channel **********
//*** Set/Get functions for DAQmxBase_SwitchChan_Usage ***
// Uses value set SwitchUsageTypes
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanUsage(const char switchChannelName[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSwitchChanUsage(const char switchChannelName[], int32 data);
//*** Set/Get functions for DAQmxBase_SwitchChan_MaxACCarryCurrent ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanMaxACCarryCurrent(const char switchChannelName[], float64 *data);
//*** Set/Get functions for DAQmxBase_SwitchChan_MaxACSwitchCurrent ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanMaxACSwitchCurrent(const char switchChannelName[], float64 *data);
//*** Set/Get functions for DAQmxBase_SwitchChan_MaxACCarryPwr ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanMaxACCarryPwr(const char switchChannelName[], float64 *data);
//*** Set/Get functions for DAQmxBase_SwitchChan_MaxACSwitchPwr ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanMaxACSwitchPwr(const char switchChannelName[], float64 *data);
//*** Set/Get functions for DAQmxBase_SwitchChan_MaxDCCarryCurrent ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanMaxDCCarryCurrent(const char switchChannelName[], float64 *data);
//*** Set/Get functions for DAQmxBase_SwitchChan_MaxDCSwitchCurrent ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanMaxDCSwitchCurrent(const char switchChannelName[], float64 *data);
//*** Set/Get functions for DAQmxBase_SwitchChan_MaxDCCarryPwr ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanMaxDCCarryPwr(const char switchChannelName[], float64 *data);
//*** Set/Get functions for DAQmxBase_SwitchChan_MaxDCSwitchPwr ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanMaxDCSwitchPwr(const char switchChannelName[], float64 *data);
//*** Set/Get functions for DAQmxBase_SwitchChan_MaxACVoltage ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanMaxACVoltage(const char switchChannelName[], float64 *data);
//*** Set/Get functions for DAQmxBase_SwitchChan_MaxDCVoltage ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanMaxDCVoltage(const char switchChannelName[], float64 *data);
//*** Set/Get functions for DAQmxBase_SwitchChan_WireMode ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanWireMode(const char switchChannelName[], uInt32 *data);
//*** Set/Get functions for DAQmxBase_SwitchChan_Bandwidth ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanBandwidth(const char switchChannelName[], float64 *data);
//*** Set/Get functions for DAQmxBase_SwitchChan_Impedance ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchChanImpedance(const char switchChannelName[], float64 *data);

//********** Switch Device **********
//*** Set/Get functions for DAQmxBase_SwitchDev_SettlingTime ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchDevSettlingTime(const char deviceName[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSwitchDevSettlingTime(const char deviceName[], float64 data);
//*** Set/Get functions for DAQmxBase_SwitchDev_AutoConnAnlgBus ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchDevAutoConnAnlgBus(const char deviceName[], bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSwitchDevAutoConnAnlgBus(const char deviceName[], bool32 data);
//*** Set/Get functions for DAQmxBase_SwitchDev_Settled ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchDevSettled(const char deviceName[], bool32 *data);
//*** Set/Get functions for DAQmxBase_SwitchDev_RelayList ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchDevRelayList(const char deviceName[], char *data, uInt32 bufferSize);
//*** Set/Get functions for DAQmxBase_SwitchDev_NumRelays ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchDevNumRelays(const char deviceName[], uInt32 *data);
//*** Set/Get functions for DAQmxBase_SwitchDev_SwitchChanList ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchDevSwitchChanList(const char deviceName[], char *data, uInt32 bufferSize);
//*** Set/Get functions for DAQmxBase_SwitchDev_NumSwitchChans ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchDevNumSwitchChans(const char deviceName[], uInt32 *data);
//*** Set/Get functions for DAQmxBase_SwitchDev_NumRows ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchDevNumRows(const char deviceName[], uInt32 *data);
//*** Set/Get functions for DAQmxBase_SwitchDev_NumColumns ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchDevNumColumns(const char deviceName[], uInt32 *data);
//*** Set/Get functions for DAQmxBase_SwitchDev_Topology ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchDevTopology(const char deviceName[], char *data, uInt32 bufferSize);

//********** Switch Scan **********
//*** Set/Get functions for DAQmxBase_SwitchScan_BreakMode ***
// Uses value set BreakMode
//int32 DllExport __CFUNC DAQmxBaseGetSwitchScanBreakMode(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSwitchScanBreakMode(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetSwitchScanBreakMode(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_SwitchScan_RepeatMode ***
// Uses value set SwitchScanRepeatMode
//int32 DllExport __CFUNC DAQmxBaseGetSwitchScanRepeatMode(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSwitchScanRepeatMode(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetSwitchScanRepeatMode(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_SwitchScan_WaitingForAdv ***
//int32 DllExport __CFUNC DAQmxBaseGetSwitchScanWaitingForAdv(TaskHandle taskHandle, bool32 *data);

//********** Scale **********
//*** Set/Get functions for DAQmxBase_Scale_Descr ***
//int32 DllExport __CFUNC DAQmxBaseGetScaleDescr(const char scaleName[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetScaleDescr(const char scaleName[], const char *data);
//*** Set/Get functions for DAQmxBase_Scale_ScaledUnits ***
//int32 DllExport __CFUNC DAQmxBaseGetScaleScaledUnits(const char scaleName[], char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetScaleScaledUnits(const char scaleName[], const char *data);
//*** Set/Get functions for DAQmxBase_Scale_PreScaledUnits ***
// Uses value set UnitsPreScaled
//int32 DllExport __CFUNC DAQmxBaseGetScalePreScaledUnits(const char scaleName[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetScalePreScaledUnits(const char scaleName[], int32 data);
//*** Set/Get functions for DAQmxBase_Scale_Type ***
// Uses value set ScaleType
//int32 DllExport __CFUNC DAQmxBaseGetScaleType(const char scaleName[], int32 *data);
//*** Set/Get functions for DAQmxBase_Scale_Lin_Slope ***
//int32 DllExport __CFUNC DAQmxBaseGetScaleLinSlope(const char scaleName[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetScaleLinSlope(const char scaleName[], float64 data);
//*** Set/Get functions for DAQmxBase_Scale_Lin_YIntercept ***
//int32 DllExport __CFUNC DAQmxBaseGetScaleLinYIntercept(const char scaleName[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetScaleLinYIntercept(const char scaleName[], float64 data);
//*** Set/Get functions for DAQmxBase_Scale_Map_ScaledMax ***
//int32 DllExport __CFUNC DAQmxBaseGetScaleMapScaledMax(const char scaleName[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetScaleMapScaledMax(const char scaleName[], float64 data);
//*** Set/Get functions for DAQmxBase_Scale_Map_PreScaledMax ***
//int32 DllExport __CFUNC DAQmxBaseGetScaleMapPreScaledMax(const char scaleName[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetScaleMapPreScaledMax(const char scaleName[], float64 data);
//*** Set/Get functions for DAQmxBase_Scale_Map_ScaledMin ***
//int32 DllExport __CFUNC DAQmxBaseGetScaleMapScaledMin(const char scaleName[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetScaleMapScaledMin(const char scaleName[], float64 data);
//*** Set/Get functions for DAQmxBase_Scale_Map_PreScaledMin ***
//int32 DllExport __CFUNC DAQmxBaseGetScaleMapPreScaledMin(const char scaleName[], float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetScaleMapPreScaledMin(const char scaleName[], float64 data);
//*** Set/Get functions for DAQmxBase_Scale_Poly_ForwardCoeff ***
//int32 DllExport __CFUNC DAQmxBaseGetScalePolyForwardCoeff(const char scaleName[], float64 *data, uInt32 arraySizeInSamples);
//int32 DllExport __CFUNC DAQmxBaseSetScalePolyForwardCoeff(const char scaleName[], float64 *data, uInt32 arraySizeInSamples);
//*** Set/Get functions for DAQmxBase_Scale_Poly_ReverseCoeff ***
//int32 DllExport __CFUNC DAQmxBaseGetScalePolyReverseCoeff(const char scaleName[], float64 *data, uInt32 arraySizeInSamples);
//int32 DllExport __CFUNC DAQmxBaseSetScalePolyReverseCoeff(const char scaleName[], float64 *data, uInt32 arraySizeInSamples);
//*** Set/Get functions for DAQmxBase_Scale_Table_ScaledVals ***
//int32 DllExport __CFUNC DAQmxBaseGetScaleTableScaledVals(const char scaleName[], float64 *data, uInt32 arraySizeInSamples);
//int32 DllExport __CFUNC DAQmxBaseSetScaleTableScaledVals(const char scaleName[], float64 *data, uInt32 arraySizeInSamples);
//*** Set/Get functions for DAQmxBase_Scale_Table_PreScaledVals ***
//int32 DllExport __CFUNC DAQmxBaseGetScaleTablePreScaledVals(const char scaleName[], float64 *data, uInt32 arraySizeInSamples);
//int32 DllExport __CFUNC DAQmxBaseSetScaleTablePreScaledVals(const char scaleName[], float64 *data, uInt32 arraySizeInSamples);

//********** System **********
//*** Set/Get functions for DAQmxBase_Sys_GlobalChans ***
//int32 DllExport __CFUNC DAQmxBaseGetSysGlobalChans(char *data, uInt32 bufferSize);
//*** Set/Get functions for DAQmxBase_Sys_Scales ***
//int32 DllExport __CFUNC DAQmxBaseGetSysScales(char *data, uInt32 bufferSize);
//*** Set/Get functions for DAQmxBase_Sys_Tasks ***
//int32 DllExport __CFUNC DAQmxBaseGetSysTasks(char *data, uInt32 bufferSize);
//*** Set/Get functions for DAQmxBase_Sys_DevNames ***
//int32 DllExport __CFUNC DAQmxBaseGetSysDevNames(char *data, uInt32 bufferSize);
//*** Set/Get functions for DAQmxBase_Sys_NIDAQMajorVersion ***
//int32 DllExport __CFUNC DAQmxBaseGetSysNIDAQMajorVersion(uInt32 *data);
//*** Set/Get functions for DAQmxBase_Sys_NIDAQMinorVersion ***
//int32 DllExport __CFUNC DAQmxBaseGetSysNIDAQMinorVersion(uInt32 *data);

//********** Task **********
//*** Set/Get functions for DAQmxBase_Task_Name ***
//int32 DllExport __CFUNC DAQmxBaseGetTaskName(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//*** Set/Get functions for DAQmxBase_Task_Channels ***
//int32 DllExport __CFUNC DAQmxBaseGetTaskChannels(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//*** Set/Get functions for DAQmxBase_Task_NumChans ***
//int32 DllExport __CFUNC DAQmxBaseGetTaskNumChans(TaskHandle taskHandle, uInt32 *data);
//*** Set/Get functions for DAQmxBase_Task_Complete ***
//int32 DllExport __CFUNC DAQmxBaseGetTaskComplete(TaskHandle taskHandle, bool32 *data);

//********** Timing **********
//*** Set/Get functions for DAQmxBase_SampQuant_SampMode ***
// Uses value set AcquisitionType
//int32 DllExport __CFUNC DAQmxBaseGetSampQuantSampMode(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSampQuantSampMode(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetSampQuantSampMode(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_SampQuant_SampPerChan ***
//int32 DllExport __CFUNC DAQmxBaseGetSampQuantSampPerChan(TaskHandle taskHandle, uInt64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSampQuantSampPerChan(TaskHandle taskHandle, uInt64 data);
//int32 DllExport __CFUNC DAQmxBaseResetSampQuantSampPerChan(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_SampTimingType ***
// Uses value set SampleTimingType
//int32 DllExport __CFUNC DAQmxBaseGetSampTimingType(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSampTimingType(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetSampTimingType(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_SampClk_Rate ***
//int32 DllExport __CFUNC DAQmxBaseGetSampClkRate(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSampClkRate(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetSampClkRate(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_SampClk_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetSampClkSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetSampClkSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetSampClkSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_SampClk_ActiveEdge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetSampClkActiveEdge(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSampClkActiveEdge(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetSampClkActiveEdge(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_SampClk_TimebaseDiv ***
//int32 DllExport __CFUNC DAQmxBaseGetSampClkTimebaseDiv(TaskHandle taskHandle, uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSampClkTimebaseDiv(TaskHandle taskHandle, uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetSampClkTimebaseDiv(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_SampClk_Timebase_Rate ***
//int32 DllExport __CFUNC DAQmxBaseGetSampClkTimebaseRate(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSampClkTimebaseRate(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetSampClkTimebaseRate(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_SampClk_Timebase_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetSampClkTimebaseSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetSampClkTimebaseSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetSampClkTimebaseSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_SampClk_Timebase_ActiveEdge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetSampClkTimebaseActiveEdge(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSampClkTimebaseActiveEdge(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetSampClkTimebaseActiveEdge(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_SampClk_Timebase_MasterTimebaseDiv ***
//int32 DllExport __CFUNC DAQmxBaseGetSampClkTimebaseMasterTimebaseDiv(TaskHandle taskHandle, uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetSampClkTimebaseMasterTimebaseDiv(TaskHandle taskHandle, uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetSampClkTimebaseMasterTimebaseDiv(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_ChangeDetect_DI_RisingEdgePhysicalChans ***
//int32 DllExport __CFUNC DAQmxBaseGetChangeDetectDIRisingEdgePhysicalChans(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetChangeDetectDIRisingEdgePhysicalChans(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetChangeDetectDIRisingEdgePhysicalChans(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_ChangeDetect_DI_FallingEdgePhysicalChans ***
//int32 DllExport __CFUNC DAQmxBaseGetChangeDetectDIFallingEdgePhysicalChans(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetChangeDetectDIFallingEdgePhysicalChans(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetChangeDetectDIFallingEdgePhysicalChans(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_OnDemand_SimultaneousAOEnable ***
//int32 DllExport __CFUNC DAQmxBaseGetOnDemandSimultaneousAOEnable(TaskHandle taskHandle, bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetOnDemandSimultaneousAOEnable(TaskHandle taskHandle, bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetOnDemandSimultaneousAOEnable(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AIConv_Rate ***
//int32 DllExport __CFUNC DAQmxBaseGetAIConvRate(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIConvRate(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIConvRate(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AIConv_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetAIConvSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetAIConvSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetAIConvSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AIConv_ActiveEdge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetAIConvActiveEdge(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIConvActiveEdge(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIConvActiveEdge(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AIConv_TimebaseDiv ***
//int32 DllExport __CFUNC DAQmxBaseGetAIConvTimebaseDiv(TaskHandle taskHandle, uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIConvTimebaseDiv(TaskHandle taskHandle, uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIConvTimebaseDiv(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AIConv_Timebase_Src ***
// Uses value set MIOAIConvertTbSrc
//int32 DllExport __CFUNC DAQmxBaseGetAIConvTimebaseSrc(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAIConvTimebaseSrc(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAIConvTimebaseSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_MasterTimebase_Rate ***
//int32 DllExport __CFUNC DAQmxBaseGetMasterTimebaseRate(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetMasterTimebaseRate(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetMasterTimebaseRate(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_MasterTimebase_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetMasterTimebaseSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetMasterTimebaseSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetMasterTimebaseSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DelayFromSampClk_DelayUnits ***
// Uses value set DigitalWidthUnits2
//int32 DllExport __CFUNC DAQmxBaseGetDelayFromSampClkDelayUnits(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetDelayFromSampClkDelayUnits(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetDelayFromSampClkDelayUnits(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DelayFromSampClk_Delay ***
//int32 DllExport __CFUNC DAQmxBaseGetDelayFromSampClkDelay(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetDelayFromSampClkDelay(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetDelayFromSampClkDelay(TaskHandle taskHandle);

//********** Trigger **********
//*** Set/Get functions for DAQmxBase_StartTrig_Type ***
// Uses value set TriggerType8
//int32 DllExport __CFUNC DAQmxBaseGetStartTrigType(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetStartTrigType(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetStartTrigType(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DigEdge_StartTrig_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetDigEdgeStartTrigSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetDigEdgeStartTrigSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetDigEdgeStartTrigSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DigEdge_StartTrig_Edge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetDigEdgeStartTrigEdge(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetDigEdgeStartTrigEdge(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetDigEdgeStartTrigEdge(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgEdge_StartTrig_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgEdgeStartTrigSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgEdgeStartTrigSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgEdgeStartTrigSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgEdge_StartTrig_Slope ***
// Uses value set Slope1
//int32 DllExport __CFUNC DAQmxBaseGetAnlgEdgeStartTrigSlope(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgEdgeStartTrigSlope(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgEdgeStartTrigSlope(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgEdge_StartTrig_Lvl ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgEdgeStartTrigLvl(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgEdgeStartTrigLvl(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgEdgeStartTrigLvl(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgEdge_StartTrig_Hyst ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgEdgeStartTrigHyst(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgEdgeStartTrigHyst(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgEdgeStartTrigHyst(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgWin_StartTrig_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgWinStartTrigSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgWinStartTrigSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgWinStartTrigSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgWin_StartTrig_When ***
// Uses value set WindowTriggerCondition1
//int32 DllExport __CFUNC DAQmxBaseGetAnlgWinStartTrigWhen(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgWinStartTrigWhen(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgWinStartTrigWhen(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgWin_StartTrig_Top ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgWinStartTrigTop(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgWinStartTrigTop(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgWinStartTrigTop(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgWin_StartTrig_Btm ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgWinStartTrigBtm(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgWinStartTrigBtm(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgWinStartTrigBtm(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_StartTrig_Delay ***
//int32 DllExport __CFUNC DAQmxBaseGetStartTrigDelay(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetStartTrigDelay(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetStartTrigDelay(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_StartTrig_DelayUnits ***
// Uses value set DigitalWidthUnits1
//int32 DllExport __CFUNC DAQmxBaseGetStartTrigDelayUnits(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetStartTrigDelayUnits(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetStartTrigDelayUnits(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_StartTrig_Retriggerable ***
//int32 DllExport __CFUNC DAQmxBaseGetStartTrigRetriggerable(TaskHandle taskHandle, bool32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetStartTrigRetriggerable(TaskHandle taskHandle, bool32 data);
//int32 DllExport __CFUNC DAQmxBaseResetStartTrigRetriggerable(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_RefTrig_Type ***
// Uses value set TriggerType1
//int32 DllExport __CFUNC DAQmxBaseGetRefTrigType(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetRefTrigType(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetRefTrigType(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_RefTrig_PretrigSamples ***
//int32 DllExport __CFUNC DAQmxBaseGetRefTrigPretrigSamples(TaskHandle taskHandle, uInt32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetRefTrigPretrigSamples(TaskHandle taskHandle, uInt32 data);
//int32 DllExport __CFUNC DAQmxBaseResetRefTrigPretrigSamples(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DigEdge_RefTrig_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetDigEdgeRefTrigSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetDigEdgeRefTrigSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetDigEdgeRefTrigSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DigEdge_RefTrig_Edge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetDigEdgeRefTrigEdge(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetDigEdgeRefTrigEdge(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetDigEdgeRefTrigEdge(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgEdge_RefTrig_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgEdgeRefTrigSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgEdgeRefTrigSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgEdgeRefTrigSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgEdge_RefTrig_Slope ***
// Uses value set Slope1
//int32 DllExport __CFUNC DAQmxBaseGetAnlgEdgeRefTrigSlope(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgEdgeRefTrigSlope(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgEdgeRefTrigSlope(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgEdge_RefTrig_Lvl ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgEdgeRefTrigLvl(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgEdgeRefTrigLvl(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgEdgeRefTrigLvl(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgEdge_RefTrig_Hyst ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgEdgeRefTrigHyst(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgEdgeRefTrigHyst(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgEdgeRefTrigHyst(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgWin_RefTrig_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgWinRefTrigSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgWinRefTrigSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgWinRefTrigSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgWin_RefTrig_When ***
// Uses value set WindowTriggerCondition1
//int32 DllExport __CFUNC DAQmxBaseGetAnlgWinRefTrigWhen(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgWinRefTrigWhen(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgWinRefTrigWhen(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgWin_RefTrig_Top ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgWinRefTrigTop(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgWinRefTrigTop(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgWinRefTrigTop(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgWin_RefTrig_Btm ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgWinRefTrigBtm(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgWinRefTrigBtm(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgWinRefTrigBtm(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AdvTrig_Type ***
// Uses value set TriggerType5
//int32 DllExport __CFUNC DAQmxBaseGetAdvTrigType(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAdvTrigType(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAdvTrigType(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DigEdge_AdvTrig_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetDigEdgeAdvTrigSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetDigEdgeAdvTrigSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetDigEdgeAdvTrigSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DigEdge_AdvTrig_Edge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetDigEdgeAdvTrigEdge(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetDigEdgeAdvTrigEdge(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetDigEdgeAdvTrigEdge(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_PauseTrig_Type ***
// Uses value set TriggerType6
//int32 DllExport __CFUNC DAQmxBaseGetPauseTrigType(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetPauseTrigType(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetPauseTrigType(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgLvl_PauseTrig_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgLvlPauseTrigSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgLvlPauseTrigSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgLvlPauseTrigSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgLvl_PauseTrig_When ***
// Uses value set ActiveLevel
//int32 DllExport __CFUNC DAQmxBaseGetAnlgLvlPauseTrigWhen(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgLvlPauseTrigWhen(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgLvlPauseTrigWhen(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgLvl_PauseTrig_Lvl ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgLvlPauseTrigLvl(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgLvlPauseTrigLvl(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgLvlPauseTrigLvl(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgLvl_PauseTrig_Hyst ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgLvlPauseTrigHyst(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgLvlPauseTrigHyst(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgLvlPauseTrigHyst(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgWin_PauseTrig_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgWinPauseTrigSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgWinPauseTrigSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgWinPauseTrigSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgWin_PauseTrig_When ***
// Uses value set WindowTriggerCondition2
//int32 DllExport __CFUNC DAQmxBaseGetAnlgWinPauseTrigWhen(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgWinPauseTrigWhen(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgWinPauseTrigWhen(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgWin_PauseTrig_Top ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgWinPauseTrigTop(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgWinPauseTrigTop(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgWinPauseTrigTop(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_AnlgWin_PauseTrig_Btm ***
//int32 DllExport __CFUNC DAQmxBaseGetAnlgWinPauseTrigBtm(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetAnlgWinPauseTrigBtm(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetAnlgWinPauseTrigBtm(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DigLvl_PauseTrig_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetDigLvlPauseTrigSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetDigLvlPauseTrigSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetDigLvlPauseTrigSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DigLvl_PauseTrig_When ***
// Uses value set Level1
//int32 DllExport __CFUNC DAQmxBaseGetDigLvlPauseTrigWhen(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetDigLvlPauseTrigWhen(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetDigLvlPauseTrigWhen(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_ArmStartTrig_Type ***
// Uses value set TriggerType4
//int32 DllExport __CFUNC DAQmxBaseGetArmStartTrigType(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetArmStartTrigType(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetArmStartTrigType(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DigEdge_ArmStartTrig_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetDigEdgeArmStartTrigSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetDigEdgeArmStartTrigSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetDigEdgeArmStartTrigSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DigEdge_ArmStartTrig_Edge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetDigEdgeArmStartTrigEdge(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetDigEdgeArmStartTrigEdge(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetDigEdgeArmStartTrigEdge(TaskHandle taskHandle);

//********** Watchdog **********
//*** Set/Get functions for DAQmxBase_Watchdog_Timeout ***
//int32 DllExport __CFUNC DAQmxBaseGetWatchdogTimeout(TaskHandle taskHandle, float64 *data);
//int32 DllExport __CFUNC DAQmxBaseSetWatchdogTimeout(TaskHandle taskHandle, float64 data);
//int32 DllExport __CFUNC DAQmxBaseResetWatchdogTimeout(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_WatchdogExpirTrig_Type ***
// Uses value set TriggerType4
//int32 DllExport __CFUNC DAQmxBaseGetWatchdogExpirTrigType(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetWatchdogExpirTrigType(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetWatchdogExpirTrigType(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DigEdge_WatchdogExpirTrig_Src ***
//int32 DllExport __CFUNC DAQmxBaseGetDigEdgeWatchdogExpirTrigSrc(TaskHandle taskHandle, char *data, uInt32 bufferSize);
//int32 DllExport __CFUNC DAQmxBaseSetDigEdgeWatchdogExpirTrigSrc(TaskHandle taskHandle, const char *data);
//int32 DllExport __CFUNC DAQmxBaseResetDigEdgeWatchdogExpirTrigSrc(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_DigEdge_WatchdogExpirTrig_Edge ***
// Uses value set Edge1
//int32 DllExport __CFUNC DAQmxBaseGetDigEdgeWatchdogExpirTrigEdge(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetDigEdgeWatchdogExpirTrigEdge(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetDigEdgeWatchdogExpirTrigEdge(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Watchdog_DO_ExpirState ***
// Uses value set DigitalLineState
//int32 DllExport __CFUNC DAQmxBaseGetWatchdogDOExpirState(TaskHandle taskHandle, const char channel[], int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetWatchdogDOExpirState(TaskHandle taskHandle, const char channel[], int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetWatchdogDOExpirState(TaskHandle taskHandle, const char channel[]);
//*** Set/Get functions for DAQmxBase_Watchdog_HasExpired ***
//int32 DllExport __CFUNC DAQmxBaseGetWatchdogHasExpired(TaskHandle taskHandle, bool32 *data);

//********** Write **********
//*** Set/Get functions for DAQmxBase_Write_RelativeTo ***
// Uses value set WriteRelativeTo
//int32 DllExport __CFUNC DAQmxBaseGetWriteRelativeTo(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetWriteRelativeTo(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetWriteRelativeTo(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Write_Offset ***
//int32 DllExport __CFUNC DAQmxBaseGetWriteOffset(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetWriteOffset(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetWriteOffset(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Write_RegenMode ***
// Uses value set RegenerationMode1
//int32 DllExport __CFUNC DAQmxBaseGetWriteRegenMode(TaskHandle taskHandle, int32 *data);
//int32 DllExport __CFUNC DAQmxBaseSetWriteRegenMode(TaskHandle taskHandle, int32 data);
//int32 DllExport __CFUNC DAQmxBaseResetWriteRegenMode(TaskHandle taskHandle);
//*** Set/Get functions for DAQmxBase_Write_CurrWritePos ***
//int32 DllExport __CFUNC DAQmxBaseGetWriteCurrWritePos(TaskHandle taskHandle, uInt64 *data);
//*** Set/Get functions for DAQmxBase_Write_SpaceAvail ***
//int32 DllExport __CFUNC DAQmxBaseGetWriteSpaceAvail(TaskHandle taskHandle, uInt32 *data);
//*** Set/Get functions for DAQmxBase_Write_TotalSampPerChanGenerated ***
//int32 DllExport __CFUNC DAQmxBaseGetWriteTotalSampPerChanGenerated(TaskHandle taskHandle, uInt64 *data);
//*** Set/Get functions for DAQmxBase_Write_RawDataWidth ***
//int32 DllExport __CFUNC DAQmxBaseGetWriteRawDataWidth(TaskHandle taskHandle, uInt32 *data);
//*** Set/Get functions for DAQmxBase_Write_NumChans ***
//int32 DllExport __CFUNC DAQmxBaseGetWriteNumChans(TaskHandle taskHandle, uInt32 *data);
//*** Set/Get functions for DAQmxBase_Write_DigitalLines_BytesPerChan ***
//int32 DllExport __CFUNC DAQmxBaseGetWriteDigitalLinesBytesPerChan(TaskHandle taskHandle, uInt32 *data);


/******************************************************************************
 *** NI-DAQmx Error Codes *****************************************************
 ******************************************************************************/

#define DAQmxSuccess                                  (0)

//#define DAQmxFailed(error)                            ((error)<0)
#define DAQmxFailed(error)                            ((error)!=0)

// Error and Warning Codes
#define DAQmxErrorInvalidInstallation                                          (-200683)
#define DAQmxErrorRefTrigMasterSessionUnavailable                              (-200682)
#define DAQmxErrorRouteFailedBecauseWatchdogExpired                            (-200681)
#define DAQmxErrorDeviceShutDownDueToHighTemp                                  (-200680)
#define DAQmxErrorNoMemMapWhenHWTimedSinglePoint                               (-200679)
#define DAQmxErrorWriteFailedBecauseWatchdogExpired                            (-200678)
#define DAQmxErrorDifftInternalAIInputSrcs                                     (-200677)
#define DAQmxErrorDifftAIInputSrcInOneChanGroup                                (-200676)
#define DAQmxErrorInternalAIInputSrcInMultChanGroups                           (-200675)
#define DAQmxErrorSwitchOpFailedDueToPrevError                                 (-200674)
#define DAQmxErrorWroteMultiSampsUsingSingleSampWrite                          (-200673)
#define DAQmxErrorMismatchedInputArraySizes                                    (-200672)
#define DAQmxErrorCantExceedRelayDriveLimit                                    (-200671)
#define DAQmxErrorDACRngLowNotEqualToMinusRefVal                               (-200670)
#define DAQmxErrorCantAllowConnectDACToGnd                                     (-200669)
#define DAQmxErrorWatchdogTimeoutOutOfRangeAndNotSpecialVal                    (-200668)
#define DAQmxErrorNoWatchdogOutputOnPortReservedForInput                       (-200667)
#define DAQmxErrorNoInputOnPortCfgdForWatchdogOutput                           (-200666)
#define DAQmxErrorWatchdogExpirationStateNotEqualForLinesInPort                (-200665)
#define DAQmxErrorCannotPerformOpWhenTaskNotReserved                           (-200664)
#define DAQmxErrorPowerupStateNotSupported                                     (-200663)
#define DAQmxErrorWatchdogTimerNotSupported                                    (-200662)
#define DAQmxErrorOpNotSupportedWhenRefClkSrcNone                              (-200661)
#define DAQmxErrorSampClkRateUnavailable                                       (-200660)
#define DAQmxErrorPrptyGetSpecdSingleActiveChanFailedDueToDifftVals            (-200659)
#define DAQmxErrorPrptyGetImpliedActiveChanFailedDueToDifftVals                (-200658)
#define DAQmxErrorPrptyGetSpecdActiveChanFailedDueToDifftVals                  (-200657)
#define DAQmxErrorNoRegenWhenUsingBrdMem                                       (-200656)
#define DAQmxErrorNonbufferedReadMoreThanSampsPerChan                          (-200655)
#define DAQmxErrorWatchdogExpirationTristateNotSpecdForEntirePort              (-200654)
#define DAQmxErrorPowerupTristateNotSpecdForEntirePort                         (-200653)
#define DAQmxErrorPowerupStateNotSpecdForEntirePort                            (-200652)
#define DAQmxErrorCantSetWatchdogExpirationOnDigInChan                         (-200651)
#define DAQmxErrorCantSetPowerupStateOnDigInChan                               (-200650)
#define DAQmxErrorPhysChanNotInTask                                            (-200649)
#define DAQmxErrorPhysChanDevNotInTask                                         (-200648)
#define DAQmxErrorDigInputNotSupported                                         (-200647)
#define DAQmxErrorDigFilterIntervalNotEqualForLines                            (-200646)
#define DAQmxErrorDigFilterIntervalAlreadyCfgd                                 (-200645)
#define DAQmxErrorCantResetExpiredWatchdog                                     (-200644)
#define DAQmxErrorActiveChanTooManyLinesSpecdWhenGettingPrpty                  (-200643)
#define DAQmxErrorActiveChanNotSpecdWhenGetting1LinePrpty                      (-200642)
#define DAQmxErrorDigPrptyCannotBeSetPerLine                                   (-200641)
#define DAQmxErrorSendAdvCmpltAfterWaitForTrigInScanlist                       (-200640)
#define DAQmxErrorDisconnectionRequiredInScanlist                              (-200639)
#define DAQmxErrorTwoWaitForTrigsAfterConnectionInScanlist                     (-200638)
#define DAQmxErrorActionSeparatorRequiredAfterBreakingConnectionInScanlist     (-200637)
#define DAQmxErrorConnectionInScanlistMustWaitForTrig                          (-200636)
#define DAQmxErrorActionNotSupportedTaskNotWatchdog                            (-200635)
#define DAQmxErrorWfmNameSameAsScriptName                                      (-200634)
#define DAQmxErrorScriptNameSameAsWfmName                                      (-200633)
#define DAQmxErrorDSFStopClock                                                 (-200632)
#define DAQmxErrorDSFReadyForStartClock                                        (-200631)
#define DAQmxErrorWriteOffsetNotMultOfIncr                                     (-200630)
#define DAQmxErrorDifferentPrptyValsNotSupportedOnDev                          (-200629)
#define DAQmxErrorRefAndPauseTrigConfigured                                    (-200628)
#define DAQmxErrorFailedToEnableHighSpeedInputClock                            (-200627)
#define DAQmxErrorEmptyPhysChanInPowerUpStatesArray                            (-200626)
#define DAQmxErrorActivePhysChanTooManyLinesSpecdWhenGettingPrpty              (-200625)
#define DAQmxErrorActivePhysChanNotSpecdWhenGetting1LinePrpty                  (-200624)
#define DAQmxErrorPXIDevTempCausedShutDown                                     (-200623)
#define DAQmxErrorInvalidNumSampsToWrite                                       (-200622)
#define DAQmxErrorOutputFIFOUnderflow2                                         (-200621)
#define DAQmxErrorRepeatedAIPhysicalChan                                       (-200620)
#define DAQmxErrorMultScanOpsInOneChassis                                      (-200619)
#define DAQmxErrorInvalidAIChanOrder                                           (-200618)
#define DAQmxErrorReversePowerProtectionActivated                              (-200617)
#define DAQmxErrorInvalidAsynOpHandle                                          (-200616)
#define DAQmxErrorFailedToEnableHighSpeedOutput                                (-200615)
#define DAQmxErrorCannotReadPastEndOfRecord                                    (-200614)
#define DAQmxErrorAcqStoppedToPreventInputBufferOverwriteOneDataXferMech       (-200613)
#define DAQmxErrorZeroBasedChanIndexInvalid                                    (-200612)
#define DAQmxErrorNoChansOfGivenTypeInTask                                     (-200611)
#define DAQmxErrorSampClkSrcInvalidForOutputValidForInput                      (-200610)
#define DAQmxErrorOutputBufSizeTooSmallToStartGen                              (-200609)
#define DAQmxErrorInputBufSizeTooSmallToStartAcq                               (-200608)
#define DAQmxErrorExportTwoSignalsOnSameTerminal                               (-200607)
#define DAQmxErrorChanIndexInvalid                                             (-200606)
#define DAQmxErrorRangeSyntaxNumberTooBig                                      (-200605)
#define DAQmxErrorNULLPtr                                                      (-200604)
#define DAQmxErrorScaledMinEqualMax                                            (-200603)
#define DAQmxErrorPreScaledMinEqualMax                                         (-200602)
#define DAQmxErrorPropertyNotSupportedForScaleType                             (-200601)
#define DAQmxErrorChannelNameGenerationNumberTooBig                            (-200600)
#define DAQmxErrorRepeatedNumberInScaledValues                                 (-200599)
#define DAQmxErrorRepeatedNumberInPreScaledValues                              (-200598)
#define DAQmxErrorLinesAlreadyReservedForOutput                                (-200597)
#define DAQmxErrorSwitchOperationChansSpanMultipleDevsInList                   (-200596)
#define DAQmxErrorInvalidIDInListAtBeginningOfSwitchOperation                  (-200595)
#define DAQmxErrorMStudioInvalidPolyDirection                                  (-200594)
#define DAQmxErrorMStudioPropertyGetWhileTaskNotVerified                       (-200593)
#define DAQmxErrorRangeWithTooManyObjects                                      (-200592)
#define DAQmxErrorCppDotNetAPINegativeBufferSize                               (-200591)
#define DAQmxErrorCppCantRemoveInvalidEventHandler                             (-200590)
#define DAQmxErrorCppCantRemoveEventHandlerTwice                               (-200589)
#define DAQmxErrorCppCantRemoveOtherObjectsEventHandler                        (-200588)
#define DAQmxErrorDigLinesReservedOrUnavailable                                (-200587)
#define DAQmxErrorDSFFailedToResetStream                                       (-200586)
#define DAQmxErrorDSFReadyForOutputNotAsserted                                 (-200585)
#define DAQmxErrorSampToWritePerChanNotMultipleOfIncr                          (-200584)
#define DAQmxErrorAOPropertiesCauseVoltageBelowMin                             (-200583)
#define DAQmxErrorAOPropertiesCauseVoltageOverMax                              (-200582)
#define DAQmxErrorPropertyNotSupportedWhenRefClkSrcNone                        (-200581)
#define DAQmxErrorAIMaxTooSmall                                                (-200580)
#define DAQmxErrorAIMaxTooLarge                                                (-200579)
#define DAQmxErrorAIMinTooSmall                                                (-200578)
#define DAQmxErrorAIMinTooLarge                                                (-200577)
#define DAQmxErrorBuiltInCJCSrcNotSupported                                    (-200576)
#define DAQmxErrorTooManyPostTrigSampsPerChan                                  (-200575)
#define DAQmxErrorTrigLineNotFoundSingleDevRoute                               (-200574)
#define DAQmxErrorDifferentInternalAIInputSources                              (-200573)
#define DAQmxErrorDifferentAIInputSrcInOneChanGroup                            (-200572)
#define DAQmxErrorInternalAIInputSrcInMultipleChanGroups                       (-200571)
#define DAQmxErrorCAPIChanIndexInvalid                                         (-200570)
#define DAQmxErrorCollectionDoesNotMatchChanType                               (-200569)
#define DAQmxErrorOutputCantStartChangedRegenerationMode                       (-200568)
#define DAQmxErrorOutputCantStartChangedBufferSize                             (-200567)
#define DAQmxErrorChanSizeTooBigForU32PortWrite                                (-200566)
#define DAQmxErrorChanSizeTooBigForU8PortWrite                                 (-200565)
#define DAQmxErrorChanSizeTooBigForU32PortRead                                 (-200564)
#define DAQmxErrorChanSizeTooBigForU8PortRead                                  (-200563)
#define DAQmxErrorInvalidDigDataWrite                                          (-200562)
#define DAQmxErrorInvalidAODataWrite                                           (-200561)
#define DAQmxErrorWaitUntilDoneDoesNotIndicateDone                             (-200560)
#define DAQmxErrorMultiChanTypesInTask                                         (-200559)
#define DAQmxErrorMultiDevsInTask                                              (-200558)
#define DAQmxErrorCannotSetPropertyWhenTaskRunning                             (-200557)
#define DAQmxErrorCannotGetPropertyWhenTaskNotCommittedOrRunning               (-200556)
#define DAQmxErrorLeadingUnderscoreInString                                    (-200555)
#define DAQmxErrorTrailingSpaceInString                                        (-200554)
#define DAQmxErrorLeadingSpaceInString                                         (-200553)
#define DAQmxErrorInvalidCharInString                                          (-200552)
#define DAQmxErrorDLLBecameUnlocked                                            (-200551)
#define DAQmxErrorDLLLock                                                      (-200550)
#define DAQmxErrorSelfCalConstsInvalid                                         (-200549)
#define DAQmxErrorInvalidTrigCouplingExceptForExtTrigChan                      (-200548)
#define DAQmxErrorWriteFailsBufferSizeAutoConfigured                           (-200547)
#define DAQmxErrorExtCalAdjustExtRefVoltageFailed                              (-200546)
#define DAQmxErrorSelfCalFailedExtNoiseOrRefVoltageOutOfCal                    (-200545)
#define DAQmxErrorExtCalTemperatureNotDAQmx                                    (-200544)
#define DAQmxErrorExtCalDateTimeNotDAQmx                                       (-200543)
#define DAQmxErrorSelfCalTemperatureNotDAQmx                                   (-200542)
#define DAQmxErrorSelfCalDateTimeNotDAQmx                                      (-200541)
#define DAQmxErrorDACRefValNotSet                                              (-200540)
#define DAQmxErrorAnalogMultiSampWriteNotSupported                             (-200539)
#define DAQmxErrorInvalidActionInControlTask                                   (-200538)
#define DAQmxErrorPolyCoeffsInconsistent                                       (-200537)
#define DAQmxErrorSensorValTooLow                                              (-200536)
#define DAQmxErrorSensorValTooHigh                                             (-200535)
#define DAQmxErrorWaveformNameTooLong                                          (-200534)
#define DAQmxErrorIdentifierTooLongInScript                                    (-200533)
#define DAQmxErrorUnexpectedIDFollowingSwitchChanName                          (-200532)
#define DAQmxErrorRelayNameNotSpecifiedInList                                  (-200531)
#define DAQmxErrorUnexpectedIDFollowingRelayNameInList                         (-200530)
#define DAQmxErrorUnexpectedIDFollowingSwitchOpInList                          (-200529)
#define DAQmxErrorInvalidLineGrouping                                          (-200528)
#define DAQmxErrorCtrMinMax                                                    (-200527)
#define DAQmxErrorWriteChanTypeMismatch                                        (-200526)
#define DAQmxErrorReadChanTypeMismatch                                         (-200525)
#define DAQmxErrorWriteNumChansMismatch                                        (-200524)
#define DAQmxErrorOneChanReadForMultiChanTask                                  (-200523)
#define DAQmxErrorCannotSelfCalDuringExtCal                                    (-200522)
#define DAQmxErrorMeasCalAdjustOscillatorPhaseDAC                              (-200521)
#define DAQmxErrorInvalidCalConstCalADCAdjustment                              (-200520)
#define DAQmxErrorInvalidCalConstOscillatorFreqDACValue                        (-200519)
#define DAQmxErrorInvalidCalConstOscillatorPhaseDACValue                       (-200518)
#define DAQmxErrorInvalidCalConstOffsetDACValue                                (-200517)
#define DAQmxErrorInvalidCalConstGainDACValue                                  (-200516)
#define DAQmxErrorInvalidNumCalADCReadsToAverage                               (-200515)
#define DAQmxErrorInvalidCfgCalAdjustDirectPathOutputImpedance                 (-200514)
#define DAQmxErrorInvalidCfgCalAdjustMainPathOutputImpedance                   (-200513)
#define DAQmxErrorInvalidCfgCalAdjustMainPathPostAmpGainAndOffset              (-200512)
#define DAQmxErrorInvalidCfgCalAdjustMainPathPreAmpGain                        (-200511)
#define DAQmxErrorInvalidCfgCalAdjustMainPreAmpOffset                          (-200510)
#define DAQmxErrorMeasCalAdjustCalADC                                          (-200509)
#define DAQmxErrorMeasCalAdjustOscillatorFrequency                             (-200508)
#define DAQmxErrorMeasCalAdjustDirectPathOutputImpedance                       (-200507)
#define DAQmxErrorMeasCalAdjustMainPathOutputImpedance                         (-200506)
#define DAQmxErrorMeasCalAdjustDirectPathGain                                  (-200505)
#define DAQmxErrorMeasCalAdjustMainPathPostAmpGainAndOffset                    (-200504)
#define DAQmxErrorMeasCalAdjustMainPathPreAmpGain                              (-200503)
#define DAQmxErrorMeasCalAdjustMainPathPreAmpOffset                            (-200502)
#define DAQmxErrorInvalidDateTimeInEEPROM                                      (-200501)
#define DAQmxErrorUnableToLocateErrorResources                                 (-200500)
#define DAQmxErrorDotNetAPINotUnsigned32BitNumber                              (-200499)
#define DAQmxErrorInvalidRangeOfObjectsSyntaxInString                          (-200498)
#define DAQmxErrorAttemptToEnableLineNotPreviouslyDisabled                     (-200497)
#define DAQmxErrorInvalidCharInPattern                                         (-200496)
#define DAQmxErrorIntermediateBufferFull                                       (-200495)
#define DAQmxErrorLoadTaskFailsBecauseNoTimingOnDev                            (-200494)
#define DAQmxErrorCAPIReservedParamNotNULLNorEmpty                             (-200493)
#define DAQmxErrorCAPIReservedParamNotNULL                                     (-200492)
#define DAQmxErrorCAPIReservedParamNotZero                                     (-200491)
#define DAQmxErrorSampleValueOutOfRange                                        (-200490)
#define DAQmxErrorChanAlreadyInTask                                            (-200489)
#define DAQmxErrorVirtualChanDoesNotExist                                      (-200488)
#define DAQmxErrorChanNotInTask                                                (-200486)
#define DAQmxErrorTaskNotInDataNeighborhood                                    (-200485)
#define DAQmxErrorCantSaveTaskWithoutReplace                                   (-200484)
#define DAQmxErrorCantSaveChanWithoutReplace                                   (-200483)
#define DAQmxErrorDevNotInTask                                                 (-200482)
#define DAQmxErrorDevAlreadyInTask                                             (-200481)
#define DAQmxErrorCanNotPerformOpWhileTaskRunning                              (-200479)
#define DAQmxErrorCanNotPerformOpWhenNoChansInTask                             (-200478)
#define DAQmxErrorCanNotPerformOpWhenNoDevInTask                               (-200477)
#define DAQmxErrorCannotPerformOpWhenTaskNotRunning                            (-200475)
#define DAQmxErrorOperationTimedOut                                            (-200474)
#define DAQmxErrorCannotReadWhenAutoStartFalseAndTaskNotRunningOrCommitted     (-200473)
#define DAQmxErrorCannotWriteWhenAutoStartFalseAndTaskNotRunningOrCommitted    (-200472)
#define DAQmxErrorTaskVersionNew                                               (-200470)
#define DAQmxErrorChanVersionNew                                               (-200469)
#define DAQmxErrorEmptyString                                                  (-200467)
#define DAQmxErrorChannelSizeTooBigForPortReadType                             (-200466)
#define DAQmxErrorChannelSizeTooBigForPortWriteType                            (-200465)
#define DAQmxErrorExpectedNumberOfChannelsVerificationFailed                   (-200464)
#define DAQmxErrorNumLinesMismatchInReadOrWrite                                (-200463)
#define DAQmxErrorOutputBufferEmpty                                            (-200462)
#define DAQmxErrorInvalidChanName                                              (-200461)
#define DAQmxErrorReadNoInputChansInTask                                       (-200460)
#define DAQmxErrorWriteNoOutputChansInTask                                     (-200459)
#define DAQmxErrorPropertyNotSupportedNotInputTask                             (-200457)
#define DAQmxErrorPropertyNotSupportedNotOutputTask                            (-200456)
#define DAQmxErrorGetPropertyNotInputBufferedTask                              (-200455)
#define DAQmxErrorGetPropertyNotOutputBufferedTask                             (-200454)
#define DAQmxErrorInvalidTimeoutVal                                            (-200453)
#define DAQmxErrorAttributeNotSupportedInTaskContext                           (-200452)
#define DAQmxErrorAttributeNotQueryableUnlessTaskIsCommitted                   (-200451)
#define DAQmxErrorAttributeNotSettableWhenTaskIsRunning                        (-200450)
#define DAQmxErrorDACRngLowNotMinusRefValNorZero                               (-200449)
#define DAQmxErrorDACRngHighNotEqualRefVal                                     (-200448)
#define DAQmxErrorUnitsNotFromCustomScale                                      (-200447)
#define DAQmxErrorInvalidVoltageReadingDuringExtCal                            (-200446)
#define DAQmxErrorCalFunctionNotSupported                                      (-200445)
#define DAQmxErrorInvalidPhysicalChanForCal                                    (-200444)
#define DAQmxErrorExtCalNotComplete                                            (-200443)
#define DAQmxErrorCantSyncToExtStimulusFreqDuringCal                           (-200442)
#define DAQmxErrorUnableToDetectExtStimulusFreqDuringCal                       (-200441)
#define DAQmxErrorInvalidCloseAction                                           (-200440)
#define DAQmxErrorExtCalFunctionOutsideExtCalSession                           (-200439)
#define DAQmxErrorInvalidCalArea                                               (-200438)
#define DAQmxErrorExtCalConstsInvalid                                          (-200437)
#define DAQmxErrorStartTrigDelayWithExtSampClk                                 (-200436)
#define DAQmxErrorDelayFromSampClkWithExtConv                                  (-200435)
#define DAQmxErrorFewerThan2PreScaledVals                                      (-200434)
#define DAQmxErrorFewerThan2ScaledValues                                       (-200433)
#define DAQmxErrorPhysChanOutputType                                           (-200432)
#define DAQmxErrorPhysChanMeasType                                             (-200431)
#define DAQmxErrorInvalidPhysChanType                                          (-200430)
#define DAQmxErrorLabVIEWEmptyTaskOrChans                                      (-200429)
#define DAQmxErrorLabVIEWInvalidTaskOrChans                                    (-200428)
#define DAQmxErrorInvalidRefClkRate                                            (-200427)
#define DAQmxErrorInvalidExtTrigImpedance                                      (-200426)
#define DAQmxErrorHystTrigLevelAIMax                                           (-200425)
#define DAQmxErrorLineNumIncompatibleWithVideoSignalFormat                     (-200424)
#define DAQmxErrorTrigWindowAIMinAIMaxCombo                                    (-200423)
#define DAQmxErrorTrigAIMinAIMax                                               (-200422)
#define DAQmxErrorHystTrigLevelAIMin                                           (-200421)
#define DAQmxErrorInvalidSampRateConsiderRIS                                   (-200420)
#define DAQmxErrorInvalidReadPosDuringRIS                                      (-200419)
#define DAQmxErrorImmedTrigDuringRISMode                                       (-200418)
#define DAQmxErrorTDCNotEnabledDuringRISMode                                   (-200417)
#define DAQmxErrorMultiRecWithRIS                                              (-200416)
#define DAQmxErrorInvalidRefClkSrc                                             (-200415)
#define DAQmxErrorInvalidSampClkSrc                                            (-200414)
#define DAQmxErrorInsufficientOnBoardMemForNumRecsAndSamps                     (-200413)
#define DAQmxErrorInvalidAIAttenuation                                         (-200412)
#define DAQmxErrorACCouplingNotAllowedWith50OhmImpedance                       (-200411)
#define DAQmxErrorInvalidRecordNum                                             (-200410)
#define DAQmxErrorZeroSlopeLinearScale                                         (-200409)
#define DAQmxErrorZeroReversePolyScaleCoeffs                                   (-200408)
#define DAQmxErrorZeroForwardPolyScaleCoeffs                                   (-200407)
#define DAQmxErrorNoReversePolyScaleCoeffs                                     (-200406)
#define DAQmxErrorNoForwardPolyScaleCoeffs                                     (-200405)
#define DAQmxErrorNoPolyScaleCoeffs                                            (-200404)
#define DAQmxErrorReversePolyOrderLessThanNumPtsToCompute                      (-200403)
#define DAQmxErrorReversePolyOrderNotPositive                                  (-200402)
#define DAQmxErrorNumPtsToComputeNotPositive                                   (-200401)
#define DAQmxErrorWaveformLengthNotMultipleOfIncr                              (-200400)
#define DAQmxErrorCAPINoExtendedErrorInfoAvailable                             (-200399)
#define DAQmxErrorCVIFunctionNotFoundInDAQmxDLL                                (-200398)
#define DAQmxErrorCVIFailedToLoadDAQmxDLL                                      (-200397)
#define DAQmxErrorNoCommonTrigLineForImmedRoute                                (-200396)
#define DAQmxErrorNoCommonTrigLineForTaskRoute                                 (-200395)
#define DAQmxErrorF64PrptyValNotUnsignedInt                                    (-200394)
#define DAQmxErrorRegisterNotWritable                                          (-200393)
#define DAQmxErrorInvalidOutputVoltageAtSampClkRate                            (-200392)
#define DAQmxErrorStrobePhaseShiftDCMBecameUnlocked                            (-200391)
#define DAQmxErrorDrivePhaseShiftDCMBecameUnlocked                             (-200390)
#define DAQmxErrorClkOutPhaseShiftDCMBecameUnlocked                            (-200389)
#define DAQmxErrorOutputBoardClkDCMBecameUnlocked                              (-200388)
#define DAQmxErrorInputBoardClkDCMBecameUnlocked                               (-200387)
#define DAQmxErrorInternalClkDCMBecameUnlocked                                 (-200386)
#define DAQmxErrorDCMLock                                                      (-200385)
#define DAQmxErrorDataLineReservedForDynamicOutput                             (-200384)
#define DAQmxErrorInvalidRefClkSrcGivenSampClkSrc                              (-200383)
#define DAQmxErrorNoPatternMatcherAvailable                                    (-200382)
#define DAQmxErrorInvalidDelaySampRateBelowPhaseShiftDCMThresh                 (-200381)
#define DAQmxErrorStrainGageCalibration                                        (-200380)
#define DAQmxErrorInvalidExtClockFreqAndDivCombo                               (-200379)
#define DAQmxErrorCustomScaleDoesNotExist                                      (-200378)
#define DAQmxErrorOnlyFrontEndChanOpsDuringScan                                (-200377)
#define DAQmxErrorInvalidOptionForDigitalPortChannel                           (-200376)
#define DAQmxErrorUnsupportedSignalTypeExportSignal                            (-200375)
#define DAQmxErrorInvalidSignalTypeExportSignal                                (-200374)
#define DAQmxErrorUnsupportedTrigTypeSendsSWTrig                               (-200373)
#define DAQmxErrorInvalidTrigTypeSendsSWTrig                                   (-200372)
#define DAQmxErrorRepeatedPhysicalChan                                         (-200371)
#define DAQmxErrorResourcesInUseForRouteInTask                                 (-200370)
#define DAQmxErrorResourcesInUseForRoute                                       (-200369)
#define DAQmxErrorRouteNotSupportedByHW                                        (-200368)
#define DAQmxErrorResourcesInUseForExportSignalPolarity                        (-200367)
#define DAQmxErrorResourcesInUseForInversionInTask                             (-200366)
#define DAQmxErrorResourcesInUseForInversion                                   (-200365)
#define DAQmxErrorExportSignalPolarityNotSupportedByHW                         (-200364)
#define DAQmxErrorInversionNotSupportedByHW                                    (-200363)
#define DAQmxErrorOverloadedChansExistNotRead                                  (-200362)
#define DAQmxErrorInputFIFOOverflow2                                           (-200361)
#define DAQmxErrorCJCChanNotSpecd                                              (-200360)
#define DAQmxErrorCtrExportSignalNotPossible                                   (-200359)
#define DAQmxErrorRefTrigWhenContinuous                                        (-200358)
#define DAQmxErrorIncompatibleSensorOutputAndDeviceInputRanges                 (-200357)
#define DAQmxErrorCustomScaleNameUsed                                          (-200356)
#define DAQmxErrorPropertyValNotSupportedByHW                                  (-200355)
#define DAQmxErrorPropertyValNotValidTermName                                  (-200354)
#define DAQmxErrorResourcesInUseForProperty                                    (-200353)
#define DAQmxErrorCJCChanAlreadyUsed                                           (-200352)
#define DAQmxErrorForwardPolynomialCoefNotSpecd                                (-200351)
#define DAQmxErrorTableScaleNumPreScaledAndScaledValsNotEqual                  (-200350)
#define DAQmxErrorTableScalePreScaledValsNotSpecd                              (-200349)
#define DAQmxErrorTableScaleScaledValsNotSpecd                                 (-200348)
#define DAQmxErrorIntermediateBufferSizeNotMultipleOfIncr                      (-200347)
#define DAQmxErrorEventPulseWidthOutOfRange                                    (-200346)
#define DAQmxErrorEventDelayOutOfRange                                         (-200345)
#define DAQmxErrorSampPerChanNotMultipleOfIncr                                 (-200344)
#define DAQmxErrorCannotCalculateNumSampsTaskNotStarted                        (-200343)
#define DAQmxErrorScriptNotInMem                                               (-200342)
#define DAQmxErrorOnboardMemTooSmall                                           (-200341)
#define DAQmxErrorReadAllAvailableDataWithoutBuffer                            (-200340)
#define DAQmxErrorPulseActiveAtStart                                           (-200339)
#define DAQmxErrorCalTempNotSupported                                          (-200338)
#define DAQmxErrorDelayFromSampClkTooLong                                      (-200337)
#define DAQmxErrorDelayFromSampClkTooShort                                     (-200336)
#define DAQmxErrorAIConvRateTooHigh                                            (-200335)
#define DAQmxErrorDelayFromStartTrigTooLong                                    (-200334)
#define DAQmxErrorDelayFromStartTrigTooShort                                   (-200333)
#define DAQmxErrorSampRateTooHigh                                              (-200332)
#define DAQmxErrorSampRateTooLow                                               (-200331)
#define DAQmxErrorPFI0UsedForAnalogAndDigitalSrc                               (-200330)
#define DAQmxErrorPrimingCfgFIFO                                               (-200329)
#define DAQmxErrorCannotOpenTopologyCfgFile                                    (-200328)
#define DAQmxErrorInvalidDTInsideWfmDataType                                   (-200327)
#define DAQmxErrorRouteSrcAndDestSame                                          (-200326)
#define DAQmxErrorReversePolynomialCoefNotSpecd                                (-200325)
#define DAQmxErrorDevAbsentOrUnavailable                                       (-200324)
#define DAQmxErrorNoAdvTrigForMultiDevScan                                     (-200323)
#define DAQmxErrorInterruptsInsufficientDataXferMech                           (-200322)
#define DAQmxErrorInvalidAttentuationBasedOnMinMax                             (-200321)
#define DAQmxErrorCabledModuleCannotRouteSSH                                   (-200320)
#define DAQmxErrorCabledModuleCannotRouteConvClk                               (-200319)
#define DAQmxErrorInvalidExcitValForScaling                                    (-200318)
#define DAQmxErrorNoDevMemForScript                                            (-200317)
#define DAQmxErrorScriptDataUnderflow                                          (-200316)
#define DAQmxErrorNoDevMemForWaveform                                          (-200315)
#define DAQmxErrorStreamDCMBecameUnlocked                                      (-200314)
#define DAQmxErrorStreamDCMLock                                                (-200313)
#define DAQmxErrorWaveformNotInMem                                             (-200312)
#define DAQmxErrorWaveformWriteOutOfBounds                                     (-200311)
#define DAQmxErrorWaveformPreviouslyAllocated                                  (-200310)
#define DAQmxErrorSampClkTbMasterTbDivNotAppropriateForSampTbSrc               (-200309)
#define DAQmxErrorSampTbRateSampTbSrcMismatch                                  (-200308)
#define DAQmxErrorMasterTbRateMasterTbSrcMismatch                              (-200307)
#define DAQmxErrorSampsPerChanTooBig                                           (-200306)
#define DAQmxErrorFinitePulseTrainNotPossible                                  (-200305)
#define DAQmxErrorExtMasterTimebaseRateNotSpecified                            (-200304)
#define DAQmxErrorExtSampClkSrcNotSpecified                                    (-200303)
#define DAQmxErrorInputSignalSlowerThanMeasTime                                (-200302)
#define DAQmxErrorCannotUpdatePulseGenProperty                                 (-200301)
#define DAQmxErrorInvalidTimingType                                            (-200300)
#define DAQmxErrorPropertyUnavailWhenUsingOnboardMemory                        (-200297)
#define DAQmxErrorCannotWriteAfterStartWithOnboardMemory                       (-200295)
#define DAQmxErrorNotEnoughSampsWrittenForInitialXferRqstCondition             (-200294)
#define DAQmxErrorNoMoreSpace                                                  (-200293)
#define DAQmxErrorSamplesCanNotYetBeWritten                                    (-200292)
#define DAQmxErrorGenStoppedToPreventIntermediateBufferRegenOfOldSamples       (-200291)
#define DAQmxErrorGenStoppedToPreventRegenOfOldSamples                         (-200290)
#define DAQmxErrorSamplesNoLongerWriteable                                     (-200289)
#define DAQmxErrorSamplesWillNeverBeGenerated                                  (-200288)
#define DAQmxErrorNegativeWriteSampleNumber                                    (-200287)
#define DAQmxErrorNoAcqStarted                                                 (-200286)
#define DAQmxErrorSamplesNotYetAvailable                                       (-200284)
#define DAQmxErrorAcqStoppedToPreventIntermediateBufferOverflow                (-200283)
#define DAQmxErrorNoRefTrigConfigured                                          (-200282)
#define DAQmxErrorCannotReadRelativeToRefTrigUntilDone                         (-200281)
#define DAQmxErrorSamplesNoLongerAvailable                                     (-200279)
#define DAQmxErrorSamplesWillNeverBeAvailable                                  (-200278)
#define DAQmxErrorNegativeReadSampleNumber                                     (-200277)
#define DAQmxErrorExternalSampClkAndRefClkThruSameTerm                         (-200276)
#define DAQmxErrorExtSampClkRateTooLowForClkIn                                 (-200275)
#define DAQmxErrorExtSampClkRateTooHighForBackplane                            (-200274)
#define DAQmxErrorSampClkRateAndDivCombo                                       (-200273)
#define DAQmxErrorSampClkRateTooLowForDivDown                                  (-200272)
#define DAQmxErrorProductOfAOMinAndGainTooSmall                                (-200271)
#define DAQmxErrorInterpolationRateNotPossible                                 (-200270)
#define DAQmxErrorOffsetTooLarge                                               (-200269)
#define DAQmxErrorOffsetTooSmall                                               (-200268)
#define DAQmxErrorProductOfAOMaxAndGainTooLarge                                (-200267)
#define DAQmxErrorMinAndMaxNotSymmetric                                        (-200266)
#define DAQmxErrorInvalidAnalogTrigSrc                                         (-200265)
#define DAQmxErrorTooManyChansForAnalogRefTrig                                 (-200264)
#define DAQmxErrorTooManyChansForAnalogPauseTrig                               (-200263)
#define DAQmxErrorTrigWhenOnDemandSampTiming                                   (-200262)
#define DAQmxErrorInconsistentAnalogTrigSettings                               (-200261)
#define DAQmxErrorMemMapDataXferModeSampTimingCombo                            (-200260)
#define DAQmxErrorInvalidJumperedAttr                                          (-200259)
#define DAQmxErrorInvalidGainBasedOnMinMax                                     (-200258)
#define DAQmxErrorInconsistentExcit                                            (-200257)
#define DAQmxErrorTopologyNotSupportedByCfgTermBlock                           (-200256)
#define DAQmxErrorBuiltInTempSensorNotSupported                                (-200255)
#define DAQmxErrorInvalidTerm                                                  (-200254)
#define DAQmxErrorCannotTristateTerm                                           (-200253)
#define DAQmxErrorCannotTristateBusyTerm                                       (-200252)
#define DAQmxErrorNoDMAChansAvailable                                          (-200251)
#define DAQmxErrorInvalidWaveformLengthWithinLoopInScript                      (-200250)
#define DAQmxErrorInvalidSubsetLengthWithinLoopInScript                        (-200249)
#define DAQmxErrorMarkerPosInvalidForLoopInScript                              (-200248)
#define DAQmxErrorIntegerExpectedInScript                                      (-200247)
#define DAQmxErrorPLLBecameUnlocked                                            (-200246)
#define DAQmxErrorPLLLock                                                      (-200245)
#define DAQmxErrorDDCClkOutDCMBecameUnlocked                                   (-200244)
#define DAQmxErrorDDCClkOutDCMLock                                             (-200243)
#define DAQmxErrorClkDoublerDCMBecameUnlocked                                  (-200242)
#define DAQmxErrorClkDoublerDCMLock                                            (-200241)
#define DAQmxErrorSampClkDCMBecameUnlocked                                     (-200240)
#define DAQmxErrorSampClkDCMLock                                               (-200239)
#define DAQmxErrorSampClkTimebaseDCMBecameUnlocked                             (-200238)
#define DAQmxErrorSampClkTimebaseDCMLock                                       (-200237)
#define DAQmxErrorAttrCannotBeReset                                            (-200236)
#define DAQmxErrorExplanationNotFound                                          (-200235)
#define DAQmxErrorWriteBufferTooSmall                                          (-200234)
#define DAQmxErrorSpecifiedAttrNotValid                                        (-200233)
#define DAQmxErrorAttrCannotBeRead                                             (-200232)
#define DAQmxErrorAttrCannotBeSet                                              (-200231)
#define DAQmxErrorNULLPtrForC_Api                                              (-200230)
#define DAQmxErrorReadBufferTooSmall                                           (-200229)
#define DAQmxErrorBufferTooSmallForString                                      (-200228)
#define DAQmxErrorNoAvailTrigLinesOnDevice                                     (-200227)
#define DAQmxErrorTrigBusLineNotAvail                                          (-200226)
#define DAQmxErrorCouldNotReserveRequestedTrigLine                             (-200225)
#define DAQmxErrorTrigLineNotFound                                             (-200224)
#define DAQmxErrorSCXI1126ThreshHystCombination                                (-200223)
#define DAQmxErrorAcqStoppedToPreventInputBufferOverwrite                      (-200222)
#define DAQmxErrorTimeoutExceeded                                              (-200221)
#define DAQmxErrorInvalidDeviceID                                              (-200220)
#define DAQmxErrorInvalidAOChanOrder                                           (-200219)
#define DAQmxErrorSampleTimingTypeAndDataXferMode                              (-200218)
#define DAQmxErrorBufferWithOnDemandSampTiming                                 (-200217)
#define DAQmxErrorBufferAndDataXferMode                                        (-200216)
#define DAQmxErrorMemMapAndBuffer                                              (-200215)
#define DAQmxErrorNoAnalogTrigHW                                               (-200214)
#define DAQmxErrorTooManyPretrigPlusMinPostTrigSamps                           (-200213)
#define DAQmxErrorInconsistentUnitsSpecified                                   (-200212)
#define DAQmxErrorMultipleRelaysForSingleRelayOp                               (-200211)
#define DAQmxErrorMultipleDevIDsPerChassisSpecifiedInList                      (-200210)
#define DAQmxErrorDuplicateDevIDInList                                         (-200209)
#define DAQmxErrorInvalidRangeStatementCharInList                              (-200208)
#define DAQmxErrorInvalidDeviceIDInList                                        (-200207)
#define DAQmxErrorTriggerPolarityConflict                                      (-200206)
#define DAQmxErrorCannotScanWithCurrentTopology                                (-200205)
#define DAQmxErrorUnexpectedIdentifierInFullySpecifiedPathInList               (-200204)
#define DAQmxErrorSwitchCannotDriveMultipleTrigLines                           (-200203)
#define DAQmxErrorInvalidRelayName                                             (-200202)
#define DAQmxErrorSwitchScanlistTooBig                                         (-200201)
#define DAQmxErrorSwitchChanInUse                                              (-200200)
#define DAQmxErrorSwitchNotResetBeforeScan                                     (-200199)
#define DAQmxErrorInvalidTopology                                              (-200198)
#define DAQmxErrorAttrNotSupported                                             (-200197)
#define DAQmxErrorUnexpectedEndOfActionsInList                                 (-200196)
#define DAQmxErrorPowerBudgetExceeded                                          (-200195)
#define DAQmxErrorHWUnexpectedlyPoweredOffAndOn                                (-200194)
#define DAQmxErrorSwitchOperationNotSupported                                  (-200193)
#define DAQmxErrorOnlyContinuousScanSupported                                  (-200192)
#define DAQmxErrorSwitchDifferentTopologyWhenScanning                          (-200191)
#define DAQmxErrorDisconnectPathNotSameAsExistingPath                          (-200190)
#define DAQmxErrorConnectionNotPermittedOnChanReservedForRouting               (-200189)
#define DAQmxErrorCannotConnectSrcChans                                        (-200188)
#define DAQmxErrorCannotConnectChannelToItself                                 (-200187)
#define DAQmxErrorChannelNotReservedForRouting                                 (-200186)
#define DAQmxErrorCannotConnectChansDirectly                                   (-200185)
#define DAQmxErrorChansAlreadyConnected                                        (-200184)
#define DAQmxErrorChanDuplicatedInPath                                         (-200183)
#define DAQmxErrorNoPathToDisconnect                                           (-200182)
#define DAQmxErrorInvalidSwitchChan                                            (-200181)
#define DAQmxErrorNoPathAvailableBetween2SwitchChans                           (-200180)
#define DAQmxErrorExplicitConnectionExists                                     (-200179)
#define DAQmxErrorSwitchDifferentSettlingTimeWhenScanning                      (-200178)
#define DAQmxErrorOperationOnlyPermittedWhileScanning                          (-200177)
#define DAQmxErrorOperationNotPermittedWhileScanning                           (-200176)
#define DAQmxErrorHardwareNotResponding                                        (-200175)
#define DAQmxErrorInvalidSampAndMasterTimebaseRateCombo                        (-200173)
#define DAQmxErrorNonZeroBufferSizeInProgIOXfer                                (-200172)
#define DAQmxErrorVirtualChanNameUsed                                          (-200171)
#define DAQmxErrorPhysicalChanDoesNotExist                                     (-200170)
#define DAQmxErrorMemMapOnlyForProgIOXfer                                      (-200169)
#define DAQmxErrorTooManyChans                                                 (-200168)
#define DAQmxErrorCannotHaveCJTempWithOtherChans                               (-200167)
#define DAQmxErrorOutputBufferUnderwrite                                       (-200166)
#define DAQmxErrorSensorInvalidCompletionResistance                            (-200163)
#define DAQmxErrorVoltageExcitIncompatibleWith2WireCfg                         (-200162)
#define DAQmxErrorIntExcitSrcNotAvailable                                      (-200161)
#define DAQmxErrorCannotCreateChannelAfterTaskVerified                         (-200160)
#define DAQmxErrorLinesReservedForSCXIControl                                  (-200159)
#define DAQmxErrorCouldNotReserveLinesForSCXIControl                           (-200158)
#define DAQmxErrorCalibrationFailed                                            (-200157)
#define DAQmxErrorReferenceFrequencyInvalid                                    (-200156)
#define DAQmxErrorReferenceResistanceInvalid                                   (-200155)
#define DAQmxErrorReferenceCurrentInvalid                                      (-200154)
#define DAQmxErrorReferenceVoltageInvalid                                      (-200153)
#define DAQmxErrorEEPROMDataInvalid                                            (-200152)
#define DAQmxErrorCabledModuleNotCapableOfRoutingAI                            (-200151)
#define DAQmxErrorChannelNotAvailableInParallelMode                            (-200150)
#define DAQmxErrorExternalTimebaseRateNotKnownForDelay                         (-200149)
#define DAQmxErrorFREQOUTCannotProduceDesiredFrequency                         (-200148)
#define DAQmxErrorMultipleCounterInputTask                                     (-200147)
#define DAQmxErrorCounterStartPauseTriggerConflict                             (-200146)
#define DAQmxErrorCounterInputPauseTriggerAndSampleClockInvalid                (-200145)
#define DAQmxErrorCounterOutputPauseTriggerInvalid                             (-200144)
#define DAQmxErrorCounterTimebaseRateNotSpecified                              (-200143)
#define DAQmxErrorCounterTimebaseRateNotFound                                  (-200142)
#define DAQmxErrorCounterOverflow                                              (-200141)
#define DAQmxErrorCounterNoTimebaseEdgesBetweenGates                           (-200140)
#define DAQmxErrorCounterMaxMinRangeFreq                                       (-200139)
#define DAQmxErrorCounterMaxMinRangeTime                                       (-200138)
#define DAQmxErrorSuitableTimebaseNotFoundTimeCombo                            (-200137)
#define DAQmxErrorSuitableTimebaseNotFoundFrequencyCombo                       (-200136)
#define DAQmxErrorInternalTimebaseSourceDivisorCombo                           (-200135)
#define DAQmxErrorInternalTimebaseSourceRateCombo                              (-200134)
#define DAQmxErrorInternalTimebaseRateDivisorSourceCombo                       (-200133)
#define DAQmxErrorExternalTimebaseRateNotknownForRate                          (-200132)
#define DAQmxErrorAnalogTrigChanNotFirstInScanList                             (-200131)
#define DAQmxErrorNoDivisorForExternalSignal                                   (-200130)
#define DAQmxErrorAttributeInconsistentAcrossRepeatedPhysicalChannels          (-200128)
#define DAQmxErrorCannotHandshakeWithPort0                                     (-200127)
#define DAQmxErrorControlLineConflictOnPortC                                   (-200126)
#define DAQmxErrorLines4To7ConfiguredForOutput                                 (-200125)
#define DAQmxErrorLines4To7ConfiguredForInput                                  (-200124)
#define DAQmxErrorLines0To3ConfiguredForOutput                                 (-200123)
#define DAQmxErrorLines0To3ConfiguredForInput                                  (-200122)
#define DAQmxErrorPortConfiguredForOutput                                      (-200121)
#define DAQmxErrorPortConfiguredForInput                                       (-200120)
#define DAQmxErrorPortConfiguredForStaticDigitalOps                            (-200119)
#define DAQmxErrorPortReservedForHandshaking                                   (-200118)
#define DAQmxErrorPortDoesNotSupportHandshakingDataIO                          (-200117)
#define DAQmxErrorCannotTristate8255OutputLines                                (-200116)
#define DAQmxErrorTemperatureOutOfRangeForCalibration                          (-200113)
#define DAQmxErrorCalibrationHandleInvalid                                     (-200112)
#define DAQmxErrorPasswordRequired                                             (-200111)
#define DAQmxErrorIncorrectPassword                                            (-200110)
#define DAQmxErrorPasswordTooLong                                              (-200109)
#define DAQmxErrorCalibrationSessionAlreadyOpen                                (-200108)
#define DAQmxErrorSCXIModuleIncorrect                                          (-200107)
#define DAQmxErrorAttributeInconsistentAcrossChannelsOnDevice                  (-200106)
#define DAQmxErrorSCXI1122ResistanceChanNotSupportedForCfg                     (-200105)
#define DAQmxErrorBracketPairingMismatchInList                                 (-200104)
#define DAQmxErrorInconsistentNumSamplesToWrite                                (-200103)
#define DAQmxErrorIncorrectDigitalPattern                                      (-200102)
#define DAQmxErrorIncorrectNumChannelsToWrite                                  (-200101)
#define DAQmxErrorIncorrectReadFunction                                        (-200100)
#define DAQmxErrorPhysicalChannelNotSpecified                                  (-200099)
#define DAQmxErrorMoreThanOneTerminal                                          (-200098)
#define DAQmxErrorMoreThanOneActiveChannelSpecified                            (-200097)
#define DAQmxErrorInvalidNumberSamplesToRead                                   (-200096)
#define DAQmxErrorAnalogWaveformExpected                                       (-200095)
#define DAQmxErrorDigitalWaveformExpected                                      (-200094)
#define DAQmxErrorActiveChannelNotSpecified                                    (-200093)
#define DAQmxErrorFunctionNotSupportedForDeviceTasks                           (-200092)
#define DAQmxErrorFunctionNotInLibrary                                         (-200091)
#define DAQmxErrorLibraryNotPresent                                            (-200090)
#define DAQmxErrorDuplicateTask                                                (-200089)
#define DAQmxErrorInvalidTask                                                  (-200088)
#define DAQmxErrorInvalidChannel                                               (-200087)
#define DAQmxErrorInvalidSyntaxForPhysicalChannelRange                         (-200086)
#define DAQmxErrorMinNotLessThanMax                                            (-200082)
#define DAQmxErrorSampleRateNumChansConvertPeriodCombo                         (-200081)
#define DAQmxErrorAODuringCounter1DMAConflict                                  (-200079)
#define DAQmxErrorAIDuringCounter0DMAConflict                                  (-200078)
#define DAQmxErrorInvalidAttributeValue                                        (-200077)
#define DAQmxErrorSuppliedCurrentDataOutsideSpecifiedRange                     (-200076)
#define DAQmxErrorSuppliedVoltageDataOutsideSpecifiedRange                     (-200075)
#define DAQmxErrorCannotStoreCalConst                                          (-200074)
#define DAQmxErrorSCXIModuleNotFound                                           (-200073)
#define DAQmxErrorDuplicatePhysicalChansNotSupported                           (-200072)
#define DAQmxErrorTooManyPhysicalChansInList                                   (-200071)
#define DAQmxErrorInvalidAdvanceEventTriggerType                               (-200070)
#define DAQmxErrorDeviceIsNotAValidSwitch                                      (-200069)
#define DAQmxErrorDeviceDoesNotSupportScanning                                 (-200068)
#define DAQmxErrorScanListCannotBeTimed                                        (-200067)
#define DAQmxErrorConnectOperatorInvalidAtPointInList                          (-200066)
#define DAQmxErrorUnexpectedSwitchActionInList                                 (-200065)
#define DAQmxErrorUnexpectedSeparatorInList                                    (-200064)
#define DAQmxErrorExpectedTerminatorInList                                     (-200063)
#define DAQmxErrorExpectedConnectOperatorInList                                (-200062)
#define DAQmxErrorExpectedSeparatorInList                                      (-200061)
#define DAQmxErrorFullySpecifiedPathInListContainsRange                        (-200060)
#define DAQmxErrorConnectionSeparatorAtEndOfList                               (-200059)
#define DAQmxErrorIdentifierInListTooLong                                      (-200058)
#define DAQmxErrorDuplicateDeviceIDInListWhenSettling                          (-200057)
#define DAQmxErrorChannelNameNotSpecifiedInList                                (-200056)
#define DAQmxErrorDeviceIDNotSpecifiedInList                                   (-200055)
#define DAQmxErrorSemicolonDoesNotFollowRangeInList                            (-200054)
#define DAQmxErrorSwitchActionInListSpansMultipleDevices                       (-200053)
#define DAQmxErrorRangeWithoutAConnectActionInList                             (-200052)
#define DAQmxErrorInvalidIdentifierFollowingSeparatorInList                    (-200051)
#define DAQmxErrorInvalidChannelNameInList                                     (-200050)
#define DAQmxErrorInvalidNumberInRepeatStatementInList                         (-200049)
#define DAQmxErrorInvalidTriggerLineInList                                     (-200048)
#define DAQmxErrorInvalidIdentifierInListFollowingDeviceID                     (-200047)
#define DAQmxErrorInvalidIdentifierInListAtEndOfSwitchAction                   (-200046)
#define DAQmxErrorDeviceRemoved                                                (-200045)
#define DAQmxErrorRoutingPathNotAvailable                                      (-200044)
#define DAQmxErrorRoutingHardwareBusy                                          (-200043)
#define DAQmxErrorRequestedSignalInversionForRoutingNotPossible                (-200042)
#define DAQmxErrorInvalidRoutingDestinationTerminalName                        (-200041)
#define DAQmxErrorInvalidRoutingSourceTerminalName                             (-200040)
#define DAQmxErrorRoutingNotSupportedForDevice                                 (-200039)
#define DAQmxErrorWaitIsLastInstructionOfLoopInScript                          (-200038)
#define DAQmxErrorClearIsLastInstructionOfLoopInScript                         (-200037)
#define DAQmxErrorInvalidLoopIterationsInScript                                (-200036)
#define DAQmxErrorRepeatLoopNestingTooDeepInScript                             (-200035)
#define DAQmxErrorMarkerPositionOutsideSubsetInScript                          (-200034)
#define DAQmxErrorSubsetStartOffsetNotAlignedInScript                          (-200033)
#define DAQmxErrorInvalidSubsetLengthInScript                                  (-200032)
#define DAQmxErrorMarkerPositionNotAlignedInScript                             (-200031)
#define DAQmxErrorSubsetOutsideWaveformInScript                                (-200030)
#define DAQmxErrorMarkerOutsideWaveformInScript                                (-200029)
#define DAQmxErrorWaveformInScriptNotInMem                                     (-200028)
#define DAQmxErrorKeywordExpectedInScript                                      (-200027)
#define DAQmxErrorBufferNameExpectedInScript                                   (-200026)
#define DAQmxErrorProcedureNameExpectedInScript                                (-200025)
#define DAQmxErrorScriptHasInvalidIdentifier                                   (-200024)
#define DAQmxErrorScriptHasInvalidCharacter                                    (-200023)
#define DAQmxErrorResourceAlreadyReserved                                      (-200022)
#define DAQmxErrorSelfTestFailed                                               (-200020)
#define DAQmxErrorADCOverrun                                                   (-200019)
#define DAQmxErrorDACUnderflow                                                 (-200018)
#define DAQmxErrorInputFIFOUnderflow                                           (-200017)
#define DAQmxErrorOutputFIFOUnderflow                                          (-200016)
#define DAQmxErrorSCXISerialCommunication                                      (-200015)
#define DAQmxErrorDigitalTerminalSpecifiedMoreThanOnce                         (-200014)
#define DAQmxErrorDigitalOutputNotSupported                                    (-200012)
#define DAQmxErrorInconsistentChannelDirections                                (-200011)
#define DAQmxErrorInputFIFOOverflow                                            (-200010)
#define DAQmxErrorTimeStampOverwritten                                         (-200009)
#define DAQmxErrorStopTriggerHasNotOccurred                                    (-200008)
#define DAQmxErrorRecordNotAvailable                                           (-200007)
#define DAQmxErrorRecordOverwritten                                            (-200006)
#define DAQmxErrorDataNotAvailable                                             (-200005)
#define DAQmxErrorDataOverwrittenInDeviceMemory                                (-200004)
#define DAQmxErrorDuplicatedChannel                                            (-200003)
#define DAQmxWarningTimestampCounterRolledOver                                  (200003)
#define DAQmxWarningInputTerminationOverloaded                                  (200004)
#define DAQmxWarningADCOverloaded                                               (200005)
#define DAQmxWarningPLLUnlocked                                                 (200007)
#define DAQmxWarningCounter0DMADuringAIConflict                                 (200008)
#define DAQmxWarningCounter1DMADuringAOConflict                                 (200009)
#define DAQmxWarningStoppedBeforeDone                                           (200010)
#define DAQmxWarningRateViolatesSettlingTime                                    (200011)
#define DAQmxWarningRateViolatesMaxADCRate                                      (200012)
#define DAQmxWarningUserDefInfoStringTooLong                                    (200013)
#define DAQmxWarningTooManyInterruptsPerSecond                                  (200014)
#define DAQmxWarningPotentialGlitchDuringWrite                                  (200015)
#define DAQmxWarningDevNotSelfCalibratedWithDAQmx                               (200016)
#define DAQmxWarningAISampRateTooLow                                            (200017)
#define DAQmxWarningAIConvRateTooLow                                            (200018)
#define DAQmxWarningReadOffsetCoercion                                          (200019)
#define DAQmxWarningPretrigCoercion                                             (200020)
#define DAQmxWarningSampValCoercedToMax                                         (200021)
#define DAQmxWarningSampValCoercedToMin                                         (200022)
#define DAQmxWarningPropertyVersionNew                                          (200024)
#define DAQmxWarningUserDefinedInfoTooLong                                      (200025)
#define DAQmxWarningCAPIStringTruncatedToFitBuffer                              (200026)
#define DAQmxWarningSampClkRateTooLow                                           (200027)
#define DAQmxWarningPossiblyInvalidCTRSampsInFiniteDMAAcq                       (200028)
#define DAQmxWarningRISAcqCompletedSomeBinsNotFilled                            (200029)
#define DAQmxWarningPXIDevTempExceedsMaxOpTemp                                  (200030)
#define DAQmxWarningOutputGainTooLowForRFFreq                                   (200031)
#define DAQmxWarningOutputGainTooHighForRFFreq                                  (200032)
#define DAQmxWarningMultipleWritesBetweenSampClks                               (200033)
#define DAQmxWarningDeviceMayShutDownDueToHighTemp                              (200034)
#define DAQmxWarningReadNotCompleteBeforeSampClk                                (209800)
#define DAQmxWarningWriteNotCompleteBeforeSampClk                               (209801)


#ifdef __cplusplus
   }
#endif

#endif // __nidaqmxbase_h__
