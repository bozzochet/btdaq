   2:REQUEST MESSAGE               :GEP-05, Group Switch Test, NONE
   2:REQUEST MESSAGE               :GEP-05, Configure Scope to measure 2s pulse, NONE
   3:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   4:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   5:SEND COMMAND                  :CAN-02, C77, PS_OPERATE_PULSED_PILOT_VALVE_NOM_V59AS_02S.CMM, 0
   3:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0

   2:REQUEST MESSAGE               :GEP-05, Configure Scope to measure 2s pulse, NONE
  15:SEND COMMAND                  :CAN-02, C74, PS_ENA_DIS_PULSED_PILOT_VALVES_NOM_ENA_ OFF.CMM, 0
  16:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, verify status, NONE
  18:SEND COMMAND                  :CAN-02, C77, PS_OPERATE_PULSED_PILOT_VALVE_NOM_V59AS_02S.CMM, 0
  19:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 3, SEC
  20:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, verify status, NONE