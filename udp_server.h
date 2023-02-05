#ifndef UDP_SERVER_H_
#define UDP_SERVER_H_

#include "adc_buffer.h"

#define UDP_SERVER_MAX_MSG_LEN 1024
#define UDP_SERVER_PORT 12345
#define UDP_SERVER_COMMAND_SEP " "

void UdpServer_init(AdcBuffer* pBuffer);
void UdpServer_waitForShutdown(void);

#endif