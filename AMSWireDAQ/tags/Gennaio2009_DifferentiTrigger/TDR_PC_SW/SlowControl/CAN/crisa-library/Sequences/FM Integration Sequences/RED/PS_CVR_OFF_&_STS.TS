   1:SEND COMMAND                  :CAN-02, C7D, PS_ON_OFF_CV_RED_OFF.CMM, 0
   2:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 2, SEC
   3:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
   4:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE