   2:REQUEST MESSAGE               :GEP-05, Poner corto en DV20A OPEN, NONE
   1:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, Condiciones iniciales, NONE
  12:SEND COMMAND                  :CAN-02, C79, PS_ENA_DIS_MOOG_VALVES_NOM_ENA_GROUPA.CMM, 0
   1:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, Switch de grupo enable, NONE
   3:SEND COMMAND                  :CAN-02, C7C, PS_OPERATE_MOOG_VALVE_NOM_V20A_OPEN_255MS.CMM, 0
   4:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, Switch de linea ON, NONE
   2:REQUEST MESSAGE               :GEP-05, Quitar corto y meter osciloscopio, NONE
  12:SEND COMMAND                  :CAN-02, C79, PS_ENA_DIS_MOOG_VALVES_NOM_ENA_GROUPA.CMM, 0
   1:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, Group SW enable_comprobar no hay pulso, NONE
   3:SEND COMMAND                  :CAN-02, C7C, PS_OPERATE_MOOG_VALVE_NOM_V20A_OPEN_255MS.CMM, 0
   4:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, Resetear Switch de grupo, NONE
  12:SEND COMMAND                  :CAN-02, C79, PS_ENA_DIS_MOOG_VALVES_NOM_DIS_GROUPA.CMM, 0
   1:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, Comprobar Group SW enable, NONE
  2:REQUEST MESSAGE               :GEP-05, Group SW enable_comprobar pulso, NONE
   3:SEND COMMAND                  :CAN-02, C7C, PS_OPERATE_MOOG_VALVE_NOM_V20A_OPEN_255MS.CMM, 0
   4:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   2:REQUEST MESSAGE               :GEP-05, Finalizar secuencia, NONE


   
   