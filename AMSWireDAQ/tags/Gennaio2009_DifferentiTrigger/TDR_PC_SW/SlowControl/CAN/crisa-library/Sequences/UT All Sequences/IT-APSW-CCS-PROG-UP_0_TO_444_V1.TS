   1:INFO MESSAGE                  :GEP-06, Verify that value of current is zero, NONE
   2:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
   3:INFO MESSAGE                  :GEP-06, Start sequence, NONE
   4:SEND COMMAND                  :CAN-02, C41, CCS_CURRENT_SETPOINT_SEQ_CURR1_444H.CMM, 0
   5:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
   6:SEND COMMAND                  :CAN-02, C41, CCS_CURRENT_SETPOINT_SEQ_CURR2_444H.CMM, 0
   7:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
   8:SEND COMMAND                  :CAN-02, C41, CCS_CURRENT_SETPOINT_SEQ_CURR3_444H.CMM, 0
   9:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  10:INFO MESSAGE                  :GEP-06, Verify that CCS current prog sequence is not in progress, NONE
  11:SEND COMMAND                  :CAN-02, C39, CCS_START_CURRENT_SETPOINT_SEQ_V1.CMM, 0
  12:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  13:INFO MESSAGE                  :GEP-06, Verify CCS current prog sequence is in progress, NONE
  15:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  16:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  17:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  18:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  19:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  20:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  21:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  15:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  16:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  17:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  18:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  19:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  20:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  21:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  22:SEND COMMAND                  :CAN-02, C47, CCS_ANALOG_TM_ACQUIRE.CMM, 0
  23:SEND COMMAND                  :CAN-02, C48, CCS_READ_ANALOG_TM.CMM, 0
  24:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  25:INFO MESSAGE                  :GEP-06, Verify that value of current is that programmed, NONE
  26:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  27:SEND COMMAND                  :CAN-02, C1E, CAB_READ_AND_RESET_ERROR_INFO.CMM, 0