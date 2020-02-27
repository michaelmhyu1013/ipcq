
/* Semaphore creation and initialization function */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <errno.h>

extern int errno;

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

void wait(int sid) /* acquire semophore */
{
    struct sembuf *sembuf_ptr;

    sembuf_ptr = (struct sembuf *)malloc(sizeof(struct sembuf *));
    sembuf_ptr->sem_num = 0;
    sembuf_ptr->sem_op = -1;
    sembuf_ptr->sem_flg = SEM_UNDO;

    if ((semop(sid, sembuf_ptr, 1)) == -1)
        printf("semop error\n");
}

void signal(int sid) /* release semaphore */
{
    struct sembuf *sembuf_ptr;

    sembuf_ptr = (struct sembuf *)malloc(sizeof(struct sembuf *));
    sembuf_ptr->sem_num = 0;
    sembuf_ptr->sem_op = 1;
    sembuf_ptr->sem_flg = SEM_UNDO;

    if ((semop(sid, sembuf_ptr, 1)) == -1)
        printf("semop error\n");
}