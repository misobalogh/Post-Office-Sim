/***************************************************************
 * File Name: args.c
 * Description: Command line argument parsing for Post Office Simulation
 * Author: MICHAL BALOGH <xbalog06@stud.fit.vutbr.cz>
 * Faculty: FIT VUT
 * Date: 28.04.2023
 * Refactored: August 2025
 ***************************************************************/

#include "../include/post_office.h"

/**
 * @brief Parse command line arguments and validate them
 *
 * @param argc Number of arguments
 * @param argv Array of argument strings
 * @param NZ Number of customers (output parameter)
 * @param NU Number of office workers (output parameter)
 * @param TZ Maximum time in milliseconds that a customer waits before entering the office (0<=TZ<=10000)
 * @param TU Maximum break time in milliseconds for workers (0<=TU<=100)
 * @param F Maximum time in milliseconds that the office stays open (0<=F<=10000)
 *
 * Exits with error code 1 if arguments are invalid
 */
void parse_arguments(int argc, char *argv[], int *NZ, int *NU, int *TZ, int *TU, int *F)
{
    const int expected_num_args = 5;

    if (argc != expected_num_args + 1)
    {
        fprintf(stderr, "Error: Invalid number of arguments.\n");
        fprintf(stderr, "Usage: %s NZ NU TZ TU F\n", argv[0]);
        fprintf(stderr, "  NZ: Number of customers\n");
        fprintf(stderr, "  NU: Number of office workers\n");
        fprintf(stderr, "  TZ: Max customer wait time (0-10000ms)\n");
        fprintf(stderr, "  TU: Max worker break time (0-100ms)\n");
        fprintf(stderr, "  F: Office open time (0-10000ms)\n");
        exit(1);
    }

    // Array of pointers to store argument values
    int *arg_values[] = {NZ, NU, TZ, TU, F};

    // Maximum allowed values for each argument
    const int max_values[] = {INT_MAX, INT_MAX, 10000, 100, 10000};

    // Argument names for error messages
    const char *arg_names[] = {"NZ", "NU", "TZ", "TU", "F"};

    // Validate each argument
    for (int i = 0; i < expected_num_args; i++)
    {
        int arg_value = validate_argument(argv[i + 1]);

        if (arg_value < 0 || arg_value > max_values[i])
        {
            fprintf(stderr, "Error: Invalid argument %s (value: %d, max: %d).\n",
                    arg_names[i], arg_value, max_values[i]);
            exit(1);
        }

        // Store the validated argument value
        *(arg_values[i]) = arg_value;
    }
}

/**
 * @brief Validate a single argument string and convert it to integer
 *
 * @param arg Argument string to validate
 * @return int Converted integer value, or -1 if invalid
 */
int validate_argument(char *arg)
{
    char *endptr;
    const long arg_value = strtol(arg, &endptr, 10);

    // Check if the argument was successfully converted
    if (endptr == arg || *endptr != '\0' || arg_value < 0 || arg_value > INT_MAX)
    {
        return -1;
    }

    return (int)arg_value;
}
