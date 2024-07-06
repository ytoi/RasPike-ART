#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include "color.h"
#include "raspike_protocol_api.h"
#include "raspike_protocol_com.h"
#include "raspike_internal.h"
#include "error.h"
#include "button.h"
#include "battery.h"
#include "speaker.h"

/* Battery */

uint16_t hub_battery_get_voltage(void)
{
  RPProtocolSpikeStatus *status = raspike_prot_get_saved_status();
  return status->voltage;
  
}

uint16_t hub_battery_get_current(void)
{
  RPProtocolSpikeStatus *status = raspike_prot_get_saved_status();
  return status->current;


}

/* Button */
pbio_error_t hub_button_is_pressed(hub_button_t *pressed)
{
  RPProtocolSpikeStatus *status = raspike_prot_get_saved_status();  
  *pressed = status->button;
  return PBIO_SUCCESS;
}


/* Display */
pbio_error_t hub_display_orientation(uint8_t up)
{
  uint8_t data[1];
  data[0] = up;
  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_DISP_ORI,data,sizeof(data));
  return PBIO_SUCCESS;

}

pbio_error_t hub_display_off(void)
{
  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_DISP_ORI,0,0);

  return PBIO_SUCCESS;
}


pbio_error_t hub_display_pixel(uint8_t row, uint8_t column, uint8_t brightness)
{
  // TODO: error check
  unsigned char data[3];
  data[0] = row;
  data[1] = column;
  data[2] = brightness;
  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_DISP_PIX,data,sizeof(data));

  return PBIO_SUCCESS;
}


pbio_error_t hub_display_image(uint8_t* image)
{
  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_DISP_IMG,(unsigned char*)image,25);
  return PBIO_SUCCESS;
}


pbio_error_t hub_display_number(const int8_t num)
{
  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_DISP_NUM,(unsigned char*)&num,1);
  return PBIO_SUCCESS;
}

pbio_error_t hub_display_char(const char c)
{
  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_DISP_CHR,(unsigned char*)&c,1);
  return PBIO_SUCCESS;
}

pbio_error_t hub_display_text(const char* text, uint32_t on, uint32_t off)
{
  // Max 256 char for text
  unsigned char data[4+4+256];
  int len = strlen(text);
  strncpy((char*)data+RP_HUB_DISP_TXT_INDEX_TXT,text,256);
  // Force null terminate
  data[sizeof(data)-1] = 0;

  *(uint32_t*)(data+RP_HUB_DISP_TXT_INDEX_ON) = on;
  *(uint32_t*)(data+RP_HUB_DISP_TXT_INDEX_OFF) = off;  

  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_DISP_TXT,data,4+4+len+1);

  return PBIO_SUCCESS;
}

pbio_error_t hub_display_text_scroll(const char* text, uint32_t delay)
{
  // Max 256 char for text
  unsigned char data[4+256];
  int len = strlen(text);
  strncpy((char*)data+RP_HUB_DISP_TXT_SCR_INDEX_TXT,text,256);
  // Force null terminate even if text is over 256 char
  data[sizeof(data)-1] = 0;

  *(uint32_t*)(data+RP_HUB_DISP_TXT_SCR_INDEX_DLY) = delay;

  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_DISP_TXT_SCR,data,4+len+1);

  return PBIO_SUCCESS;
}


/* IMU */

pbio_error_t hub_imu_init(void)
{
  /* hub_imu_init is automatically called on spike.
     so, just return OK */

  return PBIO_SUCCESS;
}

void hub_imu_get_acceleration(float accel[3])
{
  RPProtocolSpikeStatus *status = raspike_prot_get_saved_status();
  memcpy(accel,status->acceleration,sizeof(float)*3);
}

void hub_imu_get_angular_velocity(float angv[3])
{
  RPProtocolSpikeStatus *status = raspike_prot_get_saved_status();
  memcpy(angv,status->angular_velocity,sizeof(float)*3);
}

float hub_imu_get_temperature(void)
{
  // TODO:
  // Not Supported
  return 0.0;
}
  
/* Light */
pbio_error_t hub_light_on_hsv(const pbio_color_hsv_t *hsv)
{
  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_LGT_ON_HSV,(unsigned char*)hsv,sizeof(*hsv));

  return PBIO_SUCCESS;
}

pbio_error_t hub_light_on_color(pbio_color_t color)
{
  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_LGT_ON_COL,(unsigned char*)&color,sizeof(color));

  return PBIO_SUCCESS;
}

pbio_error_t hub_light_off(void)
{
  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_LGT_OFF,0,0);

  return PBIO_SUCCESS;
}

void hub_speaker_set_volume(uint8_t volume)
{
  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_SPK_SET_VOL,(unsigned char*)&volume,sizeof(volume));
}

void hub_speaker_play_tone(uint16_t frequency, int32_t duration)
{
  unsigned char data[6];
  // *(int32_t*)(data+RP_HUB_SPK_PLY_TON_INDEX_DUR) = duration;
  /* always use MANUAL_STOP */
  *(int32_t*)(data+RP_HUB_SPK_PLY_TON_INDEX_DUR) = SOUND_MANUAL_STOP;
  *(uint16_t*)(data+RP_HUB_SPK_PLY_TON_INDEX_FRQ) = frequency;
  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_SPK_PLY_TON,data,sizeof(data));

  if ( duration != SOUND_MANUAL_STOP ) {
    // Wait for duration
    struct timespec req;
    struct timespec rem;
    req.tv_sec = duration / 1000;
    req.tv_nsec = (duration % 1000 ) * 1000000;
    int ret;
    while(1) {
      ret = nanosleep(&req,&rem);
      if ( ret == 0 || errno != EINTR ) break;
      req = rem;
    }
      
  }
}
  
  
void hub_speaker_stop(void)
{
  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_HUB_SPK_STP,0,0);
}

  
/* System Command */
void hub_system_shutdown(void)
{
  raspike_prot_send(RP_PORT_NONE,RP_CMD_ID_SHT_DWN,0,0);
  raspike_prot_shutdown();
}

  
