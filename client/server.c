#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "message.h"
#include "server.h"
#include "io.h"
#include "handler.h"

int main(int argc, char *argv[])
{
    key_t msg_queue_key;
    struct msqid_ds msq_status;
    struct my_msg rmsg;
    int msq_id;
    pid_t client_handler_pid;

    system("touch msgqueue.txt");
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

    /*--- get status info -----------------*/
    if (msgctl(msq_id, IPC_STAT, &msq_status) == -1)
    {
        perror("msgctl (get status)failed!");
        exit(3);
    }
    mqstat_print(msg_queue_key, msq_id, &msq_status);

    server(msg_queue_key, client_handler_pid, msq_id, &rmsg);

    if (client_handler_pid == 0)
    {
        printf("Child handler spawned\n");
        manage_client_process(msg_queue_key, msq_id, &msq_status, &rmsg);
    }

    // Remove the message queue
    if (msgctl(msq_id, IPC_RMID, 0) < 0)
    {
        perror("msgctl (remove queue) failed!");
        exit(3);
    }
    exit(0);
}

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
            printf("MTYPE RECEIVED: %ld", rmsg->mtype);

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