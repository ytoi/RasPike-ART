
#include "raspike_com.h"
#include "raspike_reeiver.h"
#include "raspike_protocol.h"


int raspike_receive_and_process_status(RPComDescriptor *desc,RPProtocolSpikeStatus *status);
{
  unsigned char buf[255];
  int len;
  
  // Check Start Keyword
  while (1) {
    len = raspike_com_receive(desc,buf,1);
    if ( len == 1 && buf[0] == RP_CMD_START ) {
      break;
    }
  }

  // Check Command ID
  
  
  
