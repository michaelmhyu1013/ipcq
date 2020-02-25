#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "message.h"
#include "server.h"

int main(int argc, char *argv[])
{
    key_t msg_queue_key;
    struct msqid_ds msq_status;
    struct my_msg rmsg;
    int msq_id;

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

    if (read_message(msq_id, 0, &rmsg) < 0)
    {
        perror("Read message failed.");
    }
    else
    {
        printf("Message was: %s", rmsg.msg_data);
    }
    // Remove the message queue
    if (msgctl(msq_id, IPC_RMID, 0) < 0)
    {
        perror("msgctl (remove queue) failed!");
        exit(3);
    }
    exit(0);
}

void server(key_t msg_queue_key, int msq_id)
{
    while (1)
    {
        struct my_msg rmsg;
        if (read_message(msq_id, 0, &rmsg) < 0)
        {
            perror("Read message failed.");
        }
        else
        {
            printf("Message was: %s", rmsg.msg_data);
            msgctl(msq_id, IPC_RMID, NULL);
        }
    }
}

/*--------- status info print function ---------*/
void mqstat_print(key_t msg_queue_key, int msq_id, struct msqid_ds *mstat)
{
    /*-- call the library function ctime ----*/
    char *ctime();

    printf("\nKey %d, msq_id %d\n\n", msg_queue_key, msq_id);
    printf("%d messages on queue\n\n", (int)mstat->msg_qnum);
    printf("Last send by proc %d at %s\n",
           mstat->msg_lspid, ctime(&(mstat->msg_stime)));
    printf("Last recv by proc %d at %s\n",
           mstat->msg_lrpid, ctime(&(mstat->msg_rtime)));
}

int send_message(int msq_id, struct my_msg *qbuf)
{
    int result, length;
    length = strnlen(qbuf->msg_data, MAX_MSG_SIZE);
    if ((result = msgsnd(msq_id, qbuf, length, 0)) == -1)
    {
        return (-1);
    }
    return result;
}

int read_message(int msq_id, long type, struct my_msg *qbuf)
{
    int result, length; // The length is essentially the size of the structure minus sizeof(mtype)
    length = sizeof(struct my_msg) - sizeof(long) - sizeof(int);
    if ((result = msgrcv(msq_id, qbuf, length, type, 0)) == -1)
    {
        return (-1);
    }
    return (result);
}
