/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: server.c - An application that creates a message queue and reads all messages from the queue. 
--                          The server will parse the data within the message for a valid file name that can be 
--                          opened to read. If the file is valid, a child process will be spawned to handle the opening
--                          of the file and writing the contents of the file to the message queue with the mtype equal
--                          to the Client's PID that initially wrote the filename to the queue.
--
-- PROGRAM: ipcq
--
-- FUNCTIONS:
--
--                  int main(void);
--                  int open_queue(key_t keyval);
--                  void server(key_t msg_queue_key, pid_t client_handler_pid, int msq_id, struct my_msg *rmsg);
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
-- The server will listen indefinitely for messages in the message queue with mtype equal to 1. If a valid filename is
-- specified by the data contained in the message, a client handler process will be spawned to write the contents of the
-- file to the queue.
----------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "handler.h"
#include "io.h"
#include "message.h"
#include "server.h"
#include "semaphore.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: main
--
-- DATE: Feb 22, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- INTERFACE: int main(void)
--
-- RETURNS: int
--              0 upon successful termination of the program
--
-- NOTES:   Acts as the driver of the program and creates a server that listens to messages in the message queue with
--          mtype equal to 1. The message queue is created in this function and exits upon unsuccessful creation.
--          The semaphore is created in this function and controls the writing by the handler processes.
-- 
----------------------------------------------------------------------------------------------------------------------*/
int main(void)
{
    key_t msg_queue_key;
    struct msqid_ds msq_status;
    struct my_msg rmsg;
    int msq_id, sem_id;
    pid_t client_handler_pid = 0;

    system("touch msgqueue.txt");
    if ((msg_queue_key = ftok("msgqueue.txt", 'B')) == -1)
    {
        fatal("ftok");
    }
    if ((msq_id = msgget(msg_queue_key, IPC_CREAT | 0660)) < 0)
    {
        fatal("msgget failed!");
    }

    sem_id = initsem((key_t)SEM_ACCESS_MQ);
    if (semctl(sem_id, 0, SETVAL, 0) < 0)
    {
        fatal("semctl failed");
    }
    signal(sem_id);

    printf("Awaiting incoming message...\n");
    server(msg_queue_key, client_handler_pid, msq_id, &rmsg);

    if (client_handler_pid == 0)
    {
        printf("Child handler spawned\n");
        manage_client_process(msg_queue_key, msq_id, sem_id, &msq_status, &rmsg);
        exit(0);
    }

    // Remove the message queue
    if (msgctl(msq_id, IPC_RMID, 0) < 0)
    {
        perror("msgctl (remove queue) failed!");
        exit(3);
    }
    exit(0);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: server
--
-- DATE: Feb 22, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- INTERFACE: void server(key_t msg_queue_key, pid_t client_handler_pid, int msq_id, struct my_msg *rmsg)
--                  msg_queue_key:      key value of the message queue
--                  client_handler_pid: PID value of the child process that is spawned
--                  msq_id:             id of the message queue
--                  rmsg:               struct of the message to be received by the client
-- RETURNS: void
--
-- NOTES:   Executes a forever loop that listens for messages of type 1 on the message queue. If a valid message is read,
--          the server will spawn a child process to handle the file operations.
-- 
----------------------------------------------------------------------------------------------------------------------*/
void server(key_t msg_queue_key, pid_t client_handler_pid, int msq_id, struct my_msg *rmsg)
{
    while (1)
    {
        if (read_message(msq_id, 1, rmsg) < 0)
        {
            perror("Read message failed. Client Handler will not be created.\n");
        }
        else
        {
            if ((client_handler_pid = fork()) < 0)
            {
                perror("Client fork failed.\n");
            }
            // CLIENT PROCESS
            if (client_handler_pid == 0)
            {
                break;
            }
        }
    }
}