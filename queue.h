#include <stdio.h>
#include <stdlib.h>


typedef struct {
    int people_in_queue;
    int capacity;
    int *array_of_customers;
} activity_t;

typedef struct {
    activity_t activity[3];
} fifo_queue_t;




//====================================== Functions for queue ============================================

/**
 * @brief Initializes queue.
 * 
 * @return fifo_queue_t* Pointer to queue.
 */
fifo_queue_t *init_queue();

/**
 * @brief Destroys queue.
 * 
 * @param queue Pointer to queue.
 */
void destroy_queue(fifo_queue_t *queue);

/**
 * @brief get in queue
 * 
 * @param id
 * @param queue
 * @param activity
 */
void get_in_queue(fifo_queue_t *queue, int activity_type, int idZ);

/**
 * @brief get out queue
 * 
 * @param id
 * @param queue
 * @param activity
 */
void leave_queue(fifo_queue_t *queue, int activity_type, int idZ);


//====================================================================================================

void get_in_queue(fifo_queue_t *queue, int activity_type, int idZ) {
    int size_of_queue = queue->activity[activity_type-1].people_in_queue;
    
    // If queue is full, double its capacity.
    if (queue->activity->capacity == size_of_queue) {
        queue->activity->capacity *= 2;
        queue->activity->array_of_customers = realloc(queue->activity->array_of_customers, queue->activity->capacity * sizeof(int));
        if (queue->activity->array_of_customers == NULL) {
            fprintf(stderr, "Error: failed reallocating memory.\n");
            cleanup();
            exit(1);
        }
    }

    // Get in queue to last position. 
    queue->activity[activity_type-1].array_of_customers[size_of_queue] = idZ;
    // Number of people in queue increases.
    queue->activity[activity_type-1].people_in_queue++;

    print(output_file, "customer %d: enters queue %d\n", idZ, activity_type);
}

void leave_queue(fifo_queue_t *queue, int activity_type, int idZ) {
    int size_of_queue = queue->activity[activity_type-1].people_in_queue;
    int *array_of_customers = queue->activity[activity_type-1].array_of_customers;

    // Find position of customer in queue.
    int position = 0;
    for (int i = 0; i < size_of_queue; i++) {
        if (array_of_customers[i] == idZ) {
            position = i;
            break;
        }
    }

    // Move all customers after customer in queue one position forward.
    for (int i = position; i < size_of_queue - 1; i++) {
        array_of_customers[i] = array_of_customers[i+1];
    }

    // Number of people in queue decreases.
    queue->activity[activity_type-1].people_in_queue--;

    print(output_file, "customer %d: leaves queue %d\n", idZ, activity_type);
}

fifo_queue_t *init_queue() {
    fifo_queue_t *queue = malloc(sizeof(fifo_queue_t));
    for (int i = 0; i < 3; i++) {
        queue->activity[i].people_in_queue = 0;
        queue->activity[i].capacity = 10;
        queue->activity[i].array_of_customers = malloc(queue->activity[i].capacity * sizeof(int));
        if (queue->activity[i].array_of_customers == NULL) {
            fprintf(stderr, "Error: failed allocating memory.\n");
            cleanup();
            exit(1);
        }
    }
    return queue;
}

void destroy_queue(fifo_queue_t *queue) {
    for (int i = 0; i < 3; i++) {
        free(queue->activity[i].array_of_customers);
    }
    free(queue);
}

//====================================================================================================