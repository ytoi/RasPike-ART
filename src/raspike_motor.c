#include <stdio.h>
#include <string.h>
#include "raspike_protocol_api.h"
#include "raspike_protocol_com.h"
#include "raspike_internal.h"
#include "motor.h"


DECLARE_DEVICE_TYPE_IN_FILE(RP_CMD_TYPE_MOTOR)

pup_motor_t *pup_motor_get_device(pbio_port_id_t port)
{
  GET_DEVICE_COMMON(pup_motor_t);
}

pbio_error_t pup_motor_setup(pup_motor_t *motor, pup_direction_t positive_direction, bool reset_count)
{
  struct _pup_device_t *p = (struct _pup_device_t*)motor;
  RasPikePort port = p->port_id;
  char cmd = RP_CMD_ID_MOT_STU;

  unsigned char data[RASPIKE_PORT_DATA_SIZE];
  memcpy(data+RP_MOTOR_STU_INDEX_DIRECTION,&positive_direction,sizeof(positive_direction));
  memcpy(data+RP_MOTOR_STU_INDEX_RESETCOUNT,&reset_count,sizeof(reset_count));

  raspike_prot_send(port,cmd,data,sizeof(data));
  int ret = raspike_wait_ack(port,cmd);

  return ret;
}

pbio_error_t pup_motor_reset_count(pup_motor_t *motor)
{
  
  struct _pup_device_t *pdev = (struct _pup_device_t*)motor;

  RasPikePort port = pdev->port_id;
  char cmd = RP_CMD_ID_MOT_RST;
  raspike_prot_send(port,cmd,0,0);
  int ret = raspike_wait_ack(port,cmd);

  /* To ensure motor count is resetted, clear local status's count. 
     after this, get_count returns 0 or updated value after resetted */
  RPProtocolSpikeStatus *p = raspike_prot_get_saved_status();
  RPProtocolPortStatus *pp = p->ports+port;
  memset(pp->data+RP_MOTOR_INDEX_COUNT,0,sizeof(int32_t));
  
  return (pbio_error_t)ret;
}

int32_t pup_motor_get_count(pup_motor_t *motor)
{
  struct _pup_device_t *pdev = (struct _pup_device_t*)motor;  
  GET_AND_RET_SENSOR_COMMON(int32_t,RP_MOTOR_INDEX_COUNT);
}

int32_t pup_motor_get_speed(pup_motor_t *motor)
{
  struct _pup_device_t *pdev = (struct _pup_device_t*)motor;  
  GET_AND_RET_SENSOR_COMMON(int32_t,RP_MOTOR_INDEX_SPEED);
}  

pbio_error_t pup_motor_set_speed(pup_motor_t *motor, int speed)
{
  struct _pup_device_t *pdev = (struct _pup_device_t*)motor;
  RasPikePort port = pdev->port_id;
  char cmd = RP_CMD_ID_MOT_SPD;
  pdev->cmd = cmd;
  raspike_prot_send(port,cmd,(unsigned char*)&speed,sizeof(speed));

  // Do not wait ack for performance
return PBIO_SUCCESS;
}

int32_t pup_motor_get_power(pup_motor_t *motor)
{
  struct _pup_device_t *pdev = (struct _pup_device_t*)motor;  

  /* if current mode is POWER, return local value */
  if ( pdev->cmd == RP_CMD_ID_MOT_POW ) {
    return pdev->power;
  }
  /* return value is int32_t but power only uses -100 - +100,
     to reduce data size, use int16_t instead */
  GET_AND_RET_SENSOR_COMMON(int16_t,RP_MOTOR_INDEX_POWER);
}

#include <time.h>

pbio_error_t pup_motor_set_power(pup_motor_t *motor, int power)
{
  struct _pup_device_t *pdev = (struct _pup_device_t*)motor;
  RasPikePort port = pdev->port_id;
  char cmd = RP_CMD_ID_MOT_POW;
  pdev->cmd = cmd;
  // save as local value 
  pdev->power = power;

#define DO_MEASURING 0
#if DO_MEASURING == 1  
  struct timespec prev = {0};
  struct timespec cur;

  clock_gettime(CLOCK_MONOTONIC,&prev);
#endif
  
  raspike_prot_send(port,cmd,(unsigned char*)&power,sizeof(power));
  //raspike_prot_send(port,cmd,(unsigned char*)buf,sizeof(buf));
#if DO_MEASURING == 1  
  clock_gettime(CLOCK_MONOTONIC,&cur);
  int diff = (cur.tv_sec-prev.tv_sec)*1000+(cur.tv_nsec-prev.tv_nsec)/1000000;
  printf("Motor setspeed spends=%d\n",diff);
#endif
  
  // Do not wait ack for performance
  return 0;
}

pbio_error_t pup_motor_stop(pup_motor_t *motor)
{
  struct _pup_device_t *pdev = (struct _pup_device_t*)motor;
  WAITACK_AND_RET_COMMON(pbio_error_t,RP_CMD_ID_MOT_STP);
}


pbio_error_t pup_motor_brake(pup_motor_t *motor)
{
  struct _pup_device_t *pdev = (struct _pup_device_t*)motor;
  WAITACK_AND_RET_COMMON(pbio_error_t,RP_CMD_ID_MOT_STP_BRK);
}

pbio_error_t pup_motor_hold(pup_motor_t *motor)
{
  struct _pup_device_t *pdev = (struct _pup_device_t*)motor;
  WAITACK_AND_RET_COMMON(pbio_error_t,RP_CMD_ID_MOT_STP_HLD);
}

bool pup_motor_is_stalled(pup_motor_t *motor)
{
  struct _pup_device_t *pdev = (struct _pup_device_t*)motor;  
  GET_AND_RET_SENSOR_COMMON(bool,RP_MOTOR_INDEX_ISSTALLED);
}

int32_t pup_motor_set_duty_limit(pup_motor_t *motor, int duty_limit)
{
  struct _pup_device_t *pdev = (struct _pup_device_t*)motor;    
  WAITACK_AND_RET_COMMON_WITH_DATA(int32_t,RP_CMD_ID_MOT_SET_DTY,&duty_limit,sizeof(duty_limit));
}

/* to use WAITACK_AND_RET_COMMON_WITH_DATA, define with return value version */
static int32_t _pup_motor_restore_duty_limit(pup_motor_t *motor, int old_value)
{
  struct _pup_device_t *pdev = (struct _pup_device_t*)motor;
  WAITACK_AND_RET_COMMON_WITH_DATA(int32_t,RP_CMD_ID_MOT_RST_DTY,&old_value,sizeof(old_value));
}

void pup_motor_restore_duty_limit(pup_motor_t *motor, int old_value)
{
  _pup_motor_restore_duty_limit(motor, old_value);
}


  
