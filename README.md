# Post Office Simulation

A multi-process simulation of a post office system using POSIX semaphores and shared memory, written in C. This project demonstrates concurrent programming concepts including process synchronization, inter-process communication, and resource management.

## 📋 Table of Contents

- [Overview](#overview)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Implementation Details](#implementation-details)
- [Testing](#testing)


## 🎯 Overview

This simulation models a post office where:
- **Customers** arrive at random times and require one of three types of services
- **Office workers** serve customers from queues, taking breaks when no customers are waiting
- The **office** closes after a random amount of time, after which no new customers are accepted
- All processes coordinate using semaphores and shared memory

The simulation outputs a detailed log of all events with timestamps, showing the complete interaction flow between customers and workers.


### System Libraries Used
```c
#include <stdio.h>      // File I/O operations
#include <stdlib.h>     // Memory allocation, process control
#include <unistd.h>     // POSIX API access
#include <semaphore.h>  // POSIX semaphores
#include <sys/mman.h>   // Memory management
#include <sys/wait.h>   // Process waiting
#include <time.h>       // Time functions
```

## Installation

### Clone and Build
```bash
# Clone the repository (if using version control)
git clone <repository-url>
cd post-office-simulation

# Build the project
make all

# Or build with debug information
make CFLAGS="-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread -g -DDEBUG"
```

## Usage

### Basic Usage
```bash
./post_office_sim NZ NU TZ TU F
```

### Parameters
- **NZ**: Number of customers (integer ≥ 0)
- **NU**: Number of office workers (integer ≥ 0)
- **TZ**: Maximum customer wait time before entering office (0-10000 ms)
- **TU**: Maximum worker break time when no customers waiting (0-100 ms)
- **F**: Maximum office operation time before closing (0-10000 ms)

### Examples
```bash
# Small simulation: 5 customers, 2 workers
./post_office_sim 5 2 100 50 200

# Large simulation: 50 customers, 10 workers
./post_office_sim 50 10 500 30 1000

# Edge case: No customers
./post_office_sim 0 3 100 50 200

# Edge case: No workers
./post_office_sim 10 0 100 50 200
```

### Quick Test Run
```bash
# Run with predefined test parameters
make run

# Run comprehensive test suite
make test

# Stress test with 1000 iterations
make stress_test
```

## 📁 Project Structure

```
post-office-simulation/
├── src/
│   ├── main.c          # Main program and process orchestration
│   ├── args.c          # Command line argument parsing
│   ├── utils.c         # Utility functions and resource management
│   ├── customer.c      # Customer process implementation
│   └── worker.c        # Office worker process implementation
├── include/
│   └── post_office.h   # Main header with all declarations
├── Makefile            # Build configuration
├── README.md           # This file
└── post_office.out     # Simulation output (generated)
```

### File Descriptions

| File | Purpose | Key Functions |
|------|---------|---------------|
| `main.c` | Program entry point, process creation | `main()`, `initialize_resources()` |
| `args.c` | Argument validation | `parse_arguments()`, `validate_argument()` |
| `utils.c` | Shared utilities | `create_semaphore()`, `print_message()`, `cleanup_resources()` |
| `customer.c` | Customer behavior | `customer_process()` |
| `worker.c` | Worker behavior | `office_worker_process()` |
| `post_office.h` | Global declarations | All function prototypes and extern variables |

## 🔧 Implementation Details

### Process Architecture
```
Main Process
├── Creates shared memory and semaphores
├── Forks Worker Processes (NU instances)
├── Forks Customer Processes (NZ instances)
├── Waits for office closing time
├── Sets office_closed flag
└── Waits for all children to complete
```

### Synchronization Mechanisms

#### Semaphores Used
- **`mutex_print`**: Ensures atomic output operations
- **`mutex_closing`**: Protects office closing state
- **`mutex_queue`**: Protects queue selection logic
- **`service_queue[3]`**: Customer-worker signaling for each service type
- **`customer_ready[3]`**: Signals worker that customer is ready
- **`customer_done[3]`**: Signals worker that customer is leaving

#### Shared Memory Variables
- **`office_closed`**: Flag indicating if office accepts new customers
- **`num_of_prints`**: Counter for output line numbering
- **`customers_in_queue[3]`**: Number of customers waiting for each service type

### Service Types
The simulation models three types of postal services:
1. **Service 1**: General postal services
2. **Service 2**: Package handling
3. **Service 3**: Financial services

Customers randomly select one service type upon entering the office.

### Timing Behavior
- **Customer arrival**: Random delay (0 to TZ ms) before entering office
- **Service time**: Random duration (0 to 10 ms) for explaining needs
- **Worker breaks**: Random duration (0 to TU ms) when no customers waiting
- **Office closing**: Random time (F/2 to F ms) after opening

## 🧪 Testing

### Available Test Commands
```bash
# Memory leak detection
make valgrind

# Stress testing
make stress_test

# Basic functionality tests
make test

# Custom test
./post_office_sim 20 5 300 50 500
```

### Expected Output Format
```
1: U 1: started
2: U 2: started
3: Z 1: started
4: Z 1: entering office for a service 2
5: U 1: serving a service of type 2
6: Z 1: called by office worker
7: Z 1: going home
8: U 1: service finished
9: closing
10: U 1: going home
11: U 2: going home
```

### Common Test Scenarios

| Scenario | Command | Purpose |
|----------|---------|---------|
| Basic function | `./post_office_sim 5 2 100 50 200` | Verify core functionality |
| No customers | `./post_office_sim 0 2 100 50 200` | Test worker-only behavior |
| No workers | `./post_office_sim 5 0 100 50 200` | Test customer timeout |
| Quick closing | `./post_office_sim 10 3 200 50 10` | Test early office closure |
| Large scale | `./post_office_sim 100 20 500 30 2000` | Stress test |

## 👥 Authors

- **MICHAL BALOGH** - <xbalog06@stud.fit.vutbr.cz>
- Faculty: FIT VUT
- Original Date: 28.04.2023
- Refactored: August 2025

## 🙏 Acknowledgments

- VUT FIT Operating Systems course materials
- POSIX documentation and examples
- GNU C Library documentation
- Linux system programming resources
