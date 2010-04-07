   3:REQUEST MESSAGE               :GEP-05, Hacer corto en Vacuum Sensor_Vp, NONE
  10:SEND COMMAND                  :CAN-02, C71, PS_ENA_DIS_HT_HEATER_GROUP_NOM_ENA.CMM, 0  
  11:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   4:SEND COMMAND                  :CAN-02, C72, PS_ENA_DIS_VACUUM_SENSOR_NOM_ENA.CMM, 0
   5:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, Quitar corto y medir 0 en Vacuum Sensor_Vp, NONE
   3:REQUEST MESSAGE               :GEP-05, Reintentar activar sin corto, NONE
  10:SEND COMMAND                  :CAN-02, C71, PS_ENA_DIS_HT_HEATER_GROUP_NOM_ENA.CMM, 0  
  11:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   4:SEND COMMAND                  :CAN-02, C72, PS_ENA_DIS_VACUUM_SENSOR_NOM_ENA.CMM, 0
   5:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, Vacuum Sensor_Vp sigue siendo 0, NONE
   3:REQUEST MESSAGE               :GEP-05, Reseteamos el LCL, NONE
   4:SEND COMMAND                  :CAN-02, C72, PS_ENA_DIS_VACUUM_SENSOR_NOM_DIS.CMM, 0
   1:SEND COMMAND                  :CAN-02, C71, PS_ENA_DIS_HT_HEATER_GROUP_NOM_DIS.CMM, 0  
   2:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, Activamos Vacuum Sensor, NONE
  10:SEND COMMAND                  :CAN-02, C71, PS_ENA_DIS_HT_HEATER_GROUP_NOM_ENA.CMM, 0  
  11:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   4:SEND COMMAND                  :CAN-02, C72, PS_ENA_DIS_VACUUM_SENSOR_NOM_ENA.CMM, 0
   5:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, Medir Vacuum Sensor_Vp, NONE
   3:REQUEST MESSAGE               :GEP-05, Apagamos Vacuum sensor driver, NONE
   4:SEND COMMAND                  :CAN-02, C72, PS_ENA_DIS_VACUUM_SENSOR_NOM_DIS.CMM, 0
   1:SEND COMMAND                  :CAN-02, C71, PS_ENA_DIS_HT_HEATER_GROUP_NOM_DIS.CMM, 0  
   2:SEND COMMAND                  :CAN-02, C05, CAB_GET_DIGITAL_STATUS_NOM.CMM, 0
   3:REQUEST MESSAGE               :GEP-05, FIN, NONE