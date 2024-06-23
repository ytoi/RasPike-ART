#ifndef _RASPIKE_RECEIVER_H_
#define _RASPIKE_RECEIVER_H_

#include "raspike_com.h"
#include "raspike_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef void (RPReceiveStatusHook)(RPProtocolSpikeStatus *before, RPProtocolSpikeStatus *after);
  int raspike_receive_and_process_status(RPComDescriptor *desc,RPProtocolSpikeStatus *status);
  int raspike_set_status_hook(RPReceiveStatusHook *hook);

#ifdef __cplusplus
}
#endif


#endif
