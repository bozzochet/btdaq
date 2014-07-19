  10:REQUEST MESSAGE               :GEP-05, configure the scope to measure 2s pulse CSP_PULSED_PILOT_VALVES, NONE  

  54:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  55:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  56:SEND COMMAND                  :CAN-02, C93, CSP_RDS_CONFIGURATION_1N_TRDS10_2S_NOM.CMM, 0
  57:SEND COMMAND                  :CAN-02, C93, CSP_RDS_CONFIGURATION_2N_TRDS10_2S_NOM.CMM, 0
  58:EXECUTE TEST SEQUENCE         :GEP-22, CSP_GET_CONFIG_AQS_RDS_N, NONE
  55:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  54:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  55:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  56:SEND COMMAND                  :CAN-02, C93, CSP_RDS_CONFIGURATION_1R_TRDS10_2S_RED.CMM, 0
  57:SEND COMMAND                  :CAN-02, C93, CSP_RDS_CONFIGURATION_2R_TRDS10_2S_RED.CMM, 0
  58:EXECUTE TEST SEQUENCE         :GEP-22, CSP_GET_CONFIG_AQS_RDS_R, NONE
  55:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
   
   2:REQUEST MESSAGE               :GEP-05, measure DV65AO (open_N), NONE

  59:SEND COMMAND                  :CAN-02, CE8, CSP_ARM_DISARM_PULSED_VALVE_NOM_V065A_ARM.CMM, 0
  60:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  61:REQUEST MESSAGE               :GEP-05, READY TO CAPTURE, NONE
  63:SEND COMMAND                  :CAN-02, CEB, CSP_OPERATE_PULSED_VALVE_NOM.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  67:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, measure DV65AS CLOSE by Nominal electronics, NONE

   4:SEND COMMAND                  :CAN-02, CE8, CSP_ARM_DISARM_PULSED_VALVE_NOM_V165A_ARM.CMM, 0
   5:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  61:REQUEST MESSAGE               :GEP-05, READY TO CAPTURE, NONE
   8:SEND COMMAND                  :CAN-02, CEB, CSP_OPERATE_PULSED_VALVE_NOM.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
   9:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, measure DV65AS CLOSE by Redundant electronics, NONE

   4:SEND COMMAND                  :CAN-02, CE8, CSP_ARM_DISARM_PULSED_VALVE_RED_V165A_ARM.CMM, 0
   5:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  61:REQUEST MESSAGE               :GEP-05, READY TO CAPTURE, NONE
   8:SEND COMMAND                  :CAN-02, CEB, CSP_OPERATE_PULSED_VALVE_RED.CMM, 0
   9:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
 

   2:REQUEST MESSAGE               :GEP-05, measure DV65CO (open_N), NONE

  59:SEND COMMAND                  :CAN-02, CE8, CSP_ARM_DISARM_PULSED_VALVE_NOM_V065C_ARM.CMM, 0
  60:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  61:REQUEST MESSAGE               :GEP-05, READY TO CAPTURE, NONE
  63:SEND COMMAND                  :CAN-02, CEB, CSP_OPERATE_PULSED_VALVE_NOM.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  67:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, measure DV65CS CLOSE by Nominal electronics, NONE

   4:SEND COMMAND                  :CAN-02, CE8, CSP_ARM_DISARM_PULSED_VALVE_NOM_V165C_ARM.CMM, 0
   5:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  61:REQUEST MESSAGE               :GEP-05, READY TO CAPTURE, NONE
   8:SEND COMMAND                  :CAN-02, CEB, CSP_OPERATE_PULSED_VALVE_NOM.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
   9:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, measure DV65CS CLOSE by Redundant electronics, NONE

   4:SEND COMMAND                  :CAN-02, CE8, CSP_ARM_DISARM_PULSED_VALVE_RED_V165C_ARM.CMM, 0
   5:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  61:REQUEST MESSAGE               :GEP-05, READY TO CAPTURE, NONE
   8:SEND COMMAND                  :CAN-02, CEB, CSP_OPERATE_PULSED_VALVE_RED.CMM, 0
   9:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

   2:REQUEST MESSAGE               :GEP-05, measure DV65BO (open_R), NONE

  59:SEND COMMAND                  :CAN-02, CE8, CSP_ARM_DISARM_PULSED_VALVE_RED_V065B_ARM.CMM, 0
  60:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  61:REQUEST MESSAGE               :GEP-05, READY TO CAPTURE, NONE
  63:SEND COMMAND                  :CAN-02, CEB, CSP_OPERATE_PULSED_VALVE_RED.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  67:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, measure DV65BS CLOSE by Nominal electronics, NONE

   4:SEND COMMAND                  :CAN-02, CE8, CSP_ARM_DISARM_PULSED_VALVE_NOM_V165B_ARM.CMM, 0
   5:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  61:REQUEST MESSAGE               :GEP-05, READY TO CAPTURE, NONE
   8:SEND COMMAND                  :CAN-02, CEB, CSP_OPERATE_PULSED_VALVE_NOM.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
   9:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, measure DV65BS CLOSE by Redundant electronics, NONE

   4:SEND COMMAND                  :CAN-02, CE8, CSP_ARM_DISARM_PULSED_VALVE_RED_V165B_ARM.CMM, 0
   5:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  61:REQUEST MESSAGE               :GEP-05, READY TO CAPTURE, NONE
   8:SEND COMMAND                  :CAN-02, CEB, CSP_OPERATE_PULSED_VALVE_RED.CMM, 0
   9:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

   2:REQUEST MESSAGE               :GEP-05, measure DV65DO (open_R), NONE

  59:SEND COMMAND                  :CAN-02, CE8, CSP_ARM_DISARM_PULSED_VALVE_RED_V065D_ARM.CMM, 0
  60:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  61:REQUEST MESSAGE               :GEP-05, READY TO CAPTURE, NONE
  63:SEND COMMAND                  :CAN-02, CEB, CSP_OPERATE_PULSED_VALVE_RED.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  67:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, measure DV65DS CLOSE by Nominal electronics, NONE

   4:SEND COMMAND                  :CAN-02, CE8, CSP_ARM_DISARM_PULSED_VALVE_NOM_V165D_ARM.CMM, 0
   5:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  61:REQUEST MESSAGE               :GEP-05, READY TO CAPTURE, NONE
   8:SEND COMMAND                  :CAN-02, CEB, CSP_OPERATE_PULSED_VALVE_NOM.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
   9:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, measure DV65DS CLOSE by Redundant electronics, NONE

   4:SEND COMMAND                  :CAN-02, CE8, CSP_ARM_DISARM_PULSED_VALVE_RED_V165D_ARM.CMM, 0
   5:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  61:REQUEST MESSAGE               :GEP-05, READY TO CAPTURE, NONE
   8:SEND COMMAND                  :CAN-02, CEB, CSP_OPERATE_PULSED_VALVE_RED.CMM, 0
   9:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, CSP_Pulsed_Pilot_Valves Test Finished, NONE
  
   2:REQUEST MESSAGE               :GEP-05, Measure with DMM CSP_BILEVEL VALVES, NONE

   2:REQUEST MESSAGE               :GEP-05, Connect DMM CH13 to DV56A, NONE
   
  16:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_NOM_ARM_OPEN_V56A.CMM, 0
  17:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, running TO OPEN, NONE
  19:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_NOM.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  20:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_NOM.CMM, 0
  21:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  22:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
   6:REQUEST MESSAGE               :GEP-05, running TO CLOSE, NONE
  24:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_NOM_ARM_CLOSE_V56A.CMM, 0
  25:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  26:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE 
  27:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_NOM.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  28:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_NOM.CMM, 0
  29:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  30:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

   2:REQUEST MESSAGE               :GEP-05, Connect DMM CH13 to DV56B, NONE
   
  16:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_RED_ARM_OPEN_V56B.CMM, 0
  17:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, running TO OPEN, NONE
  19:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_RED.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  20:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_RED.CMM, 0
  21:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  22:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
   6:REQUEST MESSAGE               :GEP-05, running TO CLOSE, NONE
  24:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_RED_ARM_CLOSE_V56B.CMM, 0
  25:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  26:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE 
  27:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_RED.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  28:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_RED.CMM, 0
  29:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  30:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

   2:REQUEST MESSAGE               :GEP-05, Connect DMM CH13 to DV21A, NONE
   
  16:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_NOM_ARM_OPEN_V21A.CMM, 0
  17:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, running TO OPEN, NONE
  19:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_NOM.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  20:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_NOM.CMM, 0
  21:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  22:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
   6:REQUEST MESSAGE               :GEP-05, running TO CLOSE, NONE
  24:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_NOM_ARM_CLOSE_V21A.CMM, 0
  25:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  26:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE 
  27:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_NOM.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  28:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_NOM.CMM, 0
  29:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  30:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

   2:REQUEST MESSAGE               :GEP-05, Connect DMM CH13 to DV21B, NONE
   
  16:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_RED_ARM_OPEN_V21B.CMM, 0
  17:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, running TO OPEN, NONE
  19:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_RED.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  20:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_RED.CMM, 0
  21:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  22:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
   6:REQUEST MESSAGE               :GEP-05, running TO CLOSE, NONE
  24:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_RED_ARM_CLOSE_V21B.CMM, 0
  25:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  26:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE 
  27:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_RED.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  28:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_RED.CMM, 0
  29:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  30:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

   2:REQUEST MESSAGE               :GEP-05, Connect DMM CH13 to DV21C, NONE
   
  16:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_NOM_ARM_OPEN_V21C.CMM, 0
  17:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, running TO OPEN, NONE
  19:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_NOM.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  20:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_NOM.CMM, 0
  21:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  22:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
   6:REQUEST MESSAGE               :GEP-05, running TO CLOSE, NONE
  24:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_NOM_ARM_CLOSE_V21C.CMM, 0
  25:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  26:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE 
  27:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_NOM.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  28:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_NOM.CMM, 0
  29:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  30:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

   2:REQUEST MESSAGE               :GEP-05, Connect DMM CH13 to DV21D, NONE
   
  16:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_RED_ARM_OPEN_V21D.CMM, 0
  17:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, running TO OPEN, NONE
  19:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_RED.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  20:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_RED.CMM, 0
  21:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  22:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
   6:REQUEST MESSAGE               :GEP-05, running TO CLOSE, NONE
  24:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_RED_ARM_CLOSE_V21D.CMM, 0
  25:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  26:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE 
  27:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_RED.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  28:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_RED.CMM, 0
  29:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE

   2:REQUEST MESSAGE               :GEP-05, Connect DMM CH13 to DV64 (NOMINAL ELECTRONICS), NONE
   
  16:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_NOM_ARM_OPEN_V064.CMM, 0
  17:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, running TO OPEN, NONE
  19:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_NOM.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  20:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_NOM.CMM, 0
  21:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  22:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
   6:REQUEST MESSAGE               :GEP-05, running TO CLOSE, NONE
  24:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_NOM_ARM_CLOSE_V064.CMM, 0
  25:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  26:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE 
  27:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_NOM.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  28:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_NOM.CMM, 0
  29:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  30:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

   2:REQUEST MESSAGE               :GEP-05, Connect DMM CH13 to DV64 (REDUNDANT ELECTRONICS), NONE
   
  16:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_RED_ARM_OPEN_V064.CMM, 0
  17:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, running TO OPEN, NONE
  19:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_RED.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  20:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_RED.CMM, 0
  21:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  22:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
   6:REQUEST MESSAGE               :GEP-05, running TO CLOSE, NONE
  24:SEND COMMAND                  :CAN-02, CE4, CSP_ARM_DISARM_BIL_VALVE_RED_ARM_CLOSE_V064.CMM, 0
  25:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  26:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE 
  27:SEND COMMAND                  :CAN-02, CE7, CSP_OPERATE_BIL_VALVE_RED.CMM, 0
   1:READ POWER SIGNAL             :DMM-01, CAB, CCS_TO_ACTIVE_L_V, IF2, NONE, VOLTS
  28:SEND COMMAND                  :CAN-02, CC9, CSP_GET_VALVES_STATUS_RED.CMM, 0
  29:REQUEST MESSAGE               :GEP-05, This is a request message example, NONE
  30:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, CSP_Bilevel_Valves Test Finished, NONE

  10:REQUEST MESSAGE               :GEP-05, configure the scope to measure 112Ms pulse CSP_MOOG_VALVES, NONE  

   2:REQUEST MESSAGE               :GEP-05, Measure with SCOPE CSP_MOOG VALVE DV22A, NONE

  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  45:REQUEST MESSAGE               :GEP-05, This is a request message , NONE 
   6:SEND COMMAND                  :CAN-02, CED, CSP_ARM_DISARM_MOOG_VALVE_NOM_ARM_OPEN_V22A.CMM, 0
  42:REQUEST MESSAGE               :GEP-05, Ready to measure!! , NONE 
  43:SEND COMMAND                  :CAN-02, CEE, CSP_OPERATE_MOOG_VALVE_NOM.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, CHANGE DV22A TO CLOSE, measure by Nominal electronics, NONE

   6:SEND COMMAND                  :CAN-02, CED, CSP_ARM_DISARM_MOOG_VALVE_NOM_ARM_CLOSE_V22A.CMM, 0
  42:REQUEST MESSAGE               :GEP-05, Ready to measure!! , NONE 
  43:SEND COMMAND                  :CAN-02, CEE, CSP_OPERATE_MOOG_VALVE_NOM.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, measure DV22A TO CLOSE by Redundant electronics, NONE

   6:SEND COMMAND                  :CAN-02, CED, CSP_ARM_DISARM_MOOG_VALVE_RED_ARM_CLOSE_V22A.CMM, 0
  42:REQUEST MESSAGE               :GEP-05, Ready to measure!! , NONE 
  43:SEND COMMAND                  :CAN-02, CEE, CSP_OPERATE_MOOG_VALVE_RED.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

   2:REQUEST MESSAGE               :GEP-05, Measure with SCOPE CSP_MOOG VALVE DV22C, NONE

  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
  45:REQUEST MESSAGE               :GEP-05, This is a request message , NONE 
   6:SEND COMMAND                  :CAN-02, CED, CSP_ARM_DISARM_MOOG_VALVE_NOM_ARM_OPEN_V22C.CMM, 0
  42:REQUEST MESSAGE               :GEP-05, Ready to measure!! , NONE 
  43:SEND COMMAND                  :CAN-02, CEE, CSP_OPERATE_MOOG_VALVE_NOM.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, CHANGE DV22C TO CLOSE, measure by Nominal electronics, NONE

   6:SEND COMMAND                  :CAN-02, CED, CSP_ARM_DISARM_MOOG_VALVE_NOM_ARM_CLOSE_V22C.CMM, 0
  42:REQUEST MESSAGE               :GEP-05, Ready to measure!! , NONE 
  43:SEND COMMAND                  :CAN-02, CEE, CSP_OPERATE_MOOG_VALVE_NOM.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, measure DV22C TO CLOSE by Redundant electronics, NONE

   6:SEND COMMAND                  :CAN-02, CED, CSP_ARM_DISARM_MOOG_VALVE_RED_ARM_CLOSE_V22C.CMM, 0
  42:REQUEST MESSAGE               :GEP-05, Ready to measure!! , NONE 
  43:SEND COMMAND                  :CAN-02, CEE, CSP_OPERATE_MOOG_VALVE_RED.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

   2:REQUEST MESSAGE               :GEP-05, Measure with SCOPE CSP_MOOG VALVE DV22B, NONE

  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  45:REQUEST MESSAGE               :GEP-05, This is a request message , NONE 
   6:SEND COMMAND                  :CAN-02, CED, CSP_ARM_DISARM_MOOG_VALVE_RED_ARM_OPEN_V22B.CMM, 0
  42:REQUEST MESSAGE               :GEP-05, Ready to measure!! , NONE 
  43:SEND COMMAND                  :CAN-02, CEE, CSP_OPERATE_MOOG_VALVE_RED.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, CHANGE DV22B TO CLOSE by Redundant electronics, NONE

   6:SEND COMMAND                  :CAN-02, CED, CSP_ARM_DISARM_MOOG_VALVE_RED_ARM_CLOSE_V22B.CMM, 0
  42:REQUEST MESSAGE               :GEP-05, Ready to measure!! , NONE 
  43:SEND COMMAND                  :CAN-02, CEE, CSP_OPERATE_MOOG_VALVE_RED.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, measure DV22B TO CLOSE, measure by Nominal electronics, NONE

   6:SEND COMMAND                  :CAN-02, CED, CSP_ARM_DISARM_MOOG_VALVE_NOM_ARM_CLOSE_V22B.CMM, 0
  42:REQUEST MESSAGE               :GEP-05, Ready to measure!! , NONE 
  43:SEND COMMAND                  :CAN-02, CEE, CSP_OPERATE_MOOG_VALVE_NOM.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0

   2:REQUEST MESSAGE               :GEP-05, Measure with SCOPE CSP_MOOG VALVE DV22D, NONE

  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0
  45:REQUEST MESSAGE               :GEP-05, This is a request message , NONE 
   6:SEND COMMAND                  :CAN-02, CED, CSP_ARM_DISARM_MOOG_VALVE_RED_ARM_OPEN_V22D.CMM, 0
  42:REQUEST MESSAGE               :GEP-05, Ready to measure!! , NONE 
  43:SEND COMMAND                  :CAN-02, CEE, CSP_OPERATE_MOOG_VALVE_RED.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, CHANGE DV22D TO CLOSE by Redundant electronics, NONE

   6:SEND COMMAND                  :CAN-02, CED, CSP_ARM_DISARM_MOOG_VALVE_RED_ARM_CLOSE_V22D.CMM, 0
  42:REQUEST MESSAGE               :GEP-05, Ready to measure!! , NONE 
  43:SEND COMMAND                  :CAN-02, CEE, CSP_OPERATE_MOOG_VALVE_RED.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_RED.CMM, 0

  10:REQUEST MESSAGE               :GEP-05, measure DV22D TO CLOSE, measure by Nominal electronics, NONE

   6:SEND COMMAND                  :CAN-02, CED, CSP_ARM_DISARM_MOOG_VALVE_NOM_ARM_CLOSE_V22D.CMM, 0
  42:REQUEST MESSAGE               :GEP-05, Ready to measure!! , NONE 
  43:SEND COMMAND                  :CAN-02, CEE, CSP_OPERATE_MOOG_VALVE_NOM.CMM, 0
  10:REQUEST MESSAGE               :GEP-05, check the amplitude & width on the scope, NONE
  44:SEND COMMAND                  :CAN-02, CC5, CSP_GET_GENERAL_STATUS_NOM.CMM, 0
   

 
 
