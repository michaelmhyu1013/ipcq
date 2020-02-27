/*--------- status info print function ---------*/
#include "io.h"

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
    length = strnlen(qbuf->msg_data, MESSAGE_STRUCTURE_SIZE - sizeof(long));
    qbuf->mesg_len = length;
    if ((result = msgsnd(msq_id, qbuf, MESSAGE_STRUCTURE_SIZE - sizeof(long), 0)) == -1)
    {
        return (-1);
    }
    return result;
}

int read_message(int msq_id, long type, struct my_msg *qbuf)
{
    int result, length; // The length is essentially the size of the structure minus sizeof(mtype)
    length = MESSAGE_STRUCTURE_SIZE;
    if ((result = msgrcv(msq_id, qbuf, length, type, 0)) == -1)
    {
        return (-1);
    }
    return (result);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: fatal
--
-- DATE: January 17, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- INTERFACE: void fatal(char *s)
--              s: represents the string that will be printed upon a failed read or write function
--
-- RETURNS: void.
--
-- NOTES:   General utility function for logging errors related to read or write functions for pipes.    
--
----------------------------------------------------------------------------------------------------------------------*/
void fatal(char *s)
{
    perror(s);
    exit(1);
}