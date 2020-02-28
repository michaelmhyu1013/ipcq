
/*------------------------------------------------------------------------------------------------------------------
-- HEADER FILE:		handler.h - This file provides the functionality required for a Client Handler process to read
--                  the desired file specified by the Client and write the contents to the message queue. The access
--                  to the writing operation is controlled using a semaphore to simulate priority for each of the 
--                  clients connected.
--
-- PROGRAM:			ipcq
--
-- FUNCTIONS:
--                  int manage_client_process(key_t msg_queue_key, int msq_id, int sem_id, struct msqid_ds *msq_status, struct my_msg *connect_msg);

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
#ifndef HANDLER_H
#define HANDLER_H
#include "message.h"
#include <sys/ipc.h>
#include <sys/msg.h>

int manage_client_process(key_t msg_queue_key, int msq_id, int sem_id, struct msqid_ds *msq_status, struct my_msg *connect_msg);

#endif