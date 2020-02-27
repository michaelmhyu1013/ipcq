#include <stdio.h>
#include "handler.h"
#include "io.h"
#include "semaphore.h"

int manage_client_process(key_t msg_queue_key, int msq_id, int sem_id, struct msqid_ds *msq_status, struct my_msg *connect_msg)
{
    int priority, i;
    struct my_msg message_to_send, control_message;
    char *buffer = NULL;
    FILE *fptr;
    size_t offset, len, msg_len;

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

    control_message.msg_data[0] = EOT;
    if (send_message(msq_id, &control_message) == -1)
    {
        fatal("Failed to send closing message");
    }

    printf("Sent exit code\n");
    printf("Handler Process %d exiting...\n", getpid());
    exit(1);
}