#include <stdio.h>
#include "raspike_protocol_api.h"
#include "raspike_protocol_com.h"
#include "raspike_internal.h"
#include "forcesensor.h"

DECLARE_DEVICE_TYPE_IN_FILE(RP_CMD_TYPE_FORCE)
  
pup_device_t *pup_force_sensor_get_device(pbio_port_id_t port)
{
  GET_DEVICE_COMMON(pup_device_t);

}



float pup_force_sensor_force(pup_device_t *pdev)
{
  GET_AND_RET_SENSOR_COMMON(float,RP_FORCESENSOR_INDEX_FRC);
}

float pup_force_sensor_distance(pup_device_t *pdev)
{
  GET_AND_RET_SENSOR_COMMON(float,RP_FORCESENSOR_INDEX_DST);
}

bool pup_force_sensor_pressed(pup_device_t *pdev, float force)
{
  return ( pup_force_sensor_force(pdev) >= force );

}

bool pup_force_sensor_touched(pup_device_t *pdev)
{
  GET_AND_RET_SENSOR_COMMON(bool,RP_FORCESENSOR_INDEX_TCH);
}
