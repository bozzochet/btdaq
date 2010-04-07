   1:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
   2:SEND COMMAND                  :CAN-02, C1E, CAB_READ_AND_RESET_ERROR_INFO.CMM, 0
   3:INFO MESSAGE                  :GEP-06, Verify that value of current is zero, NONE
   4:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
   5:INFO MESSAGE                  :GEP-06, Start sequence, NONE
   6:SEND COMMAND                  :CAN-02, C41, CCS_CURRENT_SETPOINT_SEQ_CURR1_444h.cmm, 0
   7:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
   8:SEND COMMAND                  :CAN-02, C41, CCS_CURRENT_SETPOINT_SEQ_CURR2_0h.cmm, 0
   9:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  10:SEND COMMAND                  :CAN-02, C41, CCS_CURRENT_SETPOINT_SEQ_CURR3_444h.cmm, 0
  11:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  13:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  12:INFO MESSAGE                  :GEP-06, Verify that CCS current prog sequence is not in progress, NONE
  14:SEND COMMAND                  :CAN-02, C39, CCS_START_CURRENT_SETPOINT_SEQ_V1.CMM, 0
  15:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  12:INFO MESSAGE                  :GEP-06, Verify CCS current prog sequence is in progress, NONE
  17:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  18:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  17:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  18:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  17:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  18:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  17:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  18:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  17:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  18:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  17:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  18:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  26:SEND COMMAND                  :CAN-02, C47, CCS_ANALOG_TM_ACQUIRE.CMM, 0
  27:SEND COMMAND                  :CAN-02, C48, CCS_READ_ANALOG_TM.CMM, 0
  19:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  66:INFO MESSAGE                  :GEP-06, After some time, verify that CCS current prog sequence is finished, NONE
  67:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  68:INFO MESSAGE                  :GEP-06, Verify that value of current is that programmed, NONE
  69:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
   8:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  70:SEND COMMAND                  :CAN-02, C1E, CAB_READ_AND_RESET_ERROR_INFO.CMM, 0