   1:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 15, SEC
   2:SEND COMMAND                  :CAN-02, CC6, CSP_GET_DETECTOR_STATUS_NOM.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   4:SEND COMMAND                  :CAN-02, CC3, CSP_GET_ALARM_SUMMARY_NOM.CMM, 0
   5:REQUEST MESSAGE               :GEP-05, Abrir switches, NONE
   8:SEND COMMAND                  :CAN-02, CBE, CSP_RESET_ALARM_AQS.CMM, 0
   9:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  10:SEND COMMAND                  :CAN-02, CC3, CSP_GET_ALARM_SUMMARY_NOM.CMM, 0
  11:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE

  12:SEND COMMAND                  :CAN-02, CE1, CSP_ARM_DISARM_HEATER_NOM_ARM_OFF_CD.CMM, 0
  13:SEND COMMAND                  :CAN-02, CE2, CSP_OPERATE_HEATER_NOM.CMM, 0
  14:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  15:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  16:SEND COMMAND                  :CAN-02, CE1, CSP_ARM_DISARM_HEATER_RED_ARM_OFF_CD.CMM, 0
  17:SEND COMMAND                  :CAN-02, CE2, CSP_OPERATE_HEATER_RED.CMM, 0
  18:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  19:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE

  20:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_NOM_ARM_CLOSE_V064.CMM, 0
  23:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_NOM.CMM, 0
  47:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_NOM.CMM, 0
  22:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE 

  24:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_RED_ARM_CLOSE_V064.CMM, 0
  27:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_RED.CMM, 0
  47:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_RED.CMM, 0
  26:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE