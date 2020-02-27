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

int main(int argc, char *argv[])
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