/*------------------------------------------------------------------------------------------------------------------
-- HEADER FILE:		semaphore.h - This file provides all functions required to control the semaphore that manages the
--                                writing to the message queue.
--
-- PROGRAM:			translator
--
-- FUNCTIONS:
--                  int initsem(key_t key);
--                  void wait(int sid);  
--                  void signal(int sid); 
--
-- DATE:			Feb 26, 2020
--
-- REVISIONS:       (N/A)
--
-- DESIGNER:		Michael Yu
--
-- PROGRAMMER:		Michael Yu
--
----------------------------------------------------------------------------------------------------------------------*/

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