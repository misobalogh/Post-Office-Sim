/***************************************************************
 * File Name: main.c
 * Description: Main program for Post Office Simulation
 * Author: MICHAL BALOGH <xbalog06@stud.fit.vutbr.cz>
 * Faculty: FIT VUT
 * Date: 28.04.2023
 * Refactored: August 2025
 *
 * Comments: Post office simulation using processes and semaphores
 ***************************************************************/

#include "../include/post_office.h"

// Global variables definitions (declared as extern in header)
FILE *output_file;
int *num_of_prints;
int *office_closed;
int *customers_in_queue[MAX_SERVICE_TYPES] = {0};

// Semaphores
sem_t *mutex_closing;
sem_t *mutex_queue;
sem_t *mutex_print;
sem_t *service_queue[MAX_SERVICE_TYPES];
sem_t *customer_ready[MAX_SERVICE_TYPES];
sem_t *customer_done[MAX_SERVICE_TYPES];

/**
 * @brief Initialize all shared resources (memory and semaphores)
 */
static void initialize_resources(void)
{
    // Create shared memory variables
    office_closed = create_shared_int(0);
    num_of_prints = create_shared_int(1);

    for (int i = 0; i < MAX_SERVICE_TYPES; i++)
    {
        customers_in_queue[i] = create_shared_int(0);
    }

    // Create synchronization semaphores
    mutex_queue = create_semaphore(1);
    mutex_print = create_semaphore(1);
    mutex_closing = create_semaphore(1);

    // Create service-specific semaphores
    for (int i = 0; i < MAX_SERVICE_TYPES; i++)
    {
        service_queue[i] = create_semaphore(0);
        customer_ready[i] = create_semaphore(0);
        customer_done[i] = create_semaphore(0);
    }
}

/**
 * @brief Create all customer processes
 *
 * @param num_customers Number of customer processes to create
 * @param max_wait_time Maximum time customers wait before entering
 */
static void create_customer_processes(int num_customers, int max_wait_time)
{
    for (int i = 0; i < num_customers; i++)
    {
        pid_t customer_pid = fork();

        if (customer_pid == 0)
        {
            // Child process - run customer simulation
            customer_process(i + 1, max_wait_time);
        }
        else if (customer_pid < 0)
        {
            perror("Error: Failed to create customer process");
            cleanup_resources();
            exit(1);
        }
        // Parent continues to create more processes
    }
}

/**
 * @brief Create all office worker processes
 *
 * @param num_workers Number of worker processes to create
 * @param max_break_time Maximum break time for workers
 */
static void create_worker_processes(int num_workers, int max_break_time)
{
    for (int i = 0; i < num_workers; i++)
    {
        pid_t worker_pid = fork();

        if (worker_pid == 0)
        {
            // Child process - run worker simulation
            office_worker_process(i + 1, max_break_time);
        }
        else if (worker_pid < 0)
        {
            perror("Error: Failed to create worker process");
            cleanup_resources();
            exit(1);
        }
        // Parent continues to create more processes
    }
}

/**
 * @brief Main function - orchestrates the entire simulation
 *
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return int Exit status (0 for success)
 */
int main(int argc, char *argv[])
{
    // Parse and validate command line arguments
    int num_customers, num_workers, customer_wait_time, worker_break_time, office_open_time;
    parse_arguments(argc, argv, &num_customers, &num_workers,
                   &customer_wait_time, &worker_break_time, &office_open_time);

    // Initialize output file
    open_output_file(&output_file);

    // Seed random number generator for main process
    srand(time(NULL));

    // Initialize all shared resources
    initialize_resources();

    // Create all worker processes first
    create_worker_processes(num_workers, worker_break_time);

    // Create all customer processes
    create_customer_processes(num_customers, customer_wait_time);

    // Calculate random office closing time (F/2 to F milliseconds)
    int closing_time = office_open_time / 2 + (rand() % (office_open_time / 2 + 1));
    usleep(closing_time);

    // Close the office to new customers
    sem_wait(mutex_closing);
    print_message(output_file, "closing\n");
    *office_closed = 1;
    sem_post(mutex_closing);

    // Wait for all child processes to finish
    while (wait(NULL) > 0)
    {
        // Continue waiting for children
    }

    // Clean up all resources before exiting
    cleanup_resources();

    return 0;
}
