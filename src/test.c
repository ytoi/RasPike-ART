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


#define RASPIKE_COM_NAME "/dev/ttyACM0"


int command_read(int fd,unsigned char *buf, int len)
{
  int err;
  unsigned char *p = buf;
  int left = len;
  return read(fd,p,1);
  
  while (1) {
    while(1) {
      err = read(fd,p,left);
      if ( err != 0 ) {
	break;
      } else {
	printf("read=0\n");
      }
    } 
    if ( err < 0 ) {
      printf("read error errno=%d\n",errno);
      return -1;
    }

    left = left - err;
    p+=err;
    if ( left == 0 ) {
      break;
    }

  }
  *p = 0;
  return len;
}

int main(void)
{

  struct termios tio;                 // シリアル通信設定  
  int device_fd = open(RASPIKE_COM_NAME,O_RDWR|O_NOCTTY);

  if ( device_fd < 0 ) {
    printf("Device not found errno = %d\n",errno);
    exit(-1);
  }

  printf("Serial Opened fd=%d\n",device_fd);

  bzero(&tio, sizeof(tio));
  //  tcgetattr(device_fd,&tio);
  //  tio.c_iflag = (IGNBRK|IGNPAR);
  tio.c_cflag =  (B115200 | CS8 | CLOCAL | CREAD);
  tio.c_cc[VMIN] = 0;
  tio.c_cc[VTIME] = 0;
  tcflush(device_fd, TCIFLUSH);
  tcsetattr(device_fd,TCSANOW,&tio);

  printf("lflag=0x%x\n",tio.c_lflag);
  
  const char command[] = {2,2,3,4};
  const char receive_command[] = {1};
  
  struct timespec before;
  struct timespec now;
  unsigned char buf[256];
  memset(buf,0,sizeof(buf));
  int ms;
  int err;
  
  while(1) {
    int len = command_read(device_fd,buf,1);
    if ( memcmp(buf,receive_command,1) == 0 ) {
	break;
    }
  }

  printf("Start---\n");

  unsigned char c = 0;
  while(1) {
    sleep(1);
    clock_gettime(CLOCK_MONOTONIC_RAW,&before);    
    err = write(device_fd,&c,1);
    while(1) {
      int len = command_read(device_fd,buf,1);
      if ( len > 0 ) {
	if ( buf[0] == (c+1) ) { printf("OK\n");break;}
	else if ( len != 0 ) printf("org=%d err=%d\n",c,buf[0]);
	else printf("Null\n");
      } else {
	//printf("len=%d\n",len);
      }
    }
    clock_gettime(CLOCK_MONOTONIC_RAW,&now);    

    if ( now.tv_nsec < before.tv_nsec ) {
      now.tv_nsec += 1000000000;
      now.tv_sec--;
    }
    ms = (now.tv_sec-before.tv_sec)*1000+(now.tv_nsec - before.tv_nsec)/1000000;
    c++;
    c%=98;
    
    printf("spend %d ms\n",ms);

  }
		
#if 0  
  unsigned char speed = 255;
  while(1) {  
    clock_gettime(CLOCK_MONOTONIC_RAW,&before);
    int err = write(device_fd,command,1);
    printf("speed=%d\n",speed);
    err = write(device_fd,&speed,1);
    //    sleep(1);
    while(1) {
      int len = command_read(device_fd,buf,1);
      if ( len > 0 ) {
	if (memcmp(buf,receive_command,1) == 0 ) break;
	else printf("err=%d\n",buf[0]);
      }
    }
    clock_gettime(CLOCK_MONOTONIC_RAW,&now);
    int ms = 0;

    if ( now.tv_nsec < before.tv_nsec ) {
      now.tv_nsec += 1000000;
      now.tv_sec--;
    }
    ms = (now.tv_sec-before.tv_sec)*1000+(now.tv_nsec - before.tv_nsec)/1000;

    printf("spend %d ms\n",ms);


    speed-=10;
    sleep(1);
  }
#endif
}
  
