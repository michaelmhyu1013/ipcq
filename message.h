/*------------------------------------------------------------------------------------------------------------------
-- HEADER FILE:		message.h - This file defines the struct that will be written to the message queue by both the
--                              client and the server. The struct is modified to contain the process ID of the originating
--                              client that sent the message, as well as the priority of the client process.
--
-- PROGRAM:			ipcq
--
-- FUNCTIONS:
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

#ifndef MESSAGE_H
#define MESSAGE_H

#define MESSAGE_STRUCTURE_SIZE (4096 + sizeof(long))
#define MAXMESSAGEDATA (4096 - sizeof(int) - sizeof(int) - sizeof(int))
#define EOT 0x03

struct my_msg
{
    long mtype;
    int p_id;
    int mesg_len;
    int priority;
    char msg_data[MAXMESSAGEDATA];
} msg;

#endif