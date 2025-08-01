/***************************************************************
 * File Name: customer.c
 * Description: Customer process implementation for Post Office Simulation
 * Author: MICHAL BALOGH <xbalog06@stud.fit.vutbr.cz>
 * Faculty: FIT VUT
 * Date: 28.04.2023
 * Refactored: August 2025
 ***************************************************************/

#include "../include/post_office.h"

/**
 * @brief Simulate a customer visiting the post office
 *
 * @param customer_id Unique identifier for this customer (1-based)
 * @param max_wait_time Maximum time to wait before entering office (0-10000ms)
 *
 * The customer process:
 * 1. Starts and announces arrival
 * 2. Waits a random time before entering
 * 3. Checks if office is closed
 * 4. Selects a random service type and joins queue
 * 5. Waits to be called by a worker
 * 6. Gets served and leaves
 */
void customer_process(int customer_id, int max_wait_time)
{
    // Seed random number generator with process ID for uniqueness
    srand(getpid());

    print_message(output_file, "Z %d: started\n", customer_id);

    // Wait random time before entering office (0 to max_wait_time ms)
    usleep(rand() % (max_wait_time + 1));

    // Check if office is closed (critical section)
    sem_wait(mutex_closing);
    if (*office_closed)
    {
        print_message(output_file, "Z %d: going home\n", customer_id);
        sem_post(mutex_closing);
        cleanup_resources();
        exit(0);
    }

    // Choose random service type (1, 2, or 3) and join its queue
    int service_type = rand() % MAX_SERVICE_TYPES;
    ++(*customers_in_queue[service_type]);
    print_message(output_file, "Z %d: entering office for a service %d\n",
                  customer_id, service_type + 1);

    sem_post(mutex_closing); // End critical section

    // Wait for a worker to call this customer
    sem_wait(service_queue[service_type]);
    print_message(output_file, "Z %d: called by office worker\n", customer_id);

    // Simulate time to explain service needs (0-10ms)
    usleep(rand() % (MAX_SERVICE_TIME + 1));

    // Signal worker that customer is ready for service
    sem_post(customer_ready[service_type]);

    print_message(output_file, "Z %d: going home\n", customer_id);

    // Signal worker that customer is leaving
    sem_post(customer_done[service_type]);

    cleanup_resources();
    exit(0);
}
