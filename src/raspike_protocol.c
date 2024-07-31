#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include "raspike_protocol_api.h"
#include "raspike_protocol_com.h"
#include "raspike_internal.h"

#define printf(...) do { printf(__VA_ARGS__);fflush(stdout);}while(0)

//#define CHECK_FREQUENCY 1


static RPProtocolSpikeStatus *fgStatus;
static RPComDescriptor *fgDesc;

static pthread_mutex_t fgStatusMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t fgStatusCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t fgSendMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t fgSendCond = PTHREAD_COND_INITIALIZER;

#define PORT_HUB (6)
RPPortDevice gPortDevices[7]; // [6] is for hub

/* Mutex/Condition Variables
   Note:
      ASP Sim uses signal USR1 as event trigger.
      It causes waiting functions resume with error.
      Receive tasks avoid it by setting signal mask not to receive USR1.
      But sender task (application task) cannot set the signal mask,
      use retry version when it catches the error by signal.
*/

static int raspike_mutex_lock(pthread_mutex_t *mutex)
{
  int ret;
  do {
    if ( (ret = pthread_mutex_lock(mutex)) == 0 ) {
      break;
    }
  } while ( errno == EINTR );
  return ret;
}

static int raspike_mutex_unlock(pthread_mutex_t *mutex)
{
  return pthread_mutex_unlock(mutex);
}

static int raspike_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
  int ret;
  do {
    if ( (ret = pthread_cond_wait(cond,mutex)) == 0 ) {
      break;
    }
  } while ( errno == EINTR );

  return ret;
}

static int raspike_cond_signal(pthread_cond_t *cond)
{
  return pthread_cond_signal(cond);
}

static int raspike_cond_broadcast(pthread_cond_t *cond)
{
  return pthread_cond_broadcast(cond);
}


#define IS_VALID_PORT(port) (((0<=(port)) && ((port) <=6)) || (port==RP_PORT_NONE))
#undef ENSURE_VALID_PORT
#define ENSURE_VALID_PORT(port) if(!IS_VALID_PORT(port)) do { /*TODO:error*/ } while(0)




static void init_port_device(RPPortDevice *dev)
{
  memset(dev,0,sizeof(*dev));
  pthread_cond_init(&dev->cond,0);
  pthread_mutex_init(&dev->mutex,0);
  dev->ack_cmd = 0;
  dev->ack_data = 0;
}

RPPortDevice *getDevice(RasPikePort port)
{
  if ( port == RP_PORT_NONE ) {
    // This is hub device. use special index
    port = PORT_HUB;
  }
  return gPortDevices+port;
}

static int did_handshake = 0;
static int stop_communication = 0;

static unsigned char version_major = 0;
static unsigned char version_minor = 0;
static unsigned char version_patch = 0;

int raspike_prot_init(RPComDescriptor *desc)
{
  if ( desc == 0 || fgDesc) {
    return -1;
  }
  fgDesc = desc;
  
  fgStatus = malloc(sizeof(*fgStatus));

  for (int i=0; i < sizeof(gPortDevices)/sizeof(gPortDevices[0]); i++ ) {
    init_port_device(gPortDevices+i);
  }


  // Hand Shake
  unsigned char cmd = RP_CMD_INIT;
  unsigned char buf[4];
  int len;
  
  buf[0] = cmd;
  buf[1] = RP_CMD_INIT_MAGIC; // Magic word
  raspike_com_send(fgDesc,buf,2);
  
  
  // skip until INIT
  while(1) {
    len = raspike_com_receive(fgDesc,buf,1);
    if ( buf[0] == RP_CMD_INIT ) {
      //      printf("Got Init \n");
      len = raspike_com_receive(fgDesc,buf,1);
      if ( buf[0] == RP_CMD_INIT_MAGIC ) {
	//	printf("Got Init magic\n");
	did_handshake = 1;
	break;
      }
    }
    // TODO: Retry send and check Timeout 
  }

  len = raspike_com_receive(fgDesc,buf,3);  
  version_major = buf[0];
  version_minor = buf[1];  
  version_patch = buf[2];  

  printf("SPIKE asp.bin version=%d.%d.%d expected=%d.%d.%d\n", version_major,version_minor,version_patch,
	 SPIKE_EXPECTED_VERSION_MAJOR,SPIKE_EXPECTED_VERSION_MINOR,SPIKE_EXPECTED_VERSION_PATCH);

  if ( version_major != SPIKE_EXPECTED_VERSION_MAJOR ||
       version_minor != SPIKE_EXPECTED_VERSION_MINOR ||
       version_patch != SPIKE_EXPECTED_VERSION_PATCH ) {
    printf("SPIKE version mismatched! update asp.bin\n");
    return -1;
  }
	   
  
  return 0;
}

int raspike_prot_shutdown(void)
{
  stop_communication = 0;
  raspike_com_close(fgDesc);
  return 0;
}


RPProtocolSpikeStatus *raspike_prot_get_saved_status(void)
{
  return fgStatus;
}



int raspike_prot_send(RasPikePort port, unsigned char cmdid, const unsigned char *buf, int size)
{
  unsigned char cmd[128];
  cmd[0] = RP_CMD_START;
  cmd[1] = cmdid;
  cmd[2] = size;
  cmd[3] = port;

  int len = 4;
  
  if ( size > 0 ) {
    memcpy(cmd+4,buf,size);
    len += size;
  }
  
  // Ensure message is packed
  raspike_mutex_lock(&fgSendMutex);
  /*
  int len = raspike_com_send(fgDesc,cmd,sizeof(cmd));
  if ( len == 4 ) {
    if ( size > 0 ) {
      len = raspike_com_send(fgDesc, buf, size);
    }
  } else {
    len = -1;
  }
  */
  raspike_com_send(fgDesc,cmd,len);
  raspike_com_flush(fgDesc);
  raspike_mutex_unlock(&fgSendMutex);

  return len;
}

static int process_command(RasPikePort port, unsigned char cmd, unsigned char *buf, int size)
{
  int ret = 0;
  
  switch(cmd) {
  case RP_CMD_ID_ALL_STATUS:
    raspike_mutex_lock(&fgStatusMutex);
    //    printf("status=%d size=%d len=%d\n",fgStatus,size,sizeof(*fgStatus));
    memcpy(fgStatus,buf,sizeof(*fgStatus));
    raspike_cond_broadcast(&fgStatusCond);
    raspike_mutex_unlock(&fgStatusMutex);
    break;
  case RP_CMD_ID_ACK:
    {
      RPPortDevice *dev = getDevice(port);
      pthread_mutex_lock(&dev->mutex);
      // ack has cmd and data
      int32_t *p = (int32_t*)buf;
      dev->ack_cmd = *p;
      dev->ack_data = *(p+1);
      // signal to the waiting task
      printf("Ack Received cmd=%d data=%d\n",dev->ack_cmd,dev->ack_data);
      raspike_cond_signal(&dev->cond);
      raspike_mutex_unlock(&dev->mutex);
    }
      break;
  default:
    ret = -1;
    break;
  }
  return ret;
}

  
int raspike_prot_receive(void)
{
  
  
  unsigned char buf[RP_PROTOCOL_BUFMAX] = {0};
  int len;

  if ( stop_communication ) {
    return -1;
  }

  // Wait Start command
  while(1) {
    len = raspike_com_receive(fgDesc,buf,1);
    //    printf("RASPI] STARTCHECK=%d\n",buf[0]);
    if ( len > 0 ) {
      if ( buf[0] == RP_CMD_START ) {
	break;
      }
    } else if ( len < 0 ) {
      return len;
    }
  }
  // Get Command ID
  len = raspike_com_receive(fgDesc,buf,3);

  if ( len <= 0 ) return -1;

  unsigned char cmd = buf[0];
  int size = buf[1];
  RasPikePort port = buf[2];

  //  printf("RASPI]cmd=%d size=%d port=%d\n",cmd,size,port);
  
  
  if ( size > 0 ) {
    len = raspike_com_receive(fgDesc,buf,size);
  }

  if ( len <= 0 ) return -1;

#ifdef CHECK_FREQUENCY    
  static struct timespec prev = {0};
  static int count = 0;

  if ( prev.tv_sec ==0 && prev.tv_nsec == 0 ) {
    clock_gettime(CLOCK_MONOTONIC,&prev);
  }

  count++;

  if (count == 200 ) {
    struct timespec cur;
    clock_gettime(CLOCK_MONOTONIC,&cur);
    long diff;
    if ( prev.tv_nsec > cur.tv_nsec ) {
      cur.tv_sec--;
      cur.tv_nsec += 1000000000;
    }
    diff = (cur.tv_sec-prev.tv_sec)*1000000000 + cur.tv_nsec - prev.tv_nsec;
    float freq = diff / 200 / 1000000;
    printf("freq = %f /s\n",freq);
    prev=cur;
    count=0;
  }
#endif  
  
  return process_command(port,cmd,buf,size);

}

int raspike_wait_ack(RasPikePort port, unsigned char cmd) {
  ENSURE_VALID_PORT(port);
  
  int32_t ret;
  RPPortDevice *dev = getDevice(port);
  
  raspike_mutex_lock(&dev->mutex);
  while(1) {
    if ( dev->ack_cmd == cmd )
      break;
    raspike_cond_wait(&dev->cond,&dev->mutex);
  }
  // reset ack
  dev->ack_cmd = 0;
  ret = dev->ack_data;
  raspike_mutex_unlock(&dev->mutex);

  return ret;
}

int raspike_wait_port_cmd_change(RasPikePort port, unsigned char wait_cmd)
{
  RPProtocolPortStatus *p = &fgStatus->ports[port];

  raspike_mutex_lock(&fgStatusMutex);
  while (1) {
    if ( p->cmd == wait_cmd ) {
      break;
    }
    raspike_cond_wait(&fgStatusCond,&fgStatusMutex);
  }
  raspike_mutex_unlock(&fgStatusMutex);

  return 0;
}

int raspike_port_com_change_if_needed(RasPikePort port, unsigned char wait_cmd)
{
  RPProtocolPortStatus *p = &fgStatus->ports[port];
  if ( p->cmd == wait_cmd ) {
    // mode is same, no action
    return 0;
  }

  // Mode change is needed. send command
  raspike_prot_send(port,wait_cmd,0,0);

  raspike_wait_port_cmd_change(port,wait_cmd);

  return 0;
}
