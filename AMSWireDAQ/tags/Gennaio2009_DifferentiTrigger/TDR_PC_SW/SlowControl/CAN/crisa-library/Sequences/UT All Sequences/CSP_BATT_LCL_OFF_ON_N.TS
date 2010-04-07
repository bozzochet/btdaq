   1:SEND COMMAND                  :CAN-02, CB8, CAB_ON_OFF_LCL_NOM_OFF.CMM, 0
   2:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   4:SEND COMMAND                  :CAN-02, CB8, CAB_ON_OFF_LCL_RED_OFF.CMM, 0
   5:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 3, SEC
   6:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   7:SEND COMMAND                  :CAN-02, CB8, CAB_ON_OFF_LCL_NOM_ON.CMM, 0
   8:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
   9:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  10:SEND COMMAND                  :CAN-02, CB8, CAB_ON_OFF_LCL_RED_ON.CMM, 0
  11:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  12:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE