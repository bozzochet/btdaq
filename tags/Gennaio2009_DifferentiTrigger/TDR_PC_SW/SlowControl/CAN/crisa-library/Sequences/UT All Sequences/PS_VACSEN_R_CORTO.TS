   1:REQUEST MESSAGE               :GEP-05, Line switch only, NONE
   2:SEND COMMAND                  :CAN-02, C71, PS_ENA_DIS_HT_HEATER_GROUP_RED_ENA.CMM, 0  
   3:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   4:SEND COMMAND                  :CAN-02, C72, PS_ENA_DIS_VACUUM_SENSOR_RED_ENA.CMM, 0
   5:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   6:REQUEST MESSAGE               :GEP-05, Medir Vp en CH113, NONE
   7:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
   8:REQUEST MESSAGE               :GEP-05, Line Switch ONLY - Sensor NOT POWERED, NONE
   9:SEND COMMAND                  :CAN-02, C21, STM_ACQUIRE_RED_ENER_DIS.CMM, 0
  10:SEND COMMAND                  :CAN-02, C22, STM_READ_RED.CMM, 0
  11:REQUEST MESSAGE               :GEP-05, Group switch ON, NONE
  12:SEND COMMAND                  :CAN-02, C72, PS_ENA_DIS_VACUUM_SENSOR_RED_DIS.CMM, 0
  13:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
  14:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  15:SEND COMMAND                  :CAN-02, C71, PS_ENA_DIS_HT_HEATER_GROUP_RED_DIS.CMM, 0
  16:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
  17:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE