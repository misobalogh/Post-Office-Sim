/***************************************************************
 * File Name: worker.c
 * Description: Office worker process implementation for Post Office Simulation
 * Author: MICHAL BALOGH <xbalog06@stud.fit.vutbr.cz>
 * Faculty: FIT VUT
 * Date: 28.04.2023
 * Refactored: August 2025
 ***************************************************************/

#include "../include/post_office.h"

/**
 * @brief Find and select a non-empty service queue randomly
 *
 * @param selected_service Output parameter for the selected service type
 * @return int Number of non-empty queues found
 */
static int select_service_queue(int *selected_service)
{
    int non_empty_queues[MAX_SERVICE_TYPES];
    int non_empty_count = 0;

    // Find all non-empty queues
    for (int i = 0; i < MAX_SERVICE_TYPES; i++)
    {
        if (*customers_in_queue[i] > 0)
        {
            non_empty_queues[non_empty_count] = i;
            non_empty_count++;
        }
    }

    // Randomly select one of the non-empty queues
    if (non_empty_count > 0)
    {
        *selected_service = non_empty_queues[rand() % non_empty_count];
    }

    return non_empty_count;
}

/**
 * @brief Check if all service queues are empty
 *
 * @return int 1 if all queues are empty, 0 otherwise
 */
static int all_queues_empty(void)
{
    for (int i = 0; i < MAX_SERVICE_TYPES; i++)
    {
        if (*customers_in_queue[i] > 0)
        {
            return 0; // Found a non-empty queue
        }
    }
    return 1; // All queues are empty
}

/**
 * @brief Simulate an office worker serving customers
 *
 * @param worker_id Unique identifier for this worker (1-based)
 * @param max_break_time Maximum break time when no customers are waiting (0-100ms)
 *
 * The worker process:
 * 1. Continuously checks for customers in queues
 * 2. Takes breaks when no customers are waiting
 * 3. Serves customers from randomly selected non-empty queues
 * 4. Goes home when office closes and all customers are served
 */
void office_worker_process(int worker_id, int max_break_time)
{
    // Seed random number generator with process ID for uniqueness
    srand(getpid());

    print_message(output_file, "U %d: started\n", worker_id);

    while (1)
    {
        // Check office status and queue state (critical section)
        sem_wait(mutex_closing);

        if (all_queues_empty())
        {
            // If office is closed and no customers are waiting, go home
            if (*office_closed)
            {
                print_message(output_file, "U %d: going home\n", worker_id);
                sem_post(mutex_closing);
                cleanup_resources();
                exit(0);
            }

            // Take a break when no customers are waiting
            print_message(output_file, "U %d: taking break\n", worker_id);
            sem_post(mutex_closing);

            // Break for random time (0 to max_break_time ms)
            usleep(rand() % (max_break_time + 1));
            print_message(output_file, "U %d: break finished\n", worker_id);
            continue;
        }

        sem_post(mutex_closing); // End critical section

        // Select a service queue to serve (critical section for queue selection)
        sem_wait(mutex_queue);

        int service_type;
        int non_empty_count = select_service_queue(&service_type);

        // Double-check that queues haven't become empty
        if (non_empty_count == 0)
        {
            sem_post(mutex_queue);
            continue;
        }

        // Remove customer from selected queue
        --(*customers_in_queue[service_type]);
        sem_post(mutex_queue); // End critical section

        // Signal customer that they can approach
        sem_post(service_queue[service_type]);

        // Wait for customer to be ready for service
        sem_wait(customer_ready[service_type]);

        print_message(output_file, "U %d: serving a service of type %d\n",
                      worker_id, service_type + 1);

        // Simulate service time (0-10ms)
        usleep(rand() % (MAX_SERVICE_TIME + 1));

        print_message(output_file, "U %d: service finished\n", worker_id);

        // Wait for customer to leave
        sem_wait(customer_done[service_type]);
    }
}
