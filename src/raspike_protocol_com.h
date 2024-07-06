#ifndef _RASPIKE_PROTOCOL_H
#define _RASPIKE_PROTOCOL_H

#include <stdint.h>
#include "raspike_protocol_com.h"

#define RP_CMD_INIT  (0xAE)
#define RP_CMD_START (0xEA)

#define RP_CMD_INIT_MAGIC (0xCE)

#define GET_CMD_TYPE(cmd) ((cmd)>>5)
#define GET_CMD_INDEX(cmd) ((cmd)&0x1f)
#define MAKE_CMD(type,id) ((type)<<5 | id)

#define RP_CMD_TYPE_SYS (0x0)
#define RP_CMD_TYPE_COLOR (0x1)
#define RP_CMD_TYPE_FORCE (0x2)
#define RP_CMD_TYPE_MOTOR (0x3)
#define RP_CMD_TYPE_US (0x4) // Ultrasonic Sensor
#define RP_CMD_TYPE_HUB (0x5) 


// PORT 
#define RP_PORT_NONE (255)

// SYSTEM CMD
#define RP_CMD_ID_ALL_STATUS (MAKE_CMD(RP_CMD_TYPE_SYS,0x1)) // 0x01
#define RP_CMD_ID_ACK (MAKE_CMD(RP_CMD_TYPE_SYS,0x2)) // 0x02
#define RP_CMD_ID_SHT_DWN (MAKE_CMD(RP_CMD_TYPE_SYS,0x3)) // 0x03

// COLOR SENSOR CMD
#define RP_CMD_ID_COL_CFG (MAKE_CMD(RP_CMD_TYPE_COLOR,0x0)) // 0x20
#define RP_CMD_ID_COL_RGB (MAKE_CMD(RP_CMD_TYPE_COLOR,0x1))// 0x21
#define RP_CMD_ID_COL_COL (MAKE_CMD(RP_CMD_TYPE_COLOR,0x2)) // 0x22
#define RP_CMD_ID_COL_COL_SUR_OFF (MAKE_CMD(RP_CMD_TYPE_COLOR,0x3)) // 0x23
#define RP_CMD_ID_COL_HSV (MAKE_CMD(RP_CMD_TYPE_COLOR,0x4)) // 0x24
#define RP_CMD_ID_COL_HSV_SUR_OFF (MAKE_CMD(RP_CMD_TYPE_COLOR,0x5)) // 0x25
#define RP_CMD_ID_COL_REF (MAKE_CMD(RP_CMD_TYPE_COLOR,0x6))// 0x26
#define RP_CMD_ID_COL_AMB (MAKE_CMD(RP_CMD_TYPE_COLOR,0x7)) // 0x27 
#define RP_CMD_ID_COL_LIGHT_SET (MAKE_CMD(RP_CMD_TYPE_COLOR,0x8)) // 0x28
#define RP_CMD_ID_COL_LIGHT_ON (MAKE_CMD(RP_CMD_TYPE_COLOR,0x9)) // 0x29
#define RP_CMD_ID_COL_LIGHT_OFF (MAKE_CMD(RP_CMD_TYPE_COLOR,0xa)) // 0x2a
#define RP_CMD_ID_COL_DETECTABLE_COL (MAKE_CMD(RP_CMD_TYPE_COLOR,0xb)) // 0x2b

// FORCE SENSOR CMD
#define RP_CMD_ID_FRC_CFG (MAKE_CMD(RP_CMD_TYPE_FORCE,0x0)) // 0x40
#define RP_CMD_ID_FRC_FRC (MAKE_CMD(RP_CMD_TYPE_FORCE,0x1)) // 0x41
#define RP_CMD_ID_FRC_DST (MAKE_CMD(RP_CMD_TYPE_FORCE,0x2)) // 0x42                                                                
#define RP_CMD_ID_FRC_PRS (MAKE_CMD(RP_CMD_TYPE_FORCE,0x3)) // 0x43
#define RP_CMD_ID_FRC_TCH (MAKE_CMD(RP_CMD_TYPE_FORCE,0x4)) // 0x44

/* Force sensor status protocol
   port->data[0]-data[3] : Force (float)
   port->data[4]-data[7] : Distance (float)
   port->data[8] : Is Touched (bool 1byte)
*/
#define RP_FORCESENSOR_INDEX_FRC (0)
#define RP_FORCESENSOR_INDEX_DST (4)
#define RP_FORCESENSOR_INDEX_TCH (8)



// MOTOR
#define RP_CMD_ID_MOT_CFG (MAKE_CMD(RP_CMD_TYPE_MOTOR,0x0)) // 0x60
#define RP_CMD_ID_MOT_STU (MAKE_CMD(RP_CMD_TYPE_MOTOR,0x1)) // 0x61
#define RP_CMD_ID_MOT_RST (MAKE_CMD(RP_CMD_TYPE_MOTOR,0x2)) // 0x62
#define RP_CMD_ID_MOT_SPD (MAKE_CMD(RP_CMD_TYPE_MOTOR,0x3)) // 0x63
#define RP_CMD_ID_MOT_POW (MAKE_CMD(RP_CMD_TYPE_MOTOR,0x4)) // 0x64
#define RP_CMD_ID_MOT_STP (MAKE_CMD(RP_CMD_TYPE_MOTOR,0x5)) // 0x65
#define RP_CMD_ID_MOT_STP_BRK (MAKE_CMD(RP_CMD_TYPE_MOTOR,0x6)) // 0x66
#define RP_CMD_ID_MOT_STP_HLD (MAKE_CMD(RP_CMD_TYPE_MOTOR,0x7)) // 0x67
#define RP_CMD_ID_MOT_SET_DTY (MAKE_CMD(RP_CMD_TYPE_MOTOR,0x8)) // 0x68
#define RP_CMD_ID_MOT_RST_DTY (MAKE_CMD(RP_CMD_TYPE_MOTOR,0x9)) // 0x69

/* Motor Set up protocol
   data[0]-data[3] : positive directoon
   data[4] : reset_count
*/
#define RP_MOTOR_STU_INDEX_DIRECTION (0)
#define RP_MOTOR_STU_INDEX_RESETCOUNT (4)

/* Motor status protocol 
   port->data[0]-data[3] : count(int32_t)
   port->data[4]-data[7] : speed(int32_t)
   port->data[8]-data[9] : power(int16_t) this is not same as original api
   port->data[10]        : is stalled
*/
#define RP_MOTOR_INDEX_COUNT (0)
#define RP_MOTOR_INDEX_SPEED (4)
#define RP_MOTOR_INDEX_POWER (8)
#define RP_MOTOR_INDEX_ISSTALLED (10)


// ULTRASONIC CMD
#define RP_CMD_ID_US_CFG (MAKE_CMD(RP_CMD_TYPE_US,0x0)) // 0x80
#define RP_CMD_ID_US_DST (MAKE_CMD(RP_CMD_TYPE_US,0x1)) // 0x81  Not Used
#define RP_CMD_ID_US_PRC (MAKE_CMD(RP_CMD_TYPE_US,0x2)) // 0x82  Not Used
#define RP_CMD_ID_US_LGT_SET (MAKE_CMD(RP_CMD_TYPE_US,0x3)) // 0x83
#define RP_CMD_ID_US_LGT_ON (MAKE_CMD(RP_CMD_TYPE_US,0x4)) // 0x84
#define RP_CMD_ID_US_LGT_OFF (MAKE_CMD(RP_CMD_TYPE_US,0x5)) // 0x85

/* Ultrasonic sensor cmd protocol
  LGT_SET
  int32_t x 4 : bv1-bv4
*/
#define RP_US_LGT_SET_INDEX_BV1 (0)
#define RP_US_LGT_SET_INDEX_BV2 (4)
#define RP_US_LGT_SET_INDEX_BV3 (8)
#define RP_US_LGT_SET_INDEX_BV4 (12)


/* Ultrasonic sensor status protocol
  port->data[0]-data[3] : distance(int32_t)
  port->data[4]         : presence(bool)
*/
#define RP_US_INDEX_DISTANCE (0)
#define RP_US_INDEX_PRESENCE (4)

/* HUB CMD*/
#define RP_CMD_ID_HUB_DISP_ORI (MAKE_CMD(RP_CMD_TYPE_HUB,0x1)) // 0xa1
#define RP_CMD_ID_HUB_DISP_OFF (MAKE_CMD(RP_CMD_TYPE_HUB,0x2)) // 0xa2
#define RP_CMD_ID_HUB_DISP_PIX (MAKE_CMD(RP_CMD_TYPE_HUB,0x3)) // 0xa3
#define RP_CMD_ID_HUB_DISP_IMG (MAKE_CMD(RP_CMD_TYPE_HUB,0x4)) // 0xa4
#define RP_CMD_ID_HUB_DISP_NUM (MAKE_CMD(RP_CMD_TYPE_HUB,0x5)) // 0xa5
#define RP_CMD_ID_HUB_DISP_CHR (MAKE_CMD(RP_CMD_TYPE_HUB,0x6)) // 0xa6
#define RP_CMD_ID_HUB_DISP_TXT (MAKE_CMD(RP_CMD_TYPE_HUB,0x7)) // 0xa7
#define RP_CMD_ID_HUB_DISP_TXT_SCR (MAKE_CMD(RP_CMD_TYPE_HUB,0x8)) // 0xa8
#define RP_CMD_ID_HUB_LGT_ON_HSV (MAKE_CMD(RP_CMD_TYPE_HUB,0x0a)) // 0xaa
#define RP_CMD_ID_HUB_LGT_ON_COL (MAKE_CMD(RP_CMD_TYPE_HUB,0x0b)) // 0xab
#define RP_CMD_ID_HUB_LGT_OFF (MAKE_CMD(RP_CMD_TYPE_HUB,0x0c)) // 0xac
#define RP_CMD_ID_HUB_SPK_SET_VOL (MAKE_CMD(RP_CMD_TYPE_HUB,0x11)) // 0xb1
#define RP_CMD_ID_HUB_SPK_PLY_TON (MAKE_CMD(RP_CMD_TYPE_HUB,0x12)) // 0xb2
#define RP_CMD_ID_HUB_SPK_STP (MAKE_CMD(RP_CMD_TYPE_HUB,0x13)) // 0xb3

/* Hub cmd protocol
  RP_CMD_ID_HUB_DISP_PIX
  data[0] : row (uint8_t) 
  data[1] : column (uint8_t) 
  data[2] : brightness (uint8_t)
*/
#define RP_HUB_DISP_PIX_INDEX_ROW (0)
#define RP_HUB_DISP_PIX_INDEX_COL (1)
#define RP_HUB_DISP_PIX_INDEX_BRT (2)

/* Hub cmd protocol
  RP_CMD_ID_HUB_DISP_TXT
  data[0]-[3] : on (uint32_t) 
  data[4]-[7]: off (uint32_t)
  data[8]- : text(null terminate) 
*/
#define RP_HUB_DISP_TXT_INDEX_ON (0)
#define RP_HUB_DISP_TXT_INDEX_OFF (4)
#define RP_HUB_DISP_TXT_INDEX_TXT (8)

/* Hub cmd protocol
  RP_CMD_ID_HUB_DISP_TXT_SCR
  data[0]-[3] : delay (uint32_t) 
  data[4]- text(null terminate) 
*/
#define RP_HUB_DISP_TXT_SCR_INDEX_DLY (0)
#define RP_HUB_DISP_TXT_SCR_INDEX_TXT (4)

/* Hub cmd protocol
  RP_CMD_ID_HUB_SPK_PLY_TON
  data[0]-[3] : duration (int32_t)
  data[4]-[5] : frequency (uint16_t)
*/
#define RP_HUB_SPK_PLY_TON_INDEX_DUR (0)
#define RP_HUB_SPK_PLY_TON_INDEX_FRQ (4)

/* In SPIKE-RT, ID port is defined as 'A' to 'F'. 
   RasPike treat them as 0 to 5 
   PORT_TO_RASPIKE and PORT_FROM_RASPIKE are used for this conversion.*/
#define PORT_TO_RASPIKE(port) ((port)-'A')   
#define PORT_FROM_RASPIKE(raspike_port) ((raspike_port)+'A')   

#define RASPIKE_PORT_DATA_SIZE (12)


typedef unsigned char RasPikePort;


typedef struct {
    RasPikePort port;
    unsigned char cmd;
    unsigned char _paddig[2];
    unsigned char data[RASPIKE_PORT_DATA_SIZE];
} RPProtocolPortStatus;


typedef struct _rp_protocol_spike_status {
    uint16_t voltage;
    uint16_t current;
    float acceleration[3];
    float angular_velocity[3];
    uint32_t button;
    RPProtocolPortStatus ports[6];
} RPProtocolSpikeStatus;

extern RPProtocolSpikeStatus *raspike_prot_get_saved_status(void);

#define RP_PROTOCOL_BUFMAX (256)



#endif // _RASPIKE_PROTOCOL_H
