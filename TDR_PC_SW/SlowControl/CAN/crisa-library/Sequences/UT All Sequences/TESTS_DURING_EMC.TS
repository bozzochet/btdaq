   1:REQUEST MESSAGE               :GEP-05, PTM NOminal, NONE
   2:PTM  ACQUIRE                  :CAN-06, 1, 0
   3:REQUEST MESSAGE               :GEP-05, PTM Redundant, NONE
   4:PTM  ACQUIRE                  :CAN-06, 1, 1
   5:REQUEST MESSAGE               :GEP-05, Cold valve status N, NONE
   6:SEND COMMAND                  :CAN-02, C24, STM_ACQUIRE_COLD_VALVE_STATUS_NOM.CMM, 0
   7:SEND COMMAND                  :CAN-02, C27, STM_READ_COLD_VALVE_STATUS_NOM.CMM, 0
   8:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   9:REQUEST MESSAGE               :GEP-05, Cold valve status R, NONE
  10:SEND COMMAND                  :CAN-02, C24, STM_ACQUIRE_COLD_VALVE_STATUS_RED.CMM, 0
  11:SEND COMMAND                  :CAN-02, C27, STM_READ_COLD_VALVE_STATUS_RED.CMM, 0
  12:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  13:REQUEST MESSAGE               :GEP-05, STM NOminal, NONE
  14:SEND COMMAND                  :CAN-02, C21, STM_ACQUIRE_NOM_ENER_ENA.CMM, 0
  15:SEND COMMAND                  :CAN-02, C22, STM_READ_NOM.CMM, 0
  16:REQUEST MESSAGE               :GEP-05, STM Redundant, NONE
  17:SEND COMMAND                  :CAN-02, C21, STM_ACQUIRE_RED_ENER_ENA.CMM, 0
  18:SEND COMMAND                  :CAN-02, C22, STM_READ_RED.CMM, 0
  19:REQUEST MESSAGE               :GEP-05, CCS TM, NONE
  20:SEND COMMAND                  :CAN-02, C47, CCS_ANALOG_TM_ACQUIRE.CMM, 0
  21:SEND COMMAND                  :CAN-02, C48, CCS_READ_ANALOG_TM.CMM, 0
  22:REQUEST MESSAGE               :GEP-05, This is a request message, NONE
  23:REQUEST MESSAGE               :GEP-05, DV55 from PS NOminal, NONE
  24:SEND COMMAND                  :CAN-02, C78, PS_ENA_DIS_PILOT_VALVES_NOM_ENA.CMM, 0
  25:SEND COMMAND                  :CAN-02, C7B, PS_OPEN_CLOSE_PILOT_VALVE_NOM_V55_OPEN.CMM, 0
  26:REQUEST MESSAGE               :GEP-05, Measure DV55 Voltage, NONE
  27:REQUEST MESSAGE               :GEP-05, CLOSE DV55 from PS NOminal, NONE
  28:SEND COMMAND                  :CAN-02, C7B, PS_OPEN_CLOSE_PILOT_VALVE_NOM_V55_CLOSE.CMM, 0
  29:SEND COMMAND                  :CAN-02, C78, PS_ENA_DIS_PILOT_VALVES_NOM_DIS.CMM, 0
  30:REQUEST MESSAGE               :GEP-05, Measure DV55 Voltage, NONE
  31:REQUEST MESSAGE               :GEP-05, DV55 from PS Redundant, NONE
  32:SEND COMMAND                  :CAN-02, C78, PS_ENA_DIS_PILOT_VALVES_RED_ENA.CMM, 0
  33:SEND COMMAND                  :CAN-02, C7B, PS_OPEN_CLOSE_PILOT_VALVE_RED_V55_OPEN.CMM, 0
  34:REQUEST MESSAGE               :GEP-05, Measure DV55 Voltage, NONE
  35:REQUEST MESSAGE               :GEP-05, CLOSE DV55 from PS Redundant, NONE
  36:SEND COMMAND                  :CAN-02, C7B, PS_OPEN_CLOSE_PILOT_VALVE_RED_V55_CLOSE.CMM, 0
  37:SEND COMMAND                  :CAN-02, C78, PS_ENA_DIS_PILOT_VALVES_RED_DIS.CMM, 0
  38:REQUEST MESSAGE               :GEP-05, Measure DV55 Voltage, NONE
  39:REQUEST MESSAGE               :GEP-05, PS N Warm valves status, NONE
  40:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
  41:REQUEST MESSAGE               :GEP-05, PS R Warm valves status, NONE
  42:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
  43:REQUEST MESSAGE               :GEP-05, DV64 from CSP N, NONE
  44:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_NOM_ARM_OPEN_V064.CMM, 0
  45:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_NOM.CMM, 0
  46:REQUEST MESSAGE               :GEP-05, Measure DV64 Voltage, NONE
  47:REQUEST MESSAGE               :GEP-05, Close DV64 from CSP N, NONE
  48:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_NOM_ARM_CLOSE_V064.CMM, 0
  49:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_NOM.CMM, 0
  50:REQUEST MESSAGE               :GEP-05, Measure DV64 Voltage, NONE
  51:REQUEST MESSAGE               :GEP-05, DV64 from CSP R, NONE
  52:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_RED_ARM_OPEN_V064.CMM, 0
  53:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_RED.CMM, 0
  54:REQUEST MESSAGE               :GEP-05, Measure DV64 Voltage, NONE
  55:REQUEST MESSAGE               :GEP-05, Close DV64 from CSP R, NONE
  56:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_RED_ARM_CLOSE_V064.CMM, 0
  57:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_RED.CMM, 0
  58:REQUEST MESSAGE               :GEP-05, Measure DV64 Voltage, NONE
  59:REQUEST MESSAGE               :GEP-05, CSP N Valves status, NONE
  60:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_NOM.CMM, 0
  61:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  62:REQUEST MESSAGE               :GEP-05, CSP R Valves status, NONE
  63:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_RED.CMM, 0
  64:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  65:REQUEST MESSAGE               :GEP-05, CSP N CL temperatures Detector status, NONE
  66:SEND COMMAND                  :CAN-02, CC6, CSP_GET_DETECTOR_STATUS_NOM.CMM, 0
  67:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  68:REQUEST MESSAGE               :GEP-05, CSP R CL temperatures Detector status, NONE
  69:SEND COMMAND                  :CAN-02, CC6, CSP_GET_DETECTOR_STATUS_RED.CMM, 0
  70:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE