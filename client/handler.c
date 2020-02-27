#include <stdio.h>
#include "handler.h"
#include "io.h"

/**
 * 
 * 
 * 
 * 
 * 
 * */
int manage_client_process(key_t msg_queue_key, int msq_id, struct msqid_ds *msq_status, struct my_msg *connect_msg)
{
    int priority;
    struct my_msg message_to_send, control_message;
    char *buffer = NULL;
    FILE *fptr;
    size_t offset, len, msg_len;

    // Create handler for client
    printf("Message was: %s\n", connect_msg->msg_data);
    printf("Message length was: %d\n", connect_msg->mesg_len);
    printf("Priority was: %ld\n", connect_msg->mtype);
    printf("Process ID was: %d\n", connect_msg->p_id);

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

    printf("File opened.\n");
    ssize_t bytes_read = getdelim(&buffer, &len, '\0', fptr);
    if (bytes_read != -1)
    {
        printf("File read with length: %ld\n", bytes_read);
    }
    // Read through the file into messages
    // size to copy is max_file_size * priority
    // read while 'len' > offset
    priority = connect_msg->mtype;
    msg_len = MAXMESSAGEDATA * priority;

    printf("Message length to read: %ld\n", msg_len);

    offset = 0;
    while (offset < bytes_read)
    {
        message_to_send.mtype = connect_msg->p_id; // write messages with the client_pid as mtype
        strncpy(message_to_send.msg_data, &buffer[offset], msg_len);
        offset += msg_len * priority;

        // printf("Offset value: %ld\n", offset);

        if (send_message(msq_id, &message_to_send) == -1)
        {
            printf("Error sending message");
            exit(3);
        }
    }

    strncpy(control_message.msg_data, "", 0);
    if (send_message(msq_id, &control_message) == -1)
    {
        fatal("Failed to send closing message");
    }

    printf("Sent exit code\n");
    printf("Client Handler process exiting...\n");
    exit(1);
}