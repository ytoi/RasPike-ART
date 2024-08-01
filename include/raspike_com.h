#ifndef _RASPIKE_COM_H_
#define _RASPIKE_COM_H_

#ifdef __cplusplus
extern "C" {
#endif

  struct _RPComDescriptor;
  typedef struct _RPComDescriptor RPComDescriptor;


  RPComDescriptor *raspike_open_usb_communication(const char *device_name);
  extern void raspike_usb_buffer_flush(RPComDescriptor *desc);
  enum {
    RASPIKE_USB_MODE_NORMAL = 0,
    RASPIKE_USB_MODE_BUFFERED = 1
  };
  extern void raspike_usb_set_mode(int mode);  
  

  extern int raspike_com_send(RPComDescriptor *desc, const unsigned char *buf, int size);
  extern int raspike_com_receive(RPComDescriptor *desc, unsigned char *buf, int size);
  extern int raspike_com_close(RPComDescriptor *desc);
  extern int raspike_com_flush(RPComDescriptor *desc);  
  

  
#ifdef __cplsplus
}
#endif
#endif
