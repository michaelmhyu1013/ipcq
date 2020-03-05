/*------------------------------------------------------------------------------------------------------------------
-- HEADER FILE:		server.h - This file provides all functionality related to a Server process that accesses the shared
--                              message queue object. It creates a semaphore that manages the handler processes and the
--                              access to writing the messages to the message queue.
--
-- PROGRAM:			ipcq
--
-- FUNCTIONS:
--                  int main(void);
--                  int open_queue(key_t keyval);
--                  void server(key_t msg_queue_key, pid_t client_handler_pid, int msq_id, struct my_msg *rmsg);
--
-- DATE:			Feb 22, 2020
--
-- REVISIONS:       (N/A)
--
-- DESIGNER:		Michael Yu
--
-- PROGRAMMER:		Michael Yu
--
----------------------------------------------------------------------------------------------------------------------*/
#ifndef SERVER_H
#define SERVER_H

#include "message.h"

#define ID 824

int main(void);
int open_queue(key_t keyval);
void server(key_t msg_queue_key, pid_t client_handler_pid, int msq_id, struct my_msg *rmsg);

#endif