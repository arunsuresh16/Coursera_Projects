#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define config_SERVER_PORT		0

#define config_SERVER_ADDR0 	0
#define config_SERVER_ADDR1 	0
#define config_SERVER_ADDR2 	0
#define config_SERVER_ADDR3 	0

 /* TCP task to send the encrypted data */
void vTcpSendDataTask(void* par);

/* Actual functionality to send the data through TCP */
void vTCPSend(char *pcBufferToTransmit, const size_t xTotalLengthToSend);
