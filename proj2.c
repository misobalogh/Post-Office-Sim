/***************************************************************
* File Name: htab.h
* Description: riesnie IJC-DU2, priklad b) datova struktura htab_t
* Author: MICHAL BALOGH <xbalog06@stud.fit.vutbr.cz
* Faculty: FIT VUT
* Date: 28.03.2023

* Comments: prekladane pomocou gcc 9.4.0
***************************************************************/

#include "proj2.h"

static FILE *output_file;
// sem_t queue[3];
// static sem_t mutex[7];

// global variables in shared memory
int *num_of_prints = NULL;
int *closed = NULL;

int *customers_in_queue[3] = {0};

sem_t *queue[3];
sem_t *mutext_closing;
sem_t *mutex;
sem_t *mutex_print;
sem_t *sem_customer;
sem_t *sem_office_worker;
sem_t *sem_customer_done;
sem_t *sem_office_worker_done;


//====================================================================================================


void cleanup()
{
    destroy_sem(mutex);
    destroy_sem(mutext_closing);
    destroy_sem(mutex_print);
    destroy_sem(sem_customer);
    destroy_sem(sem_office_worker);
    destroy_sem(sem_customer_done);
    destroy_sem(sem_office_worker_done);
    for (int i = 0; i < 3; i++)
    {
        destroy_sem(queue[i]);
    }
    munmap(closed, sizeof(int));        //!
    munmap(num_of_prints, sizeof(int)); //!

    fclose(output_file);
}



void print(FILE *file, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    sem_wait(mutex_print);

    fprintf(stdout, "%d:  ", *num_of_prints);
    vfprintf(stdout, format, args);
    fflush(stdout);

    va_end(args);

    va_start(args, format);

    fprintf(file, "%d:  ", *num_of_prints);
    vfprintf(file, format, args);
    fflush(file);

    va_end(args);
    
    ++(*num_of_prints);

    sem_post(mutex_print);
}

void customer(int idZ, int TZ)
{
    print(output_file, "Z %d: started\n", idZ);
    // usleep random number from interval <0,TZ>
    usleep(rand() % (TZ + 1));
    
    sem_wait(mutext_closing);
    
        if (*closed)
        {
            print(output_file, "Z %d: going home\n", idZ);
            sem_post(mutext_closing);
            exit(0);
        }

    sem_post(mutext_closing);
    
    sem_wait(mutex);

        int activity_type = rand() % 3;
        (*customers_in_queue[activity_type])++;
        printf("customers_in_queue[%d] = %d\n", activity_type, *customers_in_queue[activity_type]);
        
        print(output_file, "Z %d: entering office for a service %d\n", idZ, activity_type + 1);
    
    sem_post(mutex);

    sem_wait(queue[activity_type]);

    print(output_file, "Z %d: called by office worker\n", idZ);
    
    // usleep random number from interval <0,10>
    usleep(rand() % (10 + 1));
    sem_post(sem_customer);
    sem_wait(sem_office_worker_done);

    print(output_file, "Z %d: going home\n", idZ);

    sem_wait(mutex);
    --(*customers_in_queue[activity_type]);
    sem_post(mutex);

    printf("dieta odislo\n");
    fflush(stdout);

    exit(0);
}

void office_worker(int Uid, int TU)
{
    print(output_file, "U %d: started\n", Uid);
    while (1)
    { 
        //sem_wait(mutex);

        int queue_size = 0;
        // Check if any queue is not empty
        for (int i = 0; i < 3; i++)
        {
            queue_size = *customers_in_queue[i];
            if (queue_size != 0)
            {   
                break;
            }
        }

        sem_post(mutex);

        printf("queue_size = %d\n", queue_size);
        fflush(stdout);
        printf("po dieatati\n");
        fflush(stdout);

        //sem_wait(mutext_closing);
        if (queue_size == 0)
        {
            // If all queues are empty, take a break

            if (*closed)
            {
                // If post mail is closed and all queues are empty, go home
                print(output_file, "U %d: going home\n", Uid);
                exit(0);
            }
            print(output_file, "U %d: taking break\n", Uid);
            // usleep random number from interval <0,TU>
            usleep(rand() % (TU + 1));
            print(output_file, "U %d: break finished\n", Uid);
            sem_post(mutext_closing);
            continue;
        }
        sem_post(mutext_closing);

        // Get random queue
        sem_wait(mutex);

        int activity_type;
        for (activity_type = 0; activity_type < 3; activity_type++)
        {
            queue_size = *customers_in_queue[activity_type];
            if (queue_size != 0)
            {
                break;
            }
        }
        sem_post(mutex);
                    
        // Serve customer
        sem_post(queue[activity_type]);
        sem_trywait(sem_customer);

        print(output_file, "U %d: serving a service of type %d\n", Uid, activity_type + 1);
        usleep(rand() % (10 + 1));

       

        print(output_file, "U %d: service finished\n", Uid);
        sem_post(sem_office_worker_done);
    }
}

//================================== Main =====================================//

// ./proj2 NZ NU TZ TU F

int main(int argc, char *argv[])
{
    int NZ, NU, TZ, TU, F;
    if (parse_args(argc, argv, &NZ, &NU, &TZ, &TU, &F) == 1)
    {
        // Invalid Arguments
        return 1;
    }

    if (open_file(&output_file) != 0)
    {
        // Error opening file
        return 1;
    }

    // Seed random number generator
    srand(time(NULL));

    // Create array of customers and workers
    int *customers_id = shuffle_id(NZ);
    int *office_workers_id = shuffle_id(NU);

    // Create variables in shared memory
    closed = shared_int(0);
    num_of_prints = shared_int(1);
    for (int i = 0; i < 3; i++)
    {
        customers_in_queue[i] = shared_int(0);
    }

    // Create semaphores
    mutex = new_semaphore(1);
    mutex_print = new_semaphore(1);
    mutext_closing = new_semaphore(1);

    queue[0] = new_semaphore(0);
    queue[1] = new_semaphore(0);
    queue[2] = new_semaphore(0);

    sem_customer = new_semaphore(0);
    sem_office_worker = new_semaphore(0);
    sem_customer_done = new_semaphore(0);
    sem_office_worker_done = new_semaphore(0);


    // Create processes

    for (int i = 0; i < NU; i++)
    {
        pid_t pid_worker = fork();
        if (pid_worker == 0)
        {
            office_worker(office_workers_id[i], TU);
        }
        else if (pid_worker < 0)
        {
            perror("Error: failed forking process office worker");
            exit(1);
        }
    }

    for (int i = 0; i < NZ; i++)
    {
        pid_t pid_customer = fork();
        if (pid_customer == 0)
        {
            customer(customers_id[i], TZ);
        }
        else if (pid_customer < 0)
        {
            perror("Error: failed forking process customer");
            exit(1);
        }
    }

    // Wait for F seconds before closing post mail

    int time_before_closing = F / 2 + rand() % (F / 2 + 1);
    usleep(time_before_closing);

    sem_wait(mutext_closing);
    print(output_file, "closing\n");
    *closed = 1;
    sem_post(mutext_closing);

    while (wait(NULL) > 0)
        ;

    // Free memory
    free(customers_id);
    free(office_workers_id);

    cleanup();

    return 0;
}

//=========================== End of file proj2.c =================================//