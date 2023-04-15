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

#define NUM_ARGS 5
// sem_init(&sem, 0, 1);
// sem_wait(&sem);
// sem_post(&sem);
// sem_destroy(&sem); 

// ./proj2 NZ NU TZ TU F


/**
 * @brief Parse arguments from command line
 * 
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @param NZ Number of customers
 * @param NU Number of officials
 * @param TZ Maximum time in milliseconds that a customer waits after creating their ticket before entering the post office (or leaving without being served). 0<=TZ<=10000
 * @param TU Maximum break time in milliseconds for officials. 0<=TU<=100
 * @param F Maximum time in milliseconds that the post office is closed for new customers. 0<=F<=10000
 * 
 * @return int 0 if success, 1 if error
 */
int parse_args(int argc, char *argv[], int *NZ, int *NU, int *TZ, int *TU, int *F);

/**
 * @brief Validate argument
 * 
 * @param arg Argument to validate
 * 
 * @return int -1 if error, argument value if success
 */
int validate_arg(char *arg);


//====================================================================================================


int parse_args(int argc, char *argv[], int *NZ, int *NU, int *TZ, int *TU, int *F)
{
    if (argc != NUM_ARGS + 1)
    {
        fprintf(stderr, "Error: wrong number of arguments.\n");
        return 1;
    }

    if((*NZ = validate_arg(argv[1])) < 0)
    {
        fprintf(stderr, "Error: invalid argument NZ.\n");
        return 1;
    }

    if((*NU = validate_arg(argv[2])) < 0)
    {
        fprintf(stderr, "Error: invalid argument NU.\n");
        return 1;
    }

    if((*TZ = validate_arg(argv[3])) < 0 || *TZ > 10000)
    {
        fprintf(stderr, "Error: invalid argument TZ.\n");
        return 1;
    }

    if((*TU = validate_arg(argv[4])) < 0 || *TU > 100)
    {
        fprintf(stderr, "Error: invalid argument TU.\n");
        return 1;
    }

    if((*F = validate_arg(argv[5])) < 0 || *F > 10000)
    {
        fprintf(stderr, "Error: invalid argument F.\n");
        return 1;
    }
    
    return 0;
}

int validate_arg(char *arg)
{
    char *endptr;
    long arg_value = strtol(arg, &endptr, 10);
    if (endptr == arg) {
        return -1;
    }
    return arg_value;
}
