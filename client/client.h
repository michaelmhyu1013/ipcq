
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
#ifndef CLIENT_H
#define CLIENT_H

#define ID 824

void usage(char **argv);
int open_queue(key_t keyval);
void *client(void *client_info);

#endif