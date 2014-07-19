   1:SEND COMMAND                  :CAN-02, C47, CCS_ANALOG_TM_ACQUIRE.CMM, 0
   2:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 100, mSEC
   3:SEND COMMAND                  :CAN-02, C48, CCS_READ_ANALOG_TM.CMM, 0
   4:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   5:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 100, mSEC
   6:SEND COMMAND                  :CAN-02, C21, STM_ACQUIRE_NOM_ENER_DIS.CMM, 0
   7:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   8:WAIT N SECONDS/MILLI-SECONDS  :GEP-04, 100, mSEC
   9:SEND COMMAND                  :CAN-02, C22, STM_READ_NOM.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE