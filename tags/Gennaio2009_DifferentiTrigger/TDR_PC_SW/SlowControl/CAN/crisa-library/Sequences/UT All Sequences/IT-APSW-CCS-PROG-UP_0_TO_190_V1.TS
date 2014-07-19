  11:INFO MESSAGE                  :GEP-06, Verify that value of current is zero, NONE
  12:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  13:INFO MESSAGE                  :GEP-06, Start sequence, NONE
  14:SEND COMMAND                  :CAN-02, C41, CCS_CURRENT_SETPOINT_SEQ_CURR1_190h.cmm, 0
  15:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  16:SEND COMMAND                  :CAN-02, C41, CCS_CURRENT_SETPOINT_SEQ_CURR2_190h.cmm, 0
  17:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  18:SEND COMMAND                  :CAN-02, C41, CCS_CURRENT_SETPOINT_SEQ_CURR3_190h.cmm, 0
  19:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  20:INFO MESSAGE                  :GEP-06, Verify that CCS current prog sequence is not in progress, NONE
  21:SEND COMMAND                  :CAN-02, C39, CCS_START_CURRENT_SETPOINT_SEQ_V1.CMM, 0
  22:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  23:INFO MESSAGE                  :GEP-06, Verify CCS current prog sequence is in progress, NONE
  25:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  26:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  27:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  28:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  29:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  30:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  31:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
  32:SEND COMMAND                  :CAN-02, C47, CCS_ANALOG_TM_ACQUIRE.CMM, 0
  33:SEND COMMAND                  :CAN-02, C48, CCS_READ_ANALOG_TM.CMM, 0
  34:SEND COMMAND                  :CAN-02, C03, CAB_GET_ESSENTIAL_STATUS.CMM, 0
  35:INFO MESSAGE                  :GEP-06, Verify that value of current is that programmed, NONE
  36:SEND COMMAND                  :CAN-02, C44, CCS_READ_SETPOINT_CMDS.cmm, 0
   4:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  37:SEND COMMAND                  :CAN-02, C1E, CAB_READ_AND_RESET_ERROR_INFO.CMM, 0