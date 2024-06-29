#ifndef _RASPIKE_PROTOCOL_API_H_
#define _RASPIKE_PROTOCOL_API_H_

#include "raspike_com.h"

#ifdef __cplusplus
extern "C" {
#endif  

  typedef unsigned char RasPikePort;
  extern int raspike_prot_init(RPComDescriptor *desc);
  extern int raspike_prot_shutdown(void);

  
  
  extern int raspike_prot_lock_status(void);
  extern int raspike_prot_unlock_status(void);

  extern int raspike_prot_send(RasPikePort port, unsigned char cmdid, const unsigned char *buf, int size);
  extern int raspike_prot_receive(void);
  extern int raspike_wait_ack(unsigned char port, unsigned char cmd);

  extern int raspike_wait_port_cmd_change(RasPikePort port, unsigned char wait_cmd);
  extern int raspike_port_com_change_if_needed(RasPikePort port, unsigned char wait_cmd);
  
#ifdef __cplusplus
}
#endif

#endif

