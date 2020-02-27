
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
#ifndef SERVER_H
#define SERVER_H

#include "message.h"

#define ID 824

void server(key_t msg_queue_key, pid_t client_handler_pid, int msq_id, struct my_msg *rmsg);
int open_queue(key_t keyval);

#endif