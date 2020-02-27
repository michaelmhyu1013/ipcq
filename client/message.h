#ifndef MESSAGE_H
#define MESSAGE_H

#define MESSAGE_STRUCTURE_SIZE (4096 + sizeof(long))
#define MAXMESSAGEDATA (4096 - sizeof(int) - sizeof(int) - sizeof(int)) /* don't want sizeof(Mesg) > 4096 */

struct my_msg
{
    long mtype;
    int p_id;
    int mesg_len;
    int priority;
    char msg_data[MAXMESSAGEDATA];
} msg;

#endif