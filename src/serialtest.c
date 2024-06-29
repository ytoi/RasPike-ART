#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include "raspike_protocol.h"

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

int main(void)
{

  struct termios tio;                 // シリアル通信設定  
  int device_fd = open(RASPIKE_COM_NAME,O_RDWR|O_NOCTTY);

  printf("size=%d\n",sizeof(RPProtocolHubStatus));
  
  if ( device_fd < 0 ) {
    printf("Device not found errno = %d\n",errno);
    exit(-1);
  }

  printf("Serial Opened fd=%d\n",device_fd);

  bzero(&tio, sizeof(tio));
  //  tcgetattr(device_fd,&tio);
  tio.c_cflag =  (B115200 | CS8 | CLOCAL | CREAD);
  tio.c_cc[VMIN] = 0;
  tio.c_cc[VTIME] = 0;
  tcflush(device_fd, TCIFLUSH);
  tcsetattr(device_fd,TCSANOW,&tio);

  struct timespec before;
  struct timespec now;
  unsigned char buf[256];
  memset(buf,0,sizeof(buf));
  int ms;
  int err;
  unsigned char start_command[] = {0x1};
  char c=0;
  RPProtocolHubStatus hub_status;

  
  while(1) {


    clock_gettime(CLOCK_MONOTONIC_RAW,&before);    
    write(device_fd,&c,1);
    // 0-99の数字を送って、SPIKEからは+1された値が帰ってくるのを期待
    //    err = write(device_fd,&c,1);
    
    while(1) {
      int len = read(device_fd,buf,2);
      if ( len > 0 ) {
	if ( buf[0] == 0xea && buf[1] == 0x1 ) {
	  len = read(device_fd,&hub_status,33);
	  printf("Voltage = %d\n",hub_status.voltage);
	  printf("Current = %d\n",hub_status.current);
	  printf("acc x=%f y=%f z=%f\n",hub_status.acceleration[0],hub_status.acceleration[1],hub_status.acceleration[2]);
	  printf("ang x=%f y=%f z=%f\n",hub_status.angular_velocity[0],hub_status.angular_velocity[1],hub_status.angular_velocity[2]);
	  printf("Button = %d\n",hub_status.button);
	  break;
	} else {
	  printf("bu[0]=%x buf[1]=%x\n",buf[0],buf[1]);
	}
      } else {
	// readで0が返ってくるとここになる
	//printf("len=%d\n",len);
      }
    }
    clock_gettime(CLOCK_MONOTONIC_RAW,&now);    

    if ( now.tv_nsec < before.tv_nsec ) {
      now.tv_nsec += 1000000000;
      now.tv_sec--;
    }
    ms = (now.tv_sec-before.tv_sec)*1000+(now.tv_nsec - before.tv_nsec)/1000000;
    printf("spend %d ms\n",ms);

    struct timespec ts={0,100*1000*1000};
    nanosleep(&ts,0);
    c++;
    // 1秒待つ
    //sleep(1);
    
  }
}
  
