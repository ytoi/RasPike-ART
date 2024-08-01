#ifndef _RASPIKE_PROTOCOL_API_H_
#define _RASPIKE_PROTOCOL_API_H_

#include "raspike_com.h"

#ifdef __cplusplus
extern "C" {
#endif  

  extern int raspike_prot_init(RPComDescriptor *desc);
  extern int raspike_prot_receive(void);
  extern int raspike_prot_shutdown(void);
  
#ifdef __cplusplus
}
#endif

#endif

