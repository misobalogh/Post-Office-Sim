/***************************************************************
 * File Name: utils.c
 * Description: Utility functions for Post Office Simulation
 * Author: MICHAL BALOGH <xbalog06@stud.fit.vutbr.cz>
 * Faculty: FIT VUT
 * Date: 28.04.2023
 * Refactored: August 2025
 ***************************************************************/

#include "../include/post_office.h"

/**
 * @brief Open the output file for writing simulation results
 *
 * @param file Pointer to file pointer that will be set to the opened file
 *
 * Exits with error code 1 if file cannot be opened
 */
void open_output_file(FILE **file)
{
    *file = fopen(OUTPUT_FILENAME, "w");
    if (*file == NULL)
    {
        perror("Error: Failed to open output file");
        exit(1);
    }
}

/**
 * @brief Create a new semaphore in shared memory
 *
 * @param value Initial value for the semaphore
 * @return sem_t* Pointer to the created semaphore
 *
 * Exits with error code 1 if semaphore creation fails
 */
sem_t *create_semaphore(unsigned int value)
{
    // Create semaphore in shared memory
    sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem == MAP_FAILED)
    {
        perror("Error: Failed to map semaphore memory");
        cleanup_resources();
        exit(1);
    }

    // Initialize semaphore with process-shared flag
    if (sem_init(sem, 1, value) != 0)
    {
        perror("Error: Failed to initialize semaphore");
        cleanup_resources();
        exit(1);
    }

    return sem;
}

/**
 * @brief Destroy a semaphore and free its memory
 *
 * @param sem Pointer to the semaphore to destroy
 */
void destroy_semaphore(sem_t *sem)
{
    if (sem != NULL)
    {
        sem_destroy(sem);
        munmap(sem, sizeof(sem_t));
    }
}

/**
 * @brief Create a shared integer variable in shared memory
 *
 * @param value Initial value for the shared integer
 * @return int* Pointer to the shared integer
 *
 * Exits with error code 1 if memory allocation fails
 */
int *create_shared_int(int value)
{
    int *shared_int = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                          MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_int == MAP_FAILED)
    {
        perror("Error: Failed to create shared memory for integer");
        exit(1);
    }

    *shared_int = value;
    return shared_int;
}

/**
 * @brief Clean up all allocated resources (semaphores, shared memory, files)
 */
void cleanup_resources(void)
{
    // Destroy all semaphores
    destroy_semaphore(mutex_queue);
    destroy_semaphore(mutex_closing);
    destroy_semaphore(mutex_print);

    for (int i = 0; i < MAX_SERVICE_TYPES; i++)
    {
        destroy_semaphore(customer_done[i]);
        destroy_semaphore(service_queue[i]);
        destroy_semaphore(customer_ready[i]);
    }

    // Free shared memory
    if (office_closed != NULL)
        munmap(office_closed, sizeof(int));
    if (num_of_prints != NULL)
        munmap(num_of_prints, sizeof(int));

    for (int i = 0; i < MAX_SERVICE_TYPES; i++)
    {
        if (customers_in_queue[i] != NULL)
            munmap(customers_in_queue[i], sizeof(int));
    }

    // Close output file
    if (output_file != NULL)
        fclose(output_file);
}

/**
 * @brief Thread-safe printing function with automatic line numbering
 *
 * @param file File to print to
 * @param format Printf-style format string
 * @param ... Variable arguments for the format string
 */
void print_message(FILE *file, const char *format, ...)
{
    sem_wait(mutex_print); // Critical section start

    va_list args;
    va_start(args, format);

    // Print line number followed by the message
    fprintf(file, "%d: ", *num_of_prints);
    vfprintf(file, format, args);
    fflush(file); // Ensure immediate output

    va_end(args);

    ++(*num_of_prints); // Increment message counter

    sem_post(mutex_print); // Critical section end
}
