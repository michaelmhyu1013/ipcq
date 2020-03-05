/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: client.c - An application that allows a user to write a filename to an existing message queue. If the
--                          file can be found by the corresponding server application, the file will be opened by the
--                          server and written to the message queue. The client will read messages from the queue, listening
--                          for messages with mtype equal to the Client's PID. Concurrent clients can be run and the priority
--                          level specified will dictate the speed in which the Client will receive messages.
--
-- PROGRAM: ipcq
--
-- FUNCTIONS:
--
--                  void *client(void *client_info);
--                  void usage(char **argv);
--                  int open_queue(key_t keyval);
--                  int read_first_response(Thread_Struct *ts, struct my_msg *rmsg);
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
-- The client application can be compiled with the command:                 make ipcqc
-- The client can be ran with the following flags:
--              -f: specifies the filename for the server to open
--              -p: specifies the priority of the Client
--
-- The server application can be compiled with the command:                 make ipcqs
-- 
-- Ensure that the server is running prior to the client, or the message queue will not be created, which is required
-- for the transfer of messages.
--
-- The initial message that the client sends will always have a mtype equal to 1, as the server only listens this type.
-- The program will echo all messages read from the mesage queue with mtype equal to this Client's PID to the terminal 
-- window. The user can specificy the priority of the Client process. The priority can only be positive, and the larger
-- the value, the greater the priority of the client. Greater priority means a larger number of messages will be written
-- to the message queue by the server for the specific Client, so that the transfer of data is faster.
----------------------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "client.h"
#include "io.h"
#include "message.h"
#include "semaphore.h"

#define OPTIONS "?f:p:"

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
-- NOTES:   Acts as the driver of the program and is responsible for the creation of the child process. The Client will
--          attempt to open the message queue that is created by the server, else exits. The filename specified by the 
--          -f flag during client execution will be written to the message queue.
--
--          The client then enters a listening mode and will read all messages from the queue with mtype equal to this 
--          Client's PID. If the filename cannot be open, the Client will exit. Else, the contents of the file will be
--          written to the terminal screen as it is read off the message queue.
-- 
----------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    char *filename;
    int msq_id, opt, this_pid, priority;
    key_t msg_queue_key;
    pthread_t terminalOutputID;
    struct my_msg msg_filename;

    Thread_Struct *ts = malloc(sizeof(Thread_Struct));
    this_pid = getpid();

    if (argc != 5)
    {
        usage(argv);
    }

    while ((opt = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch (opt)
        {
        case 'f':
            filename = optarg;
            break;
        case 'p':
            priority = atoi(optarg);
            break;
        default:
        case '?':
            usage(argv);
            break;
        }
    }

    if ((msg_queue_key = ftok("msgqueue.txt", 'B')) == -1)
    {
        perror("ftok");
        exit(1);
    }

    if ((msq_id = msgget(msg_queue_key, IPC_CREAT | 0660)) < 0)
    {
        perror("msgget failed!");
        exit(2);
    }

    msg_filename.mtype = 1;
    msg_filename.priority = priority;
    msg_filename.p_id = this_pid;
    strcpy(msg_filename.msg_data, filename);

    ts->p_id = msg_filename.p_id;
    ts->msq_id = msq_id;

    if (pthread_create(&terminalOutputID, NULL, client, (void *)ts) != 0)
    {
        fatal("Thread failed to create.");
    }
    if (send_message(msq_id, &msg_filename) == -1)
    {
        printf("Error sending message");
        exit(3);
    }
    pthread_join(terminalOutputID, NULL);
    free(ts);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: client
--
-- DATE: Feb 26, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- INTERFACE: void *client(void *client_info)
--
-- RETURNS:   void
--
-- NOTES:   Thread function that performs reading from the message queue and printing to the terminal window. The function
--          keeps a running count of the message length received from the server and exits upon receiving an EOT, printing 
--          the total length of data read from the queue.
-- 
----------------------------------------------------------------------------------------------------------------------*/
void *client(void *client_info)
{
    struct my_msg rmsg;
    int totalBytesRead = 0;
    Thread_Struct *ts = (Thread_Struct *)client_info;

    if ((read_first_response(ts, &rmsg)) != 0)
    {
        fatal("Server failed to read first message.");
    }

    totalBytesRead += rmsg.mesg_len;
    printf("%s", rmsg.msg_data);

    while (1)
    {
        if (read_message(ts->msq_id, ts->p_id, &rmsg) < 0)
        {
            fatal("Read from msg queue failed.\n");
        }
        else
        {
            totalBytesRead += rmsg.mesg_len;

            if (rmsg.msg_data[0] == EOT)
            {
                // Decrement to not count EOT
                printf("Total bytes read: %d\n", --totalBytesRead);
                printf("\nExiting client...\n");
                break;
            }
            printf("%s", rmsg.msg_data);
        }
    }
    return 0;
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: read_first_response
--
-- DATE: Feb 27, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- INTERFACE: int read_first_response(Thread_Struct *ts, struct my_msg *rmsg)
--               ts:    struct that holds the PID and message queue ID required for reading
--               rmsg:  struct that represents the message to be sent to the message queue
-- RETURNS:   int
--               0 upon successfully reading a message containing the files contents, else -1 if it reads the character '|'
--
-- NOTES:   Reads the first message from the server to ensure that the filename is valid
-- 
----------------------------------------------------------------------------------------------------------------------*/
int read_first_response(Thread_Struct *ts, struct my_msg *rmsg)
{
    if (read_message(ts->msq_id, ts->p_id, rmsg) < 0)
    {
        fatal("Failed to read first response\n");
    }
    if (rmsg->msg_data[0] == '|')
    {
        fatal("Server failed to read file name.\n");
    }
    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: usage
--
-- DATE: Feb 24, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- INTERFACE: void usage(char **argv)
--                  argv: pointer to an array of character buffers that represent the command line arguments
-- 
-- RETURNS:   void
--
-- NOTES:   Prints the proper usage of the function to the terminal window.
-- 
----------------------------------------------------------------------------------------------------------------------*/
void usage(char **argv)
{
    fprintf(stderr, "Usage: %s -f <filename> -p <priority_level> \n", argv[0]);
    exit(1);
}
