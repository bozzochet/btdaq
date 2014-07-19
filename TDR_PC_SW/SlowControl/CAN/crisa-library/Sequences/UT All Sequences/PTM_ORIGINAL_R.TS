   1:SEND COMMAND                  :CAN-02, C28, PTM_START_ACQ_SEQ_RED.CMM, 0
   2:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 2, SEC
   3:SEND COMMAND                  :CAN-02, C2B, PTM_READ_RED.CMM, 0
   4:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE