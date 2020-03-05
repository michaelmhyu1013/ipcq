/*------------------------------------------------------------------------------------------------------------------
-- HEADER FILE:		io.h - This file provides all i/o operations related to the message queue for both the client 
--                          and server programs.
--
-- PROGRAM:			ipcq
--
-- FUNCTIONS:
--                  void mqstat_print(key_t msg_queue_key, int msq_id, struct msqid_ds *mstat);
--                  void fatal(char *s);
--                  int send_message(int msq_id, struct my_msg *qbuf);
--                  int read_message(int msq_id, long type, struct my_msg *qbuf);
--
-- DATE:			Feb 24, 2020
--
-- REVISIONS:       (N/A)
--
-- DESIGNER:		Michael Yu
--
-- PROGRAMMER:		Michael Yu
--
----------------------------------------------------------------------------------------------------------------------*/

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

void fatal(char *s);
int send_message(int msq_id, struct my_msg *qbuf);
int read_message(int msq_id, long type, struct my_msg *qbuf);

#endif