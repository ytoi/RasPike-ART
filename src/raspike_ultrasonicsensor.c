#include <stdio.h>
#include "raspike_protocol_api.h"
#include "raspike_protocol_com.h"
#include "raspike_internal.h"
#include "ultrasonicsensor.h"

DECLARE_DEVICE_TYPE_IN_FILE(RP_CMD_TYPE_US)
  
pup_device_t *pup_ultrasonic_sensor_get_device(pbio_port_id_t port)
{
  GET_DEVICE_COMMON(pup_device_t);

}


int32_t pup_ultrasonic_sensor_distance(pup_device_t *pdev)
{
  GET_AND_RET_SENSOR_COMMON(int32_t,RP_US_INDEX_DISTANCE);
}


bool pup_ultrasonic_sensor_presence(pup_device_t *pdev)
{
  GET_AND_RET_SENSOR_COMMON(bool,RP_US_INDEX_PRESENCE);  
}

pbio_error_t pup_ultrasonic_sensor_light_set(pup_device_t *pdev, 
                                             int32_t bv1, int32_t bv2, int32_t bv3, int32_t bv4)
{
  char data[16];
  *(int32_t*)(data+RP_US_LGT_SET_INDEX_BV1) = bv1;
  *(int32_t*)(data+RP_US_LGT_SET_INDEX_BV2) = bv2;
  *(int32_t*)(data+RP_US_LGT_SET_INDEX_BV3) = bv3;
  *(int32_t*)(data+RP_US_LGT_SET_INDEX_BV4) = bv4;

  // No wait ack for performance
  //WAITACK_AND_RET_COMMON_WITH_DATA(pbio_error_t,RP_CMD_ID_US_LGT_SET,data,sizeof(data));

  ENSURE_VALID_DEVICE(pdev);
  RasPikePort port = pdev->port_id;	  
  unsigned char cmd = RP_CMD_ID_US_LGT_SET;
  raspike_prot_send(port,cmd,(unsigned char*)data,sizeof(data));

  return 0; // always returns OK
  
}

pbio_error_t pup_ultrasonic_sensor_light_on(pup_device_t *pdev)
{
  ENSURE_VALID_DEVICE(pdev);
  RasPikePort port = pdev->port_id;	  
  unsigned char cmd = RP_CMD_ID_US_LGT_ON;
  raspike_prot_send(port,cmd,0,0);
  
  return 0; // always returns OK
}
 
pbio_error_t pup_ultrasonic_sensor_light_off(pup_device_t *pdev)
{
  ENSURE_VALID_DEVICE(pdev);
  RasPikePort port = pdev->port_id;	  
  unsigned char cmd = RP_CMD_ID_US_LGT_OFF;
  raspike_prot_send(port,cmd,0,0);
  
  return 0; // always returns OK
}
