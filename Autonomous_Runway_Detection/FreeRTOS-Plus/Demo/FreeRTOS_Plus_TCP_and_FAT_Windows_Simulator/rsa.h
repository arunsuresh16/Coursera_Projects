#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include "queue.h"
#include "FreeRTOSConfig.h"

#define FIRST_PRIME_P	11
#define SECOND_PRIME_Q	23

extern QueueHandle_t encrypt_tcp_queue_handle;

typedef struct {
	char out_data[DATA_SIZE];
	int size;
} pack_encrypt_data_t;


int prime(long int pr);

void ce();

long int cd(long int x);

void encrypt();

void decrypt();

/* RSA Encryption task to encrypt the output of the edge detection */
void vRsaEncryptionTask(void* par);

void StartEncryption(long int prime_p, long int prime_q, int msg_size);
