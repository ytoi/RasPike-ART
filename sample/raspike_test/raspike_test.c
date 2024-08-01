

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "raspike_com.h"
#include "raspike_protocol_api.h"

#include "spike/pup/motor.h"
#include "spike/pup/colorsensor.h"
#include "spike/pup/forcesensor.h"
#include "spike/pup/ultrasonicsensor.h"
#include "spike/hub/system.h"
#include "spike/hub/light.h"
#include "spike/hub/imu.h"
#include "spike/hub/display.h"
#include "spike/hub/button.h"
#include "spike/hub/battery.h"


#define RASPIKE_COM_NAME "/dev/USB_SPIKE"

void *receiver_task(void *arg)
{
  while(1) {
    raspike_prot_receive();
  }
}

void ultrasonicsensor_test(void)
{
  // Ultrasonic sensor test
  pup_device_t *us = pup_ultrasonic_sensor_get_device(PBIO_PORT_ID_F);
  sleep(1);

  pup_ultrasonic_sensor_light_set(us,100,0,0,0);
#if 0
  // 超音波センサーのライトを点けるとハングするので、コメントアウトする
  sleep(1);
  pup_ultrasonic_sensor_light_set(us,0,50,0,0);
  sleep(1);
  pup_ultrasonic_sensor_light_set(us,0,0,50,0);  
  sleep(1);
  pup_ultrasonic_sensor_light_set(us,0,0,0,100);  
  sleep(1);
  pup_ultrasonic_sensor_light_set(us,100,100,100,100);  
  sleep(1);
  pup_ultrasonic_sensor_light_off(us);
#endif
  while ( 1 ) {
    printf("US: Distance = %d, presence =%d\n",
	   pup_ultrasonic_sensor_distance(us),
	   pup_ultrasonic_sensor_presence(us));
    sleep(1);
  }
}  

void motor_test(void)
{
  pup_motor_t *mot1 = pup_motor_get_device(PBIO_PORT_ID_A);  
  pup_motor_t *mot2 = pup_motor_get_device(PBIO_PORT_ID_E);
  pup_motor_t *mot3 = pup_motor_get_device(PBIO_PORT_ID_B);  
  pbio_error_t err= pup_motor_setup(mot1,PUP_DIRECTION_CLOCKWISE,true);
  err= pup_motor_setup(mot2,PUP_DIRECTION_CLOCKWISE,true);
  err= pup_motor_setup(mot3,PUP_DIRECTION_CLOCKWISE,true);
  int i = 50;

  while ( 1 ) {
    int power = i%50+30;
    pup_motor_set_power(mot1,power);
    pup_motor_set_power(mot2,power);    
    pup_motor_set_power(mot3,power);  
    usleep(10*1000);
  }
  pup_motor_set_speed(mot1,400);
  sleep(1);
  pup_motor_stop(mot1);
  sleep(1);
  pup_motor_set_speed(mot1,-400);
  sleep(1);
  pup_motor_brake(mot1);
  sleep(1);
  pup_motor_set_speed(mot1,400);  
  sleep(1);
  pup_motor_hold(mot1);
  sleep(1);
  
  while ( i < 100 ) {
    pup_motor_set_speed(mot1,i+400);
    printf("mot1:power=%d speed=%d count=%d isStall=%d\n",
	   pup_motor_get_power(mot1),
	   pup_motor_get_speed(mot1),
	   pup_motor_get_count(mot1),
	   pup_motor_is_stalled(mot1));
    sleep(1);
  }
}

void display_test(void)
{
  hub_display_number(12);
  sleep(3);

  hub_display_char('X');
  sleep(2);

  hub_display_text("Ras Pike!",1000,1000);

  hub_display_text_scroll("ETRobocon20th",500);
}

#include "color.h"
void light_test(void)
{
  pbio_color_hsv_t hsv = {30, 100, 100};
  hub_light_on_hsv(&hsv);
  sleep(2);
  hub_light_on_color(PBIO_COLOR_GREEN);
  sleep(2);
  hub_light_off();
  
}
  
#include "speaker.h"
void speaker_test(void)
{
  hub_speaker_set_volume(50);
  hub_speaker_play_tone((uint16_t)NOTE_C5,2000);
  hub_speaker_play_tone(NOTE_A5,SOUND_MANUAL_STOP);
  sleep(3);
  hub_speaker_stop();
  hub_light_on_color(PBIO_COLOR_GREEN);
  sleep(3);
  hub_system_shutdown();
}



void colorsensor_test(void)
{
  int i = 0;
  pup_device_t *col = pup_color_sensor_get_device(PBIO_PORT_ID_C);

  while ( i < 100 ) {

    pup_color_rgb_t rgb = pup_color_sensor_rgb(col);
    printf("[RGB]r=%d g=%d b=%d\n",rgb.r,rgb.g,rgb.b);

    i++;
    sleep(1);
  }

}

int main(int argc,char const *argv[])
{
  const char *p = RASPIKE_COM_NAME;
  if ( argc > 1 ) {
    p = argv[1];
  }

  RPComDescriptor *desc = raspike_open_usb_communication(p);

  if ( !desc ) {
    printf("Cannot Open desc name=%s\n",p);
    exit(-1);
  }

  raspike_prot_init(desc);

  pthread_t receive_thread;
  pthread_create(&receive_thread,0,receiver_task,0);


  //  ultrasonic_sensor_test();
  //  motor_test();
  //display_test();
  // light_test();
  //  speaker_test();
    motor_test();
  //colorsensor_test();
  
  return 0;
}
