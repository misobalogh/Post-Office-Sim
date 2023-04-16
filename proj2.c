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
//  static sem_t mutex[7];

// global variables in shared memory
int *num_of_prints;
int *closed;


sem_t queue[3];
sem_t customer_sem;

sem_t *mutex;
int customers = 0;



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

sem_t *new_semaphore()
{
    sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem == MAP_FAILED)
    {
        fprintf(stderr, "Error: failed creating semaphore.\n");
        cleanup();
        exit(1);
    }

    if (sem_init(sem, 1, 1) != 0)
    {
        fprintf(stderr, "Error: failed creating semaphore.\n");
        cleanup();
        exit(1);
    }

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

void print(FILE *file, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(file, "%d:  ", *num_of_prints);
    vfprintf(file, format, args);
    fflush(file);
    va_end(args);
    *num_of_prints+=1;
}

void cleanup()
{
    clean_semaphores(mutex);
    clean_memory();
    fclose(output_file);
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

void customer(int idZ, int TZ)
{
    print(output_file, "Z %d: started\n", idZ);
    usleep(rand() % TZ);

    if (*closed) // closed
    {
        print(output_file, "Z %d: going home\n", idZ);
        exit(1);
    }

    int activity_type = rand() % 3;
    //sem_wait(&queue[activity_type]);
    print(output_file, "Z %d: entering office for a service %d\n", idZ, activity_type);

    // sem_wait
    print(output_file, "Z %d: called by office worker\n", idZ);
    usleep(rand() % 10);

    print(output_file, "Z %d: going home\n", idZ);
    exit(0);
}

void office_worker(int id)
{
    if (*closed)
    {
        print(output_file, "U %d: going home\n", id);
        exit(1);
    }
    print(output_file, "U %d: started\n", id);
    usleep(rand() % 10);

    print(output_file, "U %d: calling next customer\n", id);
    usleep(rand() % 10);
}

int* shared_var(int value) {
    int* shared_int = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_int == MAP_FAILED) {
        perror("Error: failed creating shared memory");
        exit(EXIT_FAILURE);
    }

    *shared_int = value;
    return shared_int;
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

    // Create variable closed in shared memory
    closed = shared_var(0);
    num_of_prints = shared_var(1);


    // sem_t sem[NU];
    // sem_t customer_done[NU];
    // sem_t office_worker[NZ];

    int time_before_closing = F / 2 + rand() % (F / 2 + 1);
    usleep(time_before_closing);

    for (int i = 0; i < NZ; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            customer(customers_id[i], TZ);
        }
    }

    for (int i = 0; i < NU; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            office_worker(office_workers_id[i]);
        }
    }
    
    *closed = 1;
    print(output_file, "closing\n");


    while (wait(NULL) > 0);

    // Free memory
    free(customers_id);
    free(office_workers_id);
    cleanup();

    return 0;
}

//=========================== End of file proj2.c =================================//