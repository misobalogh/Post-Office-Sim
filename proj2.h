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

static FILE *output_file;
static sem_t mutex[7];
static int num_of_prints = 1;


// sem_init(&sem, 0, 1);
// sem_wait(&sem);
// sem_post(&sem);
// sem_destroy(&sem); 

// ./proj2 NZ NU TZ TU F


typedef struct {
    int people_in_queue;
    int capacity;
    int *array_of_customers;
} activity_t;

typedef struct {
    activity_t activity[3];
} fifo_queue_t;


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
sem_t *new_semaphore();

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
void customer(int id, fifo_queue_t *queue, int activity_type);

/**
 * @brief Creates new office worker.
 * 
 * @param id Office worker ID.
 */
void office_worker(int id);

//====================================== Functions for queue ============================================

/**
 * @brief Initializes queue.
 * 
 * @return fifo_queue_t* Pointer to queue.
 */
fifo_queue_t *init_queue();

/**
 * @brief Destroys queue.
 * 
 * @param queue Pointer to queue.
 */
void destroy_queue(fifo_queue_t *queue);

/**
 * @brief get in queue
 * 
 * @param id
 * @param queue
 * @param activity
 */
void get_in_queue(fifo_queue_t *queue, int activity_type, int idZ);

/**
 * @brief get out queue
 * 
 * @param id
 * @param queue
 * @param activity
 */
void leave_queue(fifo_queue_t *queue, int activity_type, int idZ);

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
    if (endptr == arg || arg_value > INT_MAX) {
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

sem_t *new_semaphore()
{
    sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem == MAP_FAILED)
    {
        fprintf(stderr, "Error: failed creating semaphore.\n");
        cleanup();
        exit(1);
    }
    sem_init(sem, 0, 1);
    return sem;
}
    
void init_semaphores(sem_t *mutex)
{
    for (int i = 0; i < 7; i++)
    {
        mutex[i] = *new_semaphore();
    }
}

void clean_semaphores(sem_t *mutex)
{
    for (int i = 0; i < 7; i++)
    {
        sem_destroy(&mutex[i]);
    }
}

void clean_memory()
{
    munmap(mutex, sizeof(sem_t) * 7);
}

void print(FILE *file,  const char *format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(file, "%d:  ", num_of_prints);
    vfprintf(file, format, args);
    fflush(file);
    va_end(args);
    num_of_prints++;
}

void cleanup()
{
    clean_semaphores(mutex);
    clean_memory();
    fclose(output_file);
}

void customer(int idZ, fifo_queue_t *queue, int TZ)
{
    print(output_file, "customer %d: started\n", idZ);
    print(output_file, "customer %d: finished\n", idZ);

    int activity_type = rand() % 3 + 1;
    get_in_queue(queue, activity_type, idZ);

    usleep(rand() % TZ); 
}


//====================================================================================================

void get_in_queue(fifo_queue_t *queue, int activity_type, int idZ) {
    int size_of_queue = queue->activity[activity_type-1].people_in_queue;
    
    // If queue is full, double its capacity.
    if (queue->activity->capacity == size_of_queue) {
        queue->activity->capacity *= 2;
        queue->activity->array_of_customers = realloc(queue->activity->array_of_customers, queue->activity->capacity * sizeof(int));
        if (queue->activity->array_of_customers == NULL) {
            fprintf(stderr, "Error: failed reallocating memory.\n");
            cleanup();
            exit(1);
        }
    }

    // Get in queue to last position. 
    queue->activity[activity_type-1].array_of_customers[size_of_queue] = idZ;
    // Number of people in queue increases.
    queue->activity[activity_type-1].people_in_queue++;

    print(output_file, "customer %d: enters queue %d\n", idZ, activity_type);
}

void leave_queue(fifo_queue_t *queue, int activity_type, int idZ) {
    int size_of_queue = queue->activity[activity_type-1].people_in_queue;
    int *array_of_customers = queue->activity[activity_type-1].array_of_customers;

    // Find position of customer in queue.
    int position = 0;
    for (int i = 0; i < size_of_queue; i++) {
        if (array_of_customers[i] == idZ) {
            position = i;
            break;
        }
    }

    // Move all customers after customer in queue one position forward.
    for (int i = position; i < size_of_queue - 1; i++) {
        array_of_customers[i] = array_of_customers[i+1];
    }

    // Number of people in queue decreases.
    queue->activity[activity_type-1].people_in_queue--;

    print(output_file, "customer %d: leaves queue %d\n", idZ, activity_type);
}

fifo_queue_t *init_queue() {
    fifo_queue_t *queue = malloc(sizeof(fifo_queue_t));
    for (int i = 0; i < 3; i++) {
        queue->activity[i].people_in_queue = 0;
        queue->activity[i].capacity = 10;
        queue->activity[i].array_of_customers = malloc(queue->activity[i].capacity * sizeof(int));
        if (queue->activity[i].array_of_customers == NULL) {
            fprintf(stderr, "Error: failed allocating memory.\n");
            cleanup();
            exit(1);
        }
    }
    return queue;
}

void destroy_queue(fifo_queue_t *queue) {
    for (int i = 0; i < 3; i++) {
        free(queue->activity[i].array_of_customers);
    }
    free(queue);
}

//====================================================================================================