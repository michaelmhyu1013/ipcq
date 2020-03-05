/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: handler.c - This file contains the functionality for the handler process for writing the file contents
--                          of the specified file to the message queue. If the file cannot be read, the handler process
--                          send a message to close the client process and the handler process. Upon finishing writing of
--                          of the file, the handler will send a message containing an EOT to close the client and handler
--                          process.
--
-- PROGRAM: ipcq
--
-- FUNCTIONS:
--
--                  int manage_client_process(key_t msg_queue_key, int msq_id, int sem_id, struct msqid_ds *msq_status, struct my_msg *connect_msg)
--
-- DATE: Feb 24, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- NOTES:
-- The client handler process must acquire the semaphore in order to write to the message queue. The amount of messages
-- that is written to the client is dependent on the priority specified by the Client when the Client is first created.
-- 
-- The character '|' is used to denote the unsuccessful reading of a file name.
-- The character EOT is used to denote the complete transmission of a file to the message queue.
-- Both of these special characters will be read by the Client to cause the termination of the program.
-- 
----------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include "handler.h"
#include "io.h"
#include "semaphore.h"
#include <time.h>
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: manage_client_process
--
-- DATE: Feb 23, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- INTERFACE: int manage_client_process(key_t msg_queue_key, int msq_id, int sem_id, struct msqid_ds *msq_status, struct my_msg *connect_msg)
--              msg_queue_key:      key of the message queue
--              msq_id:             id of the message queue to write to
--              sem_id:             id of the semaphore that controls the writing to the message queue
--              connect_msg:        pointer to a struct that is sent to the message queue
-- RETURNS: int
--              0 upon successful termination of the program
--
-- NOTES:   The main function of the handler process that is spawn in response to a message received by the server
--          from the client. Handles the closing of the client upon unsuccessful and successful reading of the filename
--          and contents, respectively. The amount of times that a handler process writes to the message queue, prior to signalling the
--          semaphore, is dependent on the priority of the client. 
-- 
----------------------------------------------------------------------------------------------------------------------*/
int manage_client_process(key_t msg_queue_key, int msq_id, int sem_id, struct msqid_ds *msq_status, struct my_msg *connect_msg)
{
    int priority, i;
    struct my_msg message_to_send, control_message;
    char *buffer = NULL;
    FILE *fptr;
    size_t offset, len, msg_len;
    clock_t t;

    // Create handler for client
    printf("Handler pid: \t\t%d\n", getpid());
    printf("Message was: \t\t%s\n", connect_msg->msg_data);
    printf("Priority was: \t\t%d\n", connect_msg->priority);
    printf("Process ID was: \t%d\n", connect_msg->p_id);

    // Open the file in the msg
    control_message.mtype = connect_msg->p_id;

    if ((fptr = fopen(connect_msg->msg_data, "r")) == NULL)
    {
        strncpy(control_message.msg_data, "|", 1);
        if (send_message(msq_id, &control_message) == -1)
        {
            printf("Failed to send |");
            exit(3);
        }
        printf("Fail message sent with content: %s", message_to_send.msg_data);
        fatal("Unable to open file.");
    }

    ssize_t bytes_read = getdelim(&buffer, &len, '\0', fptr);
    if (bytes_read != -1)
    {
        printf("File read with length: \t%ld\n", bytes_read);
    }

    priority = connect_msg->priority;
    msg_len = MAXMESSAGEDATA;

    offset = 0;
    t = clock();
    while (offset < bytes_read)
    {
        wait(sem_id);
        for (i = 0; i < priority && offset < bytes_read; i++)
        {
            message_to_send.mtype = connect_msg->p_id; // write messages with the client_pid as mtype
            strncpy(message_to_send.msg_data, &buffer[offset], MAXMESSAGEDATA);
            offset += msg_len;
            if (send_message(msq_id, &message_to_send) == -1)
            {
                printf("Error sending message");
                exit(3);
            }
            // printf("Message Sent. PID: %d, offset: %ld\n", connect_msg->p_id, offset);
        }
        signal(sem_id);
    }
    t = clock() - t;
    double time_taken = ((double)t/CLOCKS_PER_SEC);


    printf("Duration of transfer: %f seconds\n", time_taken * 10);

    control_message.msg_data[0] =  EOT;
    if (send_message(msq_id, &control_message) == -1)
    {
        fatal("Failed to send closing message");
    }

    printf("Sent exit code\n");
    printf("Handler Process %d exiting...\n", getpid());
    exit(1);
}
