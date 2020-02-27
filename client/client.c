#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "message.h"
#include "client.h"
#include "io.h"

#define OPTIONS "?f:p:"

typedef struct
{
    int p_id;
    int msq_id;
} Thread_Struct;

int main(int argc, char **argv)
{
    key_t msg_queue_key;
    struct msqid_ds msq_status;
    struct my_msg rmsg, msg_filename;
    pthread_t terminalOutputID;

    Thread_Struct *ts = malloc(sizeof(Thread_Struct));

    char *filename;
    int msq_id, opt, this_pid, priority;

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
    else
    {
        printf("msg queue id: %d\n", msq_id);
    }
    msg_filename.mtype = 1;
    msg_filename.priority = priority;
    msg_filename.p_id = this_pid;

    printf("Message type: %ld\n", msg_filename.mtype);
    printf("Priority: %d\n", msg_filename.priority);
    printf("PID: %d\n", this_pid);
    printf("Filename: %s\n", filename);

    printf("Message p_id: %d\n", msg_filename.p_id);
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
    Thread_Struct *ts = (Thread_Struct *)client_info;

    // check if server could read file before entering loop
    if (read_message(ts->msq_id, ts->p_id, &rmsg) < 0)
    {
        fatal("Failed to read first response\n");
    }
    if (rmsg.msg_data[0] == '|')
    {
        printf("Server failed to read file name.\n");
        exit(4);
    }

    printf("%s", rmsg.msg_data);

    while (1)
    {
        if (read_message(ts->msq_id, ts->p_id, &rmsg) < 0)
        {
            fatal("Read from msg queue failed.\n");
        }
        else
        {
            if (rmsg.mesg_len == 0)
            {
                printf("\nExiting client...\n");
                break;
            }
            printf("%s", rmsg.msg_data);
        }
    }
    return 0;
}

// Usage Message
void usage(char **argv)
{
    fprintf(stderr, "Usage: %s -f <filename> -p <priority_level> \n", argv[0]);
    exit(1);
}
