   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
   2:REQUEST MESSAGE               :GEP-05, This is a request message, NONE
   3:SEND COMMAND                  :CAN-02, C21, STM_ACQUIRE_NOM_ENER_DIS.CMM, 0
   4:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 250, mSEC
   5:SEND COMMAND                  :CAN-02, C22, STM_READ_NOM.CMM, 0
   6:REQUEST MESSAGE               :GEP-05, This is a request message, NONE
   7:SEND COMMAND                  :CAN-02, CC6, CSP_GET_DETECTOR_STATUS_NOM.CMM, 0 
   8:REQUEST MESSAGE               :GEP-05, This is a request message, NONE
   9:SEND COMMAND                  :CAN-02, CBE, CSP_RESET_ALARM_AQS.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, This is a request message, NONE
  11:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  12:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  13:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  14:REQUEST MESSAGE               :GEP-05, This is a request message, NONE
  15:SEND COMMAND                  :CAN-02, C21, STM_ACQUIRE_RED_ENER_DIS.CMM, 0
  16:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 250, mSEC
  17:SEND COMMAND                  :CAN-02, C22, STM_READ_RED.CMM, 0
  18:REQUEST MESSAGE               :GEP-05, This is a request message, NONE
  19:SEND COMMAND                  :CAN-02, CC6, CSP_GET_DETECTOR_STATUS_RED.CMM, 0
  20:REQUEST MESSAGE               :GEP-05, This is a request message, NONE
  21:SEND COMMAND                  :CAN-02, CBE, CSP_RESET_ALARM_AQS.CMM, 0
  22:REQUEST MESSAGE               :GEP-05, This is a request message, NONE
  23:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  24:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  25:SEND COMMAND                  :CAN-02, C1E, CAB_READ_AND_RESET_ERROR_INFO.CMM, 0
  26:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  27:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE