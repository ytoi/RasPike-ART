#include <errno.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include "raspike_com.h"
#include <pthread.h>
#define printf(...) do { printf(__VA_ARGS__);fflush(stdout);}while(0)

typedef int (*RPComSendFunc)(RPComDescriptor *desc, const unsigned char *buf, int size);
typedef int (*RPComReceiveFunc)(RPComDescriptor *desc, unsigned char *buf, int size);
typedef int (*RPComCloseFunc)(RPComDescriptor *desc);
typedef int (*RPComFlushFunc)(RPComDescriptor *desc);

struct _RPComDescriptor {
  int type;
  int desc;
  RPComSendFunc send;
  RPComReceiveFunc receive;
  RPComCloseFunc close;
  RPComFlushFunc flush;
};
			      
#define RP_COM_TYPE_USB (1)




/** USB Functions */
static int get_usb_fd(RPComDescriptor *desc)
{
  if ( desc->type == RP_COM_TYPE_USB ) {
    return (int)desc->desc;
  }
  printf("Not USB Descriptor! type = %d(expected=%d)\n",
	 desc->type,RP_COM_TYPE_USB);
  exit(-1);
}

/* Buffered write */
static char fg_send_buffer[1024];
static int current_index = 0;
static pthread_mutex_t fg_send_mutex = PTHREAD_MUTEX_INITIALIZER;
static int fg_send_mode = RASPIKE_USB_MODE_NORMAL;

void raspike_usb_buffer_flush(RPComDescriptor *desc)
{
  pthread_mutex_lock(&fg_send_mutex);
  int len;
  if ( fg_send_mode == RASPIKE_USB_MODE_BUFFERED ) {
    /* for mesuring */
#define DO_MEASURING 0
#if DO_MEASURING == 1
    static struct timespec prev = {0};
    struct timespec cur,cur2;

    clock_gettime(CLOCK_MONOTONIC,&cur);
#endif
    
    len = write(get_usb_fd(desc),fg_send_buffer,current_index);

#if DO_MEASURING == 1    
    clock_gettime(CLOCK_MONOTONIC,&cur2);
    int cur_time = cur.tv_sec*1000+cur.tv_nsec/1000000;
    int diff = (cur.tv_sec-prev.tv_sec)*1000+(cur.tv_nsec-prev.tv_nsec)/1000000;
    int write_diff =  (cur2.tv_sec-cur.tv_sec)*1000+(cur2.tv_nsec-cur.tv_nsec)/1000000;

    prev = cur;
    printf("Flush time=%d write_diff=%d diff=%d\n",cur_time,write_diff,diff);
#endif    
    current_index = 0;
  }
  pthread_mutex_unlock(&fg_send_mutex);
}  
extern void raspike_usb_set_mode(int mode)
{
  fg_send_mode = mode;
}

static int buffered_write(int fd, const char *buf, size_t size)
{
  pthread_mutex_lock(&fg_send_mutex);
  if ( current_index + size > sizeof(fg_send_buffer) ) {
    printf("Send Buffer Over! size=%d current_index=%d\n", size,current_index);
    errno = ENOMEM;
    pthread_mutex_unlock(&fg_send_mutex);
    return -1;
  }

  memcpy(fg_send_buffer+current_index,buf,size);
  current_index+=size;

  pthread_mutex_unlock(&fg_send_mutex);
  return size; // success
}


  /* this function is not thread safe. calling this function in serial is application's responsibirity */
static int rp_usb_send(RPComDescriptor *desc, const unsigned char *buf, int size)
{
  int fd = get_usb_fd(desc);

  int len;
  do {
    /* for mesuring
       struct timespec prev = {0};
       struct timespec cur;
    
       clock_gettime(CLOCK_MONOTONIC,&prev);
    */
    if ( fg_send_mode == RASPIKE_USB_MODE_BUFFERED ) {
      len = buffered_write(fd,buf,size);
    } else {
      len = write(fd,buf,size);
    } 

    /*
      clock_gettime(CLOCK_MONOTONIC,&cur);
      int diff = (cur.tv_sec-prev.tv_sec)*1000+(cur.tv_nsec-prev.tv_nsec)/1000000;
      printf("Send spends=%d size=%d\n",diff,size);
    */
    if ( len >=0 || errno != EAGAIN || errno != EINTR ){
      break;
    }
  } while(1);

  //  fflush(fp);
  return len;
}
    
static int rp_usb_receive(RPComDescriptor *desc, unsigned char *buf, int size)
{
  int fd = get_usb_fd(desc);

  int len;
  do {
    len = read(fd,buf,size);
    //len = fread(buf,size,1,fp);
    if ( len >= 0 || errno != EAGAIN || errno != EINTR ) {
      break;
    }
  } while(1);
  return len;
}

static int rp_usb_close(RPComDescriptor *desc)
{
  int fd = get_usb_fd(desc);
  return close(fd);
}

static int rp_usb_flush(RPComDescriptor *desc)
{
  int fd = get_usb_fd(desc);
  return fsync(fd);
}


RPComDescriptor *raspike_open_usb_communication(const char *device_name)
{
  struct termios tio;                 // シリアル通信設定  
  /*
  FILE *fp = fopen(device_name,"rw");
  if ( fp == 0 ) {
	printf("Device not found errno = %d\n",errno);
	return 0;
  }
  
  int fd = fileno(fp);
  */
  int fd = open(device_name,O_RDWR|O_NOCTTY);

  if ( fd < 0 ) {
	printf("Device not found errno = %d\n",errno);
	return 0;
  }

  bzero(&tio, sizeof(tio));
  //  tcgetattr(fd,&tio);
  tio.c_cflag =  (B115200 | CS8 | CLOCAL | CREAD);
  tio.c_cc[VMIN] = 0; /* 最低1文字くるまでロックさせる*/
  tio.c_cc[VTIME] = 0;
  tcflush(fd, TCIOFLUSH);
  tcsetattr(fd,TCSANOW,&tio);

  RPComDescriptor *desc = malloc(sizeof(RPComDescriptor));
  memset(desc,0,sizeof(RPComDescriptor));

  desc->type = RP_COM_TYPE_USB;
  desc->desc = fd;
  desc->send = rp_usb_send;
  desc->receive = rp_usb_receive;
  desc->close = rp_usb_close;
  desc->flush = rp_usb_flush;
  
  return desc;
}
  

void dump_buf(const unsigned char *buf, int size)
{
  if ( buf && size>0 ) {
    int i;
    printf("send-----\n");
    for ( i = 0; i < size; i++ ) {
      printf("%x ",buf[i]);
      if ( i >0 && i % 8 == 0 ) printf("\n");
    }
    printf("\n");
  }
}

void dump_buf2(int wait_size,const unsigned char *buf, int size)
{
  if ( buf && size>0 ) {
    int i;
    printf("receive-----(%d)\n",wait_size);
    for ( i = 0; i < size; i++ ) {
      printf("%x ",buf[i]);
      if ( i >0 && i % 8 == 0 ) printf("\n");
    }
    printf("\n");
  }

}


int raspike_com_send(RPComDescriptor *desc, const unsigned char *buf, int size)
{
  //  dump_buf(buf,size); 
  int ret =  desc->send(desc,buf,size);
  if ( ret < 0 ) {
    printf("com_send error err=%d\n",errno);
  }
  return ret;
}


int raspike_com_receive(RPComDescriptor *desc, unsigned char *buf, int size)
{
  int num = 0;
  int len = 0;
  unsigned char *p = buf;
  
  while (num != size) {
    len = desc->receive(desc,buf+num,size-num);

    if ( len < 0 ) {
      if ( len < 0 ) {
	printf("com_receive error err=%d\n",errno);
      }

      return len;
    }
    num+=len;
    //    dump_buf2(size,buf,num);
  }
  //  printf("com_receive return\n");
  return num;
}
  
int raspike_com_close(RPComDescriptor *desc)
{
  return desc->close(desc);
}

int raspike_com_flush(RPComDescriptor *desc)
{
  if ( fg_send_mode == RASPIKE_USB_MODE_NORMAL ) {
    return desc->flush(desc);
  } else {
    raspike_usb_buffer_flush(desc);
  }
}
