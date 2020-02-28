
/*------------------------------------------------------------------------------------------------------------------
-- HEADER FILE:		client.h - This file provides all functionality related to a Client process that accesses and shared
--                              message queue object.
--
-- PROGRAM:			translator
--
-- FUNCTIONS:
--                  void *client(void *client_info);
--                  void usage(char **argv);
--                  int open_queue(key_t keyval);
--                  int read_first_response(Thread_Struct *ts, struct my_msg *rmsg);
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
#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include "message.h"

#define ID 824

typedef struct
{
    int p_id;
    int msq_id;
} Thread_Struct;

void *client(void *client_info);
void usage(char **argv);
int open_queue(key_t keyval);
int read_first_response(Thread_Struct *ts, struct my_msg *rmsg);

#endif