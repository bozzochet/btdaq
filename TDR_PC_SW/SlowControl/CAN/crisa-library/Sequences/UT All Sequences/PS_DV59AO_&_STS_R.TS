   2:REQUEST MESSAGE               :GEP-05, Driver OFF: Group switch DISABLED, NONE
   2:REQUEST MESSAGE               :GEP-05, Configure Scope to measure 2s pulse, NONE
   1:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   3:SEND COMMAND                  :CAN-02, C77, PS_OPERATE_PULSED_PILOT_VALVE_RED_V59AO_02S.CMM, 0
   4:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0

   2:REQUEST MESSAGE               :GEP-05, Configure scope to measure 2s, NONE
   8:SEND COMMAND                  :CAN-02, C74, PS_ENA_DIS_PULSED_PILOT_VALVES_RED_ENA_ON.CMM, 0
   9:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, verify status, NONE
  11:SEND COMMAND                  :CAN-02, C77, PS_OPERATE_PULSED_PILOT_VALVE_RED_V59AO_02S.CMM, 0
  12:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 3, SEC
  14:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, verify status, NONE
 
  2:REQUEST MESSAGE               :GEP-05, Configure Scope to measure 60s pulse, NONE
  16:SEND COMMAND                  :CAN-02, C74, PS_ENA_DIS_PULSED_PILOT_VALVES_RED_ENA_ON.CMM, 0
   9:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, verify status, NONE
  19:SEND COMMAND                  :CAN-02, C77, PS_OPERATE_PULSED_PILOT_VALVE_RED_V59AO_60S.CMM, 0 
  12:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 61, SEC
  14:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, verify status, NONE
  
   2:REQUEST MESSAGE               :GEP-05, Configure Scope to measure 127s pulse, NONE
  22:SEND COMMAND                  :CAN-02, C74, PS_ENA_DIS_PULSED_PILOT_VALVES_RED_ENA_ON.CMM, 0
   9:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, verify status, NONE
  25:SEND COMMAND                  :CAN-02, C77, PS_OPERATE_PULSED_PILOT_VALVE_RED_V59AO_127S.CMM, 0
  12:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 128, SEC
  20:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, verify status, NONE
 