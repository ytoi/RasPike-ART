#include <stdio.h>
#include "raspike_protocol_api.h"
#include "raspike_protocol_com.h"
#include "raspike_internal.h"
#include "colorsensor.h"


DECLARE_DEVICE_TYPE_IN_FILE(RP_CMD_TYPE_COLOR)

pup_device_t *pup_color_sensor_get_device(pbio_port_id_t port)
{
  GET_DEVICE_COMMON(pup_device_t);
}


pup_color_hsv_t pup_color_sensor_color(pup_device_t *pdev, bool surface)
{
  DELAYED_SENSOR_COMMON(pup_color_hsv_t,RP_CMD_ID_COL_COL);
}


pup_color_rgb_t pup_color_sensor_rgb(pup_device_t *pdev)
{
  DELAYED_SENSOR_COMMON(pup_color_rgb_t,RP_CMD_ID_COL_RGB);
}


pup_color_hsv_t pup_color_sensor_hsv(pup_device_t *pdev, bool surface)
{
  unsigned char cmd = ((surface==0)?RP_CMD_ID_COL_HSV_SUR_OFF:RP_CMD_ID_COL_HSV);
  DELAYED_SENSOR_COMMON(pup_color_hsv_t,cmd);  
}

int32_t pup_color_sensor_reflection(pup_device_t *pdev)
{
  DELAYED_SENSOR_COMMON(int32_t,RP_CMD_ID_COL_REF);    
}


int32_t pup_color_sensor_ambient(pup_device_t *pdev)
{
  DELAYED_SENSOR_COMMON(int32_t,RP_CMD_ID_COL_AMB);
}

pbio_error_t pup_color_sensor_light_set(pup_device_t *pdev, 
                                             int32_t bv1, int32_t bv2, int32_t bv3)
{
  ENSURE_VALID_DEVICE(pdev); 
  RasPikePort port = pdev->port_id;
  int32_t data[3] = { bv1,bv2,bv3};

  raspike_prot_send(port,RP_CMD_ID_COL_LIGHT_SET,(unsigned char*)data,sizeof(data));

  return (pbio_error_t)raspike_wait_ack(port,RP_CMD_ID_COL_LIGHT_SET);
}

pbio_error_t pup_color_sensor_light_on(pup_device_t *pdev)
{
  ENSURE_VALID_DEVICE(pdev); 
  RasPikePort port = pdev->port_id;
  raspike_prot_send(port,RP_CMD_ID_COL_LIGHT_ON,0,0);
  return PBIO_SUCCESS;
}

pbio_error_t pup_color_sensor_light_off(pup_device_t *pdev)
{
  ENSURE_VALID_DEVICE(pdev); 
  RasPikePort port = pdev->port_id;
  raspike_prot_send(port,RP_CMD_ID_COL_LIGHT_OFF,0,0);
  return PBIO_SUCCESS;
}

/* TODO: RasPike Limitation size must be less than 3 */
pup_color_hsv_t *pup_color_sensor_detectable_colors(int32_t size, pup_color_hsv_t *colors)
{
#if 0
  ENSURE_VALID_DEVICE(pdev); 
  RasPikePort port = RP_PORT_NONE;
  if ( size > 3 ) return colors; /* TODO: Which valus should be returned */
  raspike_prot_send(port,RP_CMD_ID_COL_DETECTABLE_COL,(char*)colors,size*sizeof(*colors));
#endif    
  return 0; /* TODO */

}
