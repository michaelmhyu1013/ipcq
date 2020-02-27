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
int read_first_response(Thread_Struct *ts, struct my_msg *rmsg);
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
