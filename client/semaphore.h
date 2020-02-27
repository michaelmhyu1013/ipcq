
#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>

#define SEM_ACCESS_MQ (key_t)824

int initsem(key_t key);
void wait(int sid);   /* acquire semophore */
void signal(int sid); /* release semaphore */

#endif