   1:SEND COMMAND                  :CAN-02, C47, CCS_ANALOG_TM_ACQUIRE.CMM, 0
   2:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 2, SEC
   3:SEND COMMAND                  :CAN-02, C48, CCS_READ_ANALOG_TM.CMM, 0
   4:REQUEST MESSAGE               :GEP-05, For reading , NONE
   5:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
   6:REQUEST MESSAGE               :GEP-05, This is a request message , NONE