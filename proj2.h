/***************************************************************
* File Name: htab.h
* Description: riesnie IJC-DU2, priklad b) datova struktura htab_t
* Author: MICHAL BALOGH <xbalog06@stud.fit.vutbr.cz
* Faculty: FIT VUT
* Date: 28.03.2023

* Comments: prekladane pomocou gcc 9.4.0
***************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <limits.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


// sem_init(&sem, 0, 1);
// sem_wait(&sem);
// sem_post(&sem);
// sem_destroy(&sem); 

//====================================================================================================

/**
 * @brief Parse arguments from command line
 * 
 * @param argc Number of arguments.
 * @param argv Array of arguments.
 * @param NZ Number of customers.
 * @param NU Number of office workers.
 * @param TZ Maximum time in milliseconds that a customer waits after creating their ticket before entering the post office (or leaving without being served). 0<=TZ<=10000
 * @param TU Maximum break time in milliseconds for workers. 0<=TU<=100
 * @param F Maximum time in milliseconds that the post office is closed for new customers. 0<=F<=10000
 * 
 * @return int 0 if success, 1 if error.
 */
int parse_args(int argc, char *argv[], int *NZ, int *NU, int *TZ, int *TU, int *F);

/**
 * @brief Validate argument and convert it to int.
 * 
 * @param arg Argument to validate.
 * 
 * @return -1 if error, converted argument value if success.
 */
int validate_arg(char *arg);

/**
 * @brief Opens file for writing.
 * 
 * @param file Pointer to file.
 * 
 * @return 1 if error, else 0.
 */
int open_file(FILE **file);

/**
 * @brief Creates new semaphore.
 * If error occurs, program exits with error code 1.
 * 
 * @return sem_t* Pointer to newly created semaphore.
 */
sem_t *new_semaphore(unsigned int value);

/**
 * @brief Frees all resources and closes file.
 * 
 */
void cleanup();

/**
 * @brief Prints to file. 
 * Works like fprintf, but also prints number of prints and flushes file.
 * 
 * @param file File to print to.
 * @param format Format of string.
 * @param ... Arguments.
 */
void print(FILE *file, const char *format, ...);

/**
 * @brief Frees all shared memory.
 * 
 */
void clean_memory();

/**
 * @brief Creates new customer.
 * 
 * @param id Customer ID.
 */
void customer(int id, int activity_type);

/**
 * @brief Creates new office worker.
 * 
 * @param id Office worker ID.
 */
void office_worker(int id, int TU);

/**
 * @brief Creates new shared variable of type int.
 * 
 * @param value Initial value of variable.
 * 
 * @return int* Pointer to shared variable.
 */
int* shared_int(int value);

//====================================================================================================



