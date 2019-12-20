/* FreeRTOS includes. */
#include <FreeRTOS.h>
#include "task.h"
#include<stdio.h>

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_TCP_server.h"
#include "FreeRTOS_DHCP.h"

#include "canny.h"
#include "rsa.h"
#include "tcp_client.h"

int tcp_file_num = 0;

void vTcpSendDataTask(void* par)
{
	/* remove compiler warning */
	(void)par;
	printf("Starting TCP Send task\n");
	pack_encrypt_data_t unpack_encrypt_data;
	memset(&unpack_encrypt_data, 0, sizeof(unpack_encrypt_data));

	while (tcp_file_num != NUM_OF_FILES)
	{
		if (pdTRUE == xQueueReceive(encrypt_tcp_queue_handle, &unpack_encrypt_data, 0))
		{
			tcp_file_num++;
				
			printf("Received data of %d size from encrypt queue\n", unpack_encrypt_data.size);
			vTCPSend(unpack_encrypt_data.out_data, unpack_encrypt_data.size);
		}
	}
	printf("Exiting TCP Send Task\n");

	printf("All Tasks complete\n");
}

void vTCPSend(char *pcBufferToTransmit, const size_t xTotalLengthToSend)
{
	Socket_t xSocket;
	struct freertos_sockaddr xRemoteAddress;
	BaseType_t xAlreadyTransmitted = 0, xBytesSent = 0;
	size_t xLenToSend;

	xRemoteAddress.sin_port = FreeRTOS_htons(config_SERVER_PORT);
	xRemoteAddress.sin_addr = FreeRTOS_inet_addr_quick(config_SERVER_ADDR0, config_SERVER_ADDR1, 
		config_SERVER_ADDR2, config_SERVER_ADDR3);
	
	/* Create a socket. */
	xSocket = FreeRTOS_socket(FREERTOS_AF_INET,
		FREERTOS_SOCK_STREAM,/* FREERTOS_SOCK_STREAM for TCP. */
		FREERTOS_IPPROTO_TCP);
	configASSERT(xSocket != FREERTOS_INVALID_SOCKET);
		
	printf("Success Creating Socket\n");

	/* Connect to the remote socket.  The socket has not previously been bound to
	a local port number so will get automatically bound to a local port inside
	the FreeRTOS_connect() function. */
	if (FreeRTOS_connect(xSocket, &xRemoteAddress, sizeof(xRemoteAddress)) == 0)
	{
		printf("Connected to the Server\n");
		/* Keep sending until the entire buffer has been sent. */
		while (xAlreadyTransmitted < (BaseType_t)xTotalLengthToSend)
		{
			/* How many bytes are left to send? */
			xLenToSend = xTotalLengthToSend - xAlreadyTransmitted;
			xBytesSent = FreeRTOS_send( /* The socket being sent to. */
				xSocket,
				/* The data being sent. */
				&(pcBufferToTransmit[xAlreadyTransmitted]),
				/* The remaining length of data to send. */
				xLenToSend,
				/* ulFlags. */
				0);

			// printf("Txmited - %d, %d, %d\n", xTotalLengthToSend, xAlreadyTransmitted, xBytesSent);
#ifdef ENABLE_MY_LOGGING
			printf("Txmited - %s, %d, %d, %d\n", pcBufferToTransmit, xTotalLengthToSend, xAlreadyTransmitted, xBytesSent);
#endif
			if (xBytesSent >= 0)
			{
				/* Data was sent successfully. */
				xAlreadyTransmitted += xBytesSent;
			}
			else
			{
				/* Error - break out of the loop for graceful socket close. */
				break;
			}
		}

		printf("TCP Transmission success for file %d\n", tcp_file_num);
	}
	

	/* Initiate graceful shutdown. */
	FreeRTOS_shutdown(xSocket, FREERTOS_SHUT_RDWR);

	/* Wait for the socket to disconnect gracefully (indicated by FreeRTOS_recv()
	returning a FREERTOS_EINVAL error) before closing the socket. */
	if (FreeRTOS_recv(xSocket, pcBufferToTransmit, xTotalLengthToSend, 100) >= 0)
	{
		vTaskDelay(250);
	}

#ifdef ENABLE_MY_LOGGING
	printf("Shut down and closed socket\n");
#endif
	/* The socket has shut down and is safe to close. */
	FreeRTOS_closesocket(xSocket);
}