/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: io.c - All input and output functionalities related to the message queue are defined in this file. All
--                     functions defined are wrapper functions around the necessary message queue read and write functions
--                      to provide better error handling for the application.
--
-- PROGRAM: ipcq
--
-- FUNCTIONS:
--
--                      void fatal(char *s);
--                      int send_message(int msq_id, struct my_msg *qbuf);
--                      int read_message(int msq_id, long type, struct my_msg *qbuf);
--
-- DATE: Feb 22, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- NOTES:
--
----------------------------------------------------------------------------------------------------------------------*/

#include "io.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: send_message
--
-- DATE: Feb 23, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- INTERFACE: int send_message(int msq_id, struct my_msg *qbuf)
--                      msq_id:     int value of the message queue id
--                      qbuf:       pointer to the structure that contains the message to be sent to the message queue
--
-- RETURNS: void.
--
-- NOTES:   Wrapper function that writes messages to the specified message queue.   
--
----------------------------------------------------------------------------------------------------------------------*/
int send_message(int msq_id, struct my_msg *qbuf)
{
    int result, length;
    length = strnlen(qbuf->msg_data, MESSAGE_STRUCTURE_SIZE - sizeof(long));
    qbuf->mesg_len = length;
    if ((result = msgsnd(msq_id, qbuf, MESSAGE_STRUCTURE_SIZE - sizeof(long), 0)) == -1)
    {
        return (-1);
    }
    return result;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: read_message
--
-- DATE: Feb 23, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- INTERFACE: int read_message(int msq_id, long type, struct my_msg *qbuf)
--                      msq_id:     int value of the message queue id
--                      type:       long representing the mtype of the message to be read
--                      qbuf:       pointer to the structure that contains the message to be read from the message queue
--
-- RETURNS: void.
--
-- NOTES:   Wrapper function that reads messages from the specified message queue.   
--
----------------------------------------------------------------------------------------------------------------------*/
int read_message(int msq_id, long type, struct my_msg *qbuf)
{
    int result, length;
    length = MESSAGE_STRUCTURE_SIZE;
    if ((result = msgrcv(msq_id, qbuf, length, type, 0)) == -1)
    {
        return (-1);
    }
    return (result);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: fatal
--
-- DATE: January 17, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- INTERFACE: void fatal(char *s)
--              s: represents the string that will be printed upon a failed read or write function
--
-- RETURNS: void.
--
-- NOTES:   General utility function for logging errors related to read or write functions for pipes.    
--
----------------------------------------------------------------------------------------------------------------------*/
void fatal(char *s)
{
    perror(s);
    exit(1);
}