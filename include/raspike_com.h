#ifndef _RASPIKE_COM_H_
#define _RASPIKE_COM_H_

#ifdef __cplusplus
extern "C" {
#endif

  struct _RPComDescriptor;
  typedef struct _RPComDescriptor RPComDescriptor;


  RPComDescriptor *raspike_open_usb_communication(const char *device_name);


  extern int raspike_com_send(RPComDescriptor *desc, const char *buf, int size);
  extern int raspike_com_receive(RPComDescriptor *desc, char *buf, int size);
  extern int raspike_com_close(RPComDescriptor *desc);
  extern int raspike_com_flush(RPComDescriptor *desc);  
  

#ifdef __cplsplus
}
#endif
#endif
