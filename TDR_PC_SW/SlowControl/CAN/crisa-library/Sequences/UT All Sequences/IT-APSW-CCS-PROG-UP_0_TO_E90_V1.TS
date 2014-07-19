   1:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
   2:SEND COMMAND                  :CAN-02, C1E, CAB_READ_AND_RESET_ERROR_INFO.CMM, 0
   3:INFO MESSAGE                  :GEP-06, Verify that value of current is zero, NONE
   4:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
   5:INFO MESSAGE                  :GEP-06, Start sequence, NONE
   6:SEND COMMAND                  :CAN-02, C41, CCS_CURRENT_SETPOINT_SEQ_CURR1_E90h.cmm, 0
   7:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
   8:SEND COMMAND                  :CAN-02, C41, CCS_CURRENT_SETPOINT_SEQ_CURR2_E90h.cmm, 0
   9:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  10:SEND COMMAND                  :CAN-02, C41, CCS_CURRENT_SETPOINT_SEQ_CURR3_E90h.cmm, 0
  11:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  12:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  13:INFO MESSAGE                  :GEP-06, Verify that CCS current prog sequence is not in progress, NONE
  14:SEND COMMAND                  :CAN-02, C39, CCS_START_CURRENT_SETPOINT_SEQ_V1.CMM, 0
  15:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  16:INFO MESSAGE                  :GEP-06, Verify CCS current prog sequence is in progress, NONE
  18:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  19:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  20:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  21:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  22:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  23:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  24:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  25:SEND COMMAND                  :CAN-02, C47, CCS_ANALOG_TM_ACQUIRE.CMM, 0
  27:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  28:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  29:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  26:SEND COMMAND                  :CAN-02, C48, CCS_READ_ANALOG_TM.CMM, 0
  30:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  31:INFO MESSAGE                  :GEP-06, After some time, verify that CCS current prog sequence is finished, NONE
  32:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  33:INFO MESSAGE                  :GEP-06, Verify that value of current is that programmed, NONE
  34:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
   4:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  35:SEND COMMAND                  :CAN-02, C1E, CAB_READ_AND_RESET_ERROR_INFO.CMM, 0