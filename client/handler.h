
/*------------------------------------------------------------------------------------------------------------------
-- HEADER FILE:		input.h - This file provides all methods related to capturing user input from
--                              the terminal
--
-- PROGRAM:			translator
--
-- FUNCTIONS:
--                  void write_from_input(int *pipe, int *pipe2, size_t buffer_size)
--                  void write_to_translate_pipe(int *pipe, char *buffer, size_t buffer_size, int *counter)
--
-- DATE:			Jan 22, 2020
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

int manage_client_process(key_t msg_queue_key, int msq_id, struct msqid_ds *msq_status, struct my_msg *connect_msg);

#endif