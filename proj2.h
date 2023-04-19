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


int parse_args(int argc, char *argv[], int *NZ, int *NU, int *TZ, int *TU, int *F)
{
    const int expected_num_args = 5;

    if (argc != expected_num_args + 1)
    {
        fprintf(stderr, "Error: wrong number of arguments.\n");
        return 1;
    }

    // Array of pointers that stores values of arguments.
    int *arg_values[] = {NZ, NU, TZ, TU, F};

    // Constraints for size of arguments TZ, TU, F.
    const int max_values[] = {INT_MAX, INT_MAX, 10000, 100, 10000};

    // Names of arguments for error message.
    const char *arg_names[] = {"NZ", "NU", "TZ", "TU", "F"};

    // Validate arguments.
    for (int i = 0; i < expected_num_args; i++)
    {
        int arg_value = validate_arg(argv[i + 1]);

        if (arg_value < 0 || arg_value > max_values[i])
        {
            fprintf(stderr, "Error: invalid argument %s.\n", arg_names[i]);
            return 1;
        }
        // Store argument value.
        *(arg_values[i]) = arg_value;
    }

    return 0;
}

int validate_arg(char *arg)
{
    char *endptr;
    const long arg_value = strtol(arg, &endptr, 10);
    // Check if argument was successfully converted to long.
    if (endptr == arg || arg_value > INT_MAX)
    {
        return -1;
    }

    return arg_value;
}

int open_file(FILE **file)
{
    *file = fopen("proj2.out", "w");
    if (*file == NULL)
    {
        fprintf(stderr, "Error: failed opening file.\n");
        return 1;
    }

    return 0;
}

sem_t *new_semaphore(unsigned int value)
{
    sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem == MAP_FAILED)
    {
        fprintf(stderr, "Error: failed creating semaphore.\n");
        cleanup();
        exit(1);
    }

    if (sem_init(sem, 1, value) != 0)
    {
        fprintf(stderr, "Error: failed creating semaphore.\n");
        cleanup();
        exit(1);
    }

    return sem;
}

void destroy_sem(sem_t *sem)
{
    sem_destroy(sem);
    munmap(sem, sizeof(sem_t));
}

int *shuffle_id(int interval_size)
{
    int *interval = malloc(sizeof(int) * interval_size);
    for (int i = 0; i < interval_size; i++)
    {
        interval[i] = i + 1;
    }

    for (int i = interval_size - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = interval[i];
        interval[i] = interval[j];
        interval[j] = temp;
    }
    return interval;
}

int *shared_int(int value)
{
    int *shared_int = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_int == MAP_FAILED)
    {
        perror("Error: failed creating shared memory");
        exit(1);
    }

    *shared_int = value;
    return shared_int;
}