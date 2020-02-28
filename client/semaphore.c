/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: semaphore.c - File that holds all functions related to create and control a semaphore.
--
-- PROGRAM: ipcq
--
-- FUNCTIONS:
--
--                  int initsem(key_t key);
--                  void wait(int sid);  
--                  void signal(int sid); 
--
-- DATE: Feb 26, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- NOTES:
--
----------------------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <errno.h>

extern int errno;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: initsem
--
-- DATE: Feb 23, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- INTERFACE: int initsem(key_t key)
--                          key: key value to initialize the semaphore with
--
-- RETURNS: int
--              the semaphore ID if the semaphore is created, else -1
--
-- NOTES:   Wrapper function that creates the semaphore, else returns the existing semaphore.   
--
----------------------------------------------------------------------------------------------------------------------*/
int initsem(key_t key)
{
    int sid, status = 0;

    if ((sid = semget((key_t)key, 1, 0660 | IPC_CREAT | IPC_EXCL)) == -1)
    {
        if (errno == EEXIST)
            sid = semget((key_t)key, 1, 0);
    }
    else /* if created */
        status = semctl(sid, 0, SETVAL, 0);
    if ((sid == -1) || status == -1)
    {
        perror("initsem failed\n");
        return (-1);
    }
    else
        return (sid);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: send_message
--
-- DATE: Feb 23, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- INTERFACE: void wait(int sid) 
--                  sid:   key_t representing the id of the semaphore
--
-- RETURNS: void.
--
-- NOTES:   Wrapper function that writes messages to the specified message queue.   
--
----------------------------------------------------------------------------------------------------------------------*/
void wait(int sid) 
{
    struct sembuf *sembuf_ptr;

    sembuf_ptr = (struct sembuf *)malloc(sizeof(struct sembuf *));
    sembuf_ptr->sem_num = 0;
    sembuf_ptr->sem_op = -1;
    sembuf_ptr->sem_flg = SEM_UNDO;

    if ((semop(sid, sembuf_ptr, 1)) == -1)
        printf("semop error\n");
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: signal
--
-- DATE: Feb 23, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Michael Yu
--
-- PROGRAMMER: Michael Yu
--
-- INTERFACE: void signal(int sid)
--                  sid:   key_t representing the id of the semaphore
--
-- RETURNS: void.
--
-- NOTES:   Wrapper function that writes messages to the specified message queue.   
--
----------------------------------------------------------------------------------------------------------------------*/
void signal(int sid)
{
    struct sembuf *sembuf_ptr;

    sembuf_ptr = (struct sembuf *)malloc(sizeof(struct sembuf *));
    sembuf_ptr->sem_num = 0;
    sembuf_ptr->sem_op = 1;
    sembuf_ptr->sem_flg = SEM_UNDO;

    if ((semop(sid, sembuf_ptr, 1)) == -1)
        printf("semop error\n");
}