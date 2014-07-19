   2:REQUEST MESSAGE               :GEP-05, Group Switch Test, NONE
   1:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   3:SEND COMMAND                  :CAN-02, C77, PS_OPERATE_PULSED_PILOT_VALVE_RED_V61BO_02S.CMM, 0
   4:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0

   5:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   8:SEND COMMAND                  :CAN-02, C74, PS_ENA_DIS_PULSED_PILOT_VALVES_RED_ENA_ON.CMM, 0
   9:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, verify status, NONE
  11:SEND COMMAND                  :CAN-02, C77, PS_OPERATE_PULSED_PILOT_VALVE_RED_V61BO_02S.CMM, 0
  19:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 3, SEC
  14:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
  15:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   2:REQUEST MESSAGE               :GEP-05, verify status, NONE