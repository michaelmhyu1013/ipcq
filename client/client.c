
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: client.c - An application that reads user input from the terminal and echoes back the contents
--                              to the terminal window. It allows the user to translate the input following a certain
--                              set of rules for special characters.
--
-- PROGRAM: ipcq
--
-- FUNCTIONS:
--
--              int main(void)
--              void fatal(char *s)
--              void clear_character_buffer(char *buf, size_t buffer_size)
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
-- This program constantly monitors user input from the terminal and echoes the exact input back to the console. All 
-- built-in keyboard commands are disabled using the command:
--
--                  system("/bin/stty raw igncr -echo")
--
-- Special characters are handled differently following a certain set of guidelines, listed below:
--                  'E'     - command to perform translation of user input based on following special characters
--                  'a'     - character will be converted to 'z' upon translation
--                  'X'     - character that will represent the BACKSPACE key upon translation
--                  'K'     - character that will represent the LINE-KILL key upon translation
--                  'T'     - character that will represent NORMAL TERMINATION of the program. The user input will be
--                              translated prior to termination
--                  'CTRL+k' - character that will represent ABNORMAL TERMINATION of the program. Application will
--                              immediately exit and no echo or translation of user text will occur 
--
-- The application utilizies a simple fan architecture in constructing the three processes. The parent process is responsible
-- for handling user input, and the two child processes are responsible for translation and terminal output. Upon any 
-- error that occurs from reading or writing from the pipe, the program will automatically terminate and notify the user of
-- the error that occured.
--
-- The application utilizes two pipes, one for transferring data from standard input to the output process. The second pipe
-- is used to transfer translated data from the translation process to the output process.
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

void usage(char **argv)
{
    fprintf(stderr, "Usage: %s -f <filename> -p <priority_level> \n", argv[0]);
    exit(1);
}
