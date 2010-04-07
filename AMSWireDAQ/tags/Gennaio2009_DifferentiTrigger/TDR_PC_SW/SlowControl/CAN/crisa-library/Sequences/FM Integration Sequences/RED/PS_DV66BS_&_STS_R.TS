   1:SEND COMMAND                  :CAN-02, C74, PS_ENA_DIS_PULSED_PILOT_VALVES_RED_ENA_OFF.CMM, 0
   2:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   4:SEND COMMAND                  :CAN-02, C77, PS_OPERATE_PULSED_PILOT_VALVE_RED_V66BS_80S.CMM, 0
   5:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   6:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   7:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 85, SEC
   8:SEND COMMAND                  :CAN-02, C74, PS_ENA_DIS_PULSED_PILOT_VALVES_RED_DIS_OFF.CMM, 0
   9:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_RED.CMM, 0
   10:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   11:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
   12:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE