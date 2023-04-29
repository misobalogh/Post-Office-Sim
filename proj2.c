/***************************************************************
* File Name: htab.h
* Description: riesnie IJC-DU2, priklad b) datova struktura htab_t
* Author: MICHAL BALOGH <xbalog06@stud.fit.vutbr.cz
* Faculty: FIT VUT
* Date: 28.04.2023

* Comments: prekladane pomocou gcc 9.4.0
***************************************************************/

#include "proj2.h"

#define usleep(x) usleep((x)*1000)

static FILE *output_file;

// global variables in shared memory
int *num_of_prints;
int *closed;

int *customers_in_queue[3] = {0};

sem_t *mutext_closing;
sem_t *mutex;
sem_t *mutex_print;

sem_t *queue[3];
sem_t *sem_customer[3];
sem_t *sem_customer_done[3];


// ============================== Utility functions =================================

void parse_args(int argc, char *argv[], int *NZ, int *NU, int *TZ, int *TU, int *F)
{
    const int expected_num_args = 5;

    if (argc != expected_num_args + 1)
    {
        fprintf(stderr, "Error: invalid number of arguments.\n");
        exit(1);
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
            exit(1);
        }
        // Store argument value.
        *(arg_values[i]) = arg_value;
    }
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

void open_output_file(FILE **file)
{
    *file = fopen("proj2.out", "w");
    if (*file == NULL)
    {
        perror("Error: failed opening file.");
        exit(1);
    }
}

sem_t *new_semaphore(unsigned int value)
{
    // Create semaphore in shared memory.
    sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem == MAP_FAILED)
    {
        perror("Error: failed mapping semaphore.");
        cleanup();
        exit(1);
    }

    // Initialize semaphore, check for errors in initialization.
    if (sem_init(sem, 1, value) != 0)
    {
        perror("Error: failed creating semaphore.");
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

//====================================================================================================

void cleanup()
{
    destroy_sem(mutex);
    destroy_sem(mutext_closing);
    destroy_sem(mutex_print);
    
    for (int i = 0; i < 3; i++)
    {
        destroy_sem(sem_customer_done[i]);
        destroy_sem(queue[i]);
        destroy_sem(sem_customer[i]);
    }

    munmap(closed, sizeof(int));
    munmap(num_of_prints, sizeof(int));

    fclose(output_file);
}

void print(FILE *file, const char *format, ...)
{
    sem_wait(mutex_print); // prevent multiple processes from printing at the same time.
    
    va_list args;
    va_start(args, format);
    fprintf(file, "%d:  ", *num_of_prints);
    vfprintf(file, format, args);
    fflush(file);
    va_end(args);

    ++(*num_of_prints); // increment number of printed messages in output file

    sem_post(mutex_print); // end of critical section
}

void customer(int idZ, int TZ)
{
    // seed random number generator
    srand(getpid());
    print(output_file, "Z %d: started\n", idZ);
    // usleep random number from interval <0,TZ>
    usleep(rand() % (TZ + 1));

    // check if office is closed
    sem_wait(mutext_closing); // start of critical section
    if (*closed)
    {
        print(output_file, "Z %d: going home\n", idZ);
        sem_post(mutext_closing);
        cleanup();
        exit(0);
    }
    
    // choose activity type and enter queue for that activity
    int activity_type = rand() % 3;
    ++(*customers_in_queue[activity_type]);
    print(output_file, "Z %d: entering office for a service %d\n", idZ, activity_type + 1);
    
    sem_post(mutext_closing); // end of critical section

    // wait for office worker signal customer
    sem_wait(queue[activity_type]);
    print(output_file, "Z %d: called by office worker\n", idZ);

    // usleep random number from interval <0,10>
    usleep(rand() % (10 + 1));

    // signal office worker that customer is done with request
    sem_post(sem_customer[activity_type]);

    print(output_file, "Z %d: going home\n", idZ);

    // signal office worker that customer is going home
    sem_post(sem_customer_done[activity_type]);
    
    cleanup();
    exit(0);
}

void office_worker(int Uid, int TU)
{
    print(output_file, "U %d: started\n", Uid);
    while (1)
    {
        int queue_size = 0;

        sem_wait(mutext_closing); // start of critical section
        // Check if any queue is not empty
        for (int i = 0; i < 3; i++)
        {
            queue_size = *customers_in_queue[i];
            if (queue_size != 0)
            {
                break;
            }
        }
        // queues are empty:
        if (queue_size == 0)
        {
            
            // if post mail is closed and all queues are empty, go home
            if (*closed)
            {
                print(output_file, "U %d: going home\n", Uid);
                sem_post(mutext_closing);
                cleanup();
                exit(0);
            }

            // if all queues are empty, take a break
            print(output_file, "U %d: taking break\n", Uid);
            sem_post(mutext_closing);
            // usleep random number from interval <0,TU>
            usleep(rand() % (TU + 1));
            print(output_file, "U %d: break finished\n", Uid);
            continue;
        }
        sem_post(mutext_closing); // end of critical section

        int activity_type;
        int non_empty_queue[3] = {0};
        int number_of_non_empty_queues = 0;

        // Get random queue
        sem_wait(mutex);
        // Get all non empty queues
        for (activity_type = 0; activity_type < 3; activity_type++)
        {
            queue_size = *customers_in_queue[activity_type];
            if (queue_size != 0)
            {
                non_empty_queue[number_of_non_empty_queues] = activity_type;
                number_of_non_empty_queues++;
            }
        }
        
        // Meanwhile, some queues might have become empty
        if (number_of_non_empty_queues == 0)
        {
            sem_post(mutex);
            continue;
        }
        // Choose random queue from non empty queues
        activity_type = non_empty_queue[rand() % number_of_non_empty_queues];
       
        // Get customer from queue

        --(*customers_in_queue[activity_type]);
        sem_post(mutex);

        // Serve customer
        sem_post(queue[activity_type]);

        // wait for customer to signal that he is done
        sem_wait(sem_customer[activity_type]);

        print(output_file, "U %d: serving a service of type %d\n", Uid, activity_type + 1);
        usleep(rand() % (10 + 1));

        print(output_file, "U %d: service finished\n", Uid);
        // wait for customer to signal that he is going home
        sem_wait(sem_customer_done[activity_type]);
    }
}

//================================== Main =====================================//

int main(int argc, char *argv[])
{
    // Arguments
    int NZ, NU, TZ, TU, F;

    // Parse arguments, errors are handled in the function
    parse_args(argc, argv, &NZ, &NU, &TZ, &TU, &F);

    // Open output file, errors are handled in the function
    open_output_file(&output_file);

    // Seed random number generator
    srand(time(NULL));

    // Create variables in shared memory

    // Flag for closing post mail
    closed = shared_int(0);
    // Counter of printed messages in output file
    num_of_prints = shared_int(1);
    for (int i = 0; i < 3; i++)
    {
        // Counter of customers in shared memory for each queue
        customers_in_queue[i] = shared_int(0);
    }

    // Create mutexes
    mutex = new_semaphore(1);
    mutex_print = new_semaphore(1);
    mutext_closing = new_semaphore(1);

    // Create semaphores for each queue
    for (int i = 0; i < 3; i++)
    {
        queue[i] = new_semaphore(0);
        sem_customer[i] = new_semaphore(0);
        sem_customer_done[i] = new_semaphore(0);
    }
    // Create semaphores

    // Create office worker processes 
    for (int i = 0; i < NU; i++)
    {
        pid_t pid_worker = fork();
        if (pid_worker == 0)
        {
            office_worker(i + 1, TU);
        }
        else if (pid_worker < 0)
        {
            perror("Error: failed forking process office worker");
            exit(1);
        }
    }

    // Create customer processes
    for (int i = 0; i < NZ; i++)
    {
        pid_t pid_customer = fork();
        if (pid_customer == 0)
        {
            customer(i + 1, TZ);
        }
        else if (pid_customer < 0)
        {
            perror("Error: failed forking process customer");
            exit(1);
        }
    }

    // Wait for 'F argument' seconds before closing post mail
    int time_before_closing = F / 2 + (rand() % (F / 2 + 1));
    usleep(time_before_closing);

    // Close post mail
    sem_wait(mutext_closing);
    print(output_file, "closing\n");
    *closed = 1;
    sem_post(mutext_closing);

    // Wait for all processes to finish
    while (wait(NULL) > 0)
        ;

    // Free memory
    cleanup();

    return 0;
}

//=========================== End of file proj2.c =================================//