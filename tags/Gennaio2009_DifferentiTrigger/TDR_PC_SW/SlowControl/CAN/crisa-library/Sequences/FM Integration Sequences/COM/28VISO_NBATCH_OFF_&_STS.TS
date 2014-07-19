   1:SEND COMMAND                  :CAN-02, CB8, CAB_ON_OFF_LCL_NOM_OFF.CMM, 0
   2:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 3, SEC
   3:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
   4:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE