

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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

#define RASPIKE_COM_NAME "/dev/ttyACM0"

void *receiver_task(void *arg)
{
  while(1) {
    raspike_prot_receive();
  }
}

void ultrasonicsensor_test(void)
{
  // Ultrasonic sensor test
  pup_device_t *us = pup_ultrasonic_sensor_get_device(PBIO_PORT_ID_B);
  sleep(1);

  pup_ultrasonic_sensor_light_set(us,100,0,0,0);
#if 0
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
  pup_motor_t *mot1 = pup_motor_get_device(PBIO_PORT_ID_B);  
  pbio_error_t err= pup_motor_setup(mot1,PUP_DIRECTION_CLOCKWISE,true);
  int i = 50;
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
  hub_speaker_play_tone(NOTE_C5,2000);
  hub_speaker_play_tone(NOTE_A5,SOUND_MANUAL_STOP);
  sleep(3);
  hub_speaker_stop();
  hub_light_on_color(PBIO_COLOR_GREEN);
  sleep(3);
  hub_system_shutdown();
}



int main(void)
{
  RPComDescriptor *desc = raspike_open_usb_communication(RASPIKE_COM_NAME);

  if ( !desc ) {
    printf("Cannot Open desc\n");
    exit(-1);
  }

  raspike_prot_init(desc,0);

  pthread_t receive_thread;
  pthread_create(&receive_thread,0,receiver_task,0);
  sleep(1);
  raspike_handshake();


  //  ultrasonic_sensor_test();
  //  motor_test();
  //display_test();
  // light_test();
  speaker_test();
  
  return 0;
  
  pup_device_t *col;// = pup_color_sensor_get_device(PBIO_PORT_ID_B);
  //  pup_device_t *col;
  pup_motor_t *mot1;// = pup_motor_get_device(PBIO_PORT_ID_A);
  pbio_error_t err;//= pup_motor_setup(mot1,PUP_DIRECTION_CLOCKWISE,true);


  pup_motor_t *mot2 = pup_motor_get_device(PBIO_PORT_ID_E);
  pup_motor_t *mot3 = pup_motor_get_device(PBIO_PORT_ID_D);
  
  int i = 50;





  
  err= pup_motor_setup(mot2,PUP_DIRECTION_COUNTERCLOCKWISE,true);
  err= pup_motor_setup(mot3,PUP_DIRECTION_COUNTERCLOCKWISE,true);
  while ( i < 100 ) {
    pup_motor_set_speed(mot1,i+400);
    printf("mot1:power=%d speed=%d count=%d isStall=%d\n",
	   pup_motor_get_power(mot1),
	   pup_motor_get_speed(mot1),
	   pup_motor_get_count(mot1),
	   pup_motor_is_stalled(mot1));

    pup_motor_set_speed(mot2,i+400);
    printf("mot2power=%d speed=%d count=%d isStall=%d\n",
	   pup_motor_get_power(mot2),
	   pup_motor_get_speed(mot2),
	   pup_motor_get_count(mot2),
	   pup_motor_is_stalled(mot2));

    pup_motor_set_speed(mot3,i+400);
    printf("mot3power=%d speed=%d count=%d isStall=%d\n",
	   pup_motor_get_power(mot3),
	   pup_motor_get_speed(mot3),
	   pup_motor_get_count(mot3),
	   pup_motor_is_stalled(mot3));

    pup_color_rgb_t rgb = pup_color_sensor_rgb(col);
    printf("[RGB]r=%d g=%d b=%d\n",rgb.r,rgb.g,rgb.b);


    i++;
    sleep(1);
  }

  pup_motor_brake(mot1);
  //  pup_motor_brake(mot2);
  //  pup_motor_brake(mot3);  
  return 0;

  

  int c = 0;
  while (c < 5) {
    pup_color_rgb_t rgb = pup_color_sensor_rgb(col);
    printf("[RGB]r=%d g=%d b=%d\n",rgb.r,rgb.g,rgb.b);
    sleep(1);
    c++;
  }


  while (c < 10) {
    pup_color_hsv_t hsv = pup_color_sensor_hsv(col,false);
    printf("[HSV:OFF]h=%d s=%d v=%d\n",hsv.h,hsv.s,hsv.v);
    sleep(1);
    c++;
  }

  while (c < 15) {
    pup_color_hsv_t hsv = pup_color_sensor_hsv(col,true);
    printf("[HSV:ON]h=%d s=%d v=%d\n",hsv.h,hsv.s,hsv.v);
    sleep(1);
    c++;
  }


  
}
