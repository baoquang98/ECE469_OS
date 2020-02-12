#ifndef __USERPROG__
#define __USERPROG__

typedef struct circ_buffer {
	int head;
	int tail;
	char buff[BUFFERSIZE];
} circ_buffer;
#define FILENAME_PRODUCER "producer.dlx.obj"
#define FILENAME_CONSUMER "consumer.dlx.obj"

#endif
