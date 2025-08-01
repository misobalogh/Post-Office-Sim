/***************************************************************
 * File Name: post_office.h
 * Description: Post Office Simulation - Main header file
 * Author: MICHAL BALOGH <xbalog06@stud.fit.vutbr.cz>
 * Faculty: FIT VUT
 * Date: 28.04.2023
 * Refactored: August 2025
 *
 * Comments: Compiled using gcc 9.4.0+
 ***************************************************************/

#ifndef POST_OFFICE_H
#define POST_OFFICE_H

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

// Constants
#define MAX_SERVICE_TYPES 3
#define OUTPUT_FILENAME "post_office.out"
#define MAX_SERVICE_TIME 10

// Macro to convert milliseconds to microseconds for usleep
#define usleep(x) usleep((x)*1000)

// Global shared variables
extern FILE *output_file;
extern int *num_of_prints;
extern int *office_closed;
extern int *customers_in_queue[MAX_SERVICE_TYPES];

// Semaphores
extern sem_t *mutex_closing;
extern sem_t *mutex_queue;
extern sem_t *mutex_print;
extern sem_t *service_queue[MAX_SERVICE_TYPES];
extern sem_t *customer_ready[MAX_SERVICE_TYPES];
extern sem_t *customer_done[MAX_SERVICE_TYPES];

// Function prototypes from args.c
void parse_arguments(int argc, char *argv[], int *NZ, int *NU, int *TZ, int *TU, int *F);
int validate_argument(char *arg);

// Function prototypes from utils.c
void open_output_file(FILE **file);
sem_t *create_semaphore(unsigned int value);
void destroy_semaphore(sem_t *sem);
int *create_shared_int(int value);
void cleanup_resources(void);
void print_message(FILE *file, const char *format, ...);

// Function prototypes from customer.c
void customer_process(int customer_id, int max_wait_time);

// Function prototypes from worker.c
void office_worker_process(int worker_id, int max_break_time);

#endif // POST_OFFICE_H
