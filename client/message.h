#ifndef MESSAGE_H
#define MESSAGE_H

#define MAX_MSG_SIZE 1024

struct my_msg
{
    long mtype;
    char msg_data[MAX_MSG_SIZE];
};

#endif