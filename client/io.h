#ifndef IO_H
#define IO_H

#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "message.h"
void mqstat_print(key_t msg_queue_key, int msq_id, struct msqid_ds *mstat);
void fatal(char *s);
int send_message(int msq_id, struct my_msg *qbuf);
int read_message(int msq_id, long type, struct my_msg *qbuf);

#endif