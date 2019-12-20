/* FreeRTOS includes. */
#include <FreeRTOS.h>
#include "task.h"
#include<stdio.h>

#include "rsa.h"
#include "canny.h"

long int p, q, n, t, flag, e[DATA_SIZE], d[DATA_SIZE], temp[DATA_SIZE], j, en[DATA_SIZE], i;
unsigned char m[DATA_SIZE];
pack_encrypt_data_t send_encr_data;
char msg[DATA_SIZE];
int message_size;
int rsa_file_number = 0, tcp_file_number = 0;

static void FileWrite(const char *data, const int size, const char* fileName, const char* access);

int prime(long int pr)
{
	int index;
	j = sqrt((double)pr);
	for (index = 2; index <= j; index++)
	{
		if (pr%index == 0)
			return 0;
	}
	return 1;
}

void ce()
{
	int k;
	k = 0;
	for (i = 2; i < t; i++)
	{
		if (t%i == 0)
			continue;
		flag = prime(i);
		if (flag == 1 && i != p && i != q)
		{
			e[k] = i;
			flag = cd(e[k]);
			if (flag > 0)
			{
				d[k] = flag;
				k++;
			}
			if (k == 99)
				break;
		}
	}
}

long int cd(long int x)
{
	long int k = 1;
	while (1)
	{
		k = k + t;
		if (k%x == 0)
			return(k / x);
	}
}

void encrypt()
{
	long int pt, ct, key = e[0], k, len;
	i = 0;
	len = message_size - 1;
	while (i != len)
	{
		pt = m[i];
		pt = pt - 96;
		k = 1;
		for (j = 0; j < key; j++)
		{
			k = k * pt;
			k = k % n;
		}
		temp[i] = k;
		ct = k + 96;
		en[i] = ct;
		i++;
	}
	en[i] = DATA_SIZE_DELIMITER;

	printf("\nEncryption done\n");
#ifdef ENABLE_DEMO_LOGGING
	printf("Encrypted message of file %d - %d, %d, %d, %d\n", file_number, en[DEMO_DATA_INDEX_1], en[DEMO_DATA_INDEX_2], 
		en[DEMO_DATA_INDEX_3], en[DEMO_DATA_INDEX_4]);
	#ifndef ENABLE_CANNY
	
	printf("THE ENCRYPTED MESSAGE IS\n");
	for (i = 0; en[i] != DATA_SIZE_DELIMITER; i++)
		printf("%c", en[i]);
	printf("\n");

	#endif
#endif

	for (int ind = 0; ind < message_size; ind++)
	{
		send_encr_data.out_data[ind] = (char)en[ind];
	}
	send_encr_data.size = message_size;

#ifdef ENABLE_TCP
	while (pdTRUE != xQueueSend(encrypt_tcp_queue_handle, &send_encr_data, 0))
	{
	#ifdef ENABLE_MY_LOGGING
		printf("Encrypted data Queue Sending failed, retrying\n");
	#endif
		// Wait for receive to complete
		vTaskDelay(portTICK_PERIOD_MS * 500);
	}
	tcp_file_number++;
	printf("Sending file %d to tcp queue with data size - %d\n", tcp_file_number, message_size);

	#ifdef ENABLE_DEMO_LOGGING
	printf("Sending file %d data - %c, %c, %c, %c\n", tcp_file_number, send_encr_data.out_data[DEMO_DATA_INDEX_1], send_encr_data.out_data[DEMO_DATA_INDEX_2],
		send_encr_data.out_data[DEMO_DATA_INDEX_3], send_encr_data.out_data[DEMO_DATA_INDEX_4]);
	#endif

#endif
#ifdef ENCRYPT_DATA_TO_FILE
	char fileName[40];
	sprintf(fileName, "%s%d.txt", ENCRYPT_DATA_FILE_NAME, tcp_file_number);
	FileWrite(send_encr_data.out_data, message_size, fileName, "wb");
#endif
}

void decrypt()
{
	long int pt, ct, key = d[0], k;
	i = 0;
	while (en[i] != DATA_SIZE_DELIMITER)
	{
		ct = temp[i];
		k = 1;
		for (j = 0; j < key; j++)
		{
			k = k * ct;
			k = k % n;
		}
		pt = k + 96;
		m[i] = pt;
		i++;
	}
	m[i] = DATA_SIZE_DELIMITER;

	printf("\nDecryption done\n");
#ifdef ENABLE_DEMO_LOGGING
	printf("Decrypted message of file %d - %d, %d, %d, %d\n", file_number, m[DEMO_DATA_INDEX_1], m[DEMO_DATA_INDEX_2], m[DEMO_DATA_INDEX_3], m[DEMO_DATA_INDEX_4]);
	#ifndef ENABLE_CANNY	
	printf("THE DECRYPTED MESSAGE IS\n");
	for (i = 0; m[i] != DATA_SIZE_DELIMITER; i++)
		printf("%c", m[i]);
	
	#endif
#endif
}

void vRsaEncryptionTask(void* par)
{
	/* remove compiler warning */
	(void)par;

	long int data_size = 0; 
	pack_data_t unpack_data;
	TickType_t tick;

	tick = xTaskGetTickCount();

	memset(&unpack_data, 0, sizeof(pack_data_t));

	while (rsa_file_number != NUM_OF_FILES)
	{
#ifdef ENABLE_CANNY
		if (pdTRUE == xQueueReceive(edge_encrypt_queue_handle, &unpack_data, 0))
		{
			rsa_file_number++;
			memset(&msg, 0, DATA_SIZE);
			for (int index = 0; index < DATA_SIZE; index++)
			{				
				msg[index] = unpack_data.out_data[index];
				data_size++;
				if (unpack_data.out_data[index] == DATA_SIZE_DELIMITER)
				{
					break;
				}
			}

			printf("Received data of %d size from queue\n", data_size);
#ifdef ENABLE_DEMO_LOGGING
			printf("Received data for file %d - %d, %d, %d, %d\n", file_number, unpack_data.out_data[DEMO_DATA_INDEX_1], unpack_data.out_data[DEMO_DATA_INDEX_2],
				unpack_data.out_data[DEMO_DATA_INDEX_3], unpack_data.out_data[DEMO_DATA_INDEX_4]);
#endif
			StartEncryption(FIRST_PRIME_P, SECOND_PRIME_Q, data_size);
		}
		data_size = 0;
#else
		printf("\nENTER FIRST PRIME NUMBER\n");
		scanf("%d", &p);
		
		printf("\nENTER ANOTHER PRIME NUMBER\n");
		scanf("%d", &q);

		printf("\nENTER MESSAGE\n");
		fflush(stdin);
		scanf("%s", msg);
		StartEncryption(p, q, strlen(msg));
#endif
	}

	tick = xTaskGetTickCount() - tick;

	printf("Done Encryption for %d files\n", rsa_file_number);
	printf("Time taken to encrypt %d files - %dms\n", rsa_file_number, tick);
	printf("Exiting RSA Encryption Task\n");
}

void StartEncryption(long int prime_p, long int prime_q, int msg_size)
{
	message_size = msg_size;
	p = prime_p;
	q = prime_q;
	flag = prime(p);
	if (flag == 0)
	{
		printf("\nWrong prime number P\n");
		exit(1);
	}

	flag = prime(q);
	if (flag == 0 || p == q)
	{
		printf("\nWrong prime number Q\n");
		exit(1);
	}

	msg[message_size] = DATA_SIZE_DELIMITER;

	for (i = 0; msg[i] != DATA_SIZE_DELIMITER; i++)
		m[i] = msg[i];
	n = p * q;
	t = (p - 1)*(q - 1);

	ce();
#ifdef ENABLE_MY_LOGGING
	printf("\nPOSSIBLE VALUES OF e AND d ARE\n");
	for (i = 0; i < j - 1; i++)
		printf("\n%ld\t%ld", e[i], d[i]);

	printf("\n");
#endif
	encrypt();

#ifdef ENABLE_DECRYPTION
	decrypt();
#endif
}

static void FileWrite(const char *data, const int size, const char* fileName, const char* access)
{
	FILE* filePtr = fopen(fileName, access);
	if (filePtr == NULL)
		printf("Opening file error\n");

	printf("Creating file %s\n", fileName);

	for (size_t index = 0; index < (size_t)size; index++) {
		if (fwrite(data, sizeof(char), 1, filePtr) != 1) {
			printf("Error writing to file\n");
			fclose(filePtr);
		}
		data++;
	}

	fclose(filePtr);
}
