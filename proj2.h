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
 * Exit with error code 1 if arguments are invalid.
 */
void parse_args(int argc, char *argv[], int *NZ, int *NU, int *TZ, int *TU, int *F);

/**
 * @brief Validate argument and convert it to int.
 * 
 * @param arg Argument to validate.
 * 
 * @return -1 if error, converted argument value if success.
 */
int validate_arg(char *arg);

/**
 * @brief Opens file 'proj2.out' for writing.
 * 
 * @param file Pointer to the output file.
 * 
 * Exit with error code 1 if file cannot be opened.
 */
void open_output_file(FILE **file);

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
 * @brief Creates new customer.
 * 
 * @param idz Customers ID.
 * @param TZ Maximum time in milliseconds that customers waits before leaving without being served.
 *  0<=TZ<=10000
 */
void customer(int idZ, int TZ);

/**
 * @brief Creates new office worker.
 * 
 * @param Uid Office workers ID.
 * @param TU Maximum break time in milliseconds workers take,
 * if there are no customers to serve. 0<=TU<=100
 */
void office_worker(int Uid, int TU);

/**
 * @brief Creates new shared variable of type int.
 * 
 * @param value Initial value of variable.
 * 
 * @return int* Pointer to shared variable.
 */
int* shared_int(int value);

//====================================================================================================


