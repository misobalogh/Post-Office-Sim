/***************************************************************
* File Name: htab.h
* Description: riesnie IJC-DU2, priklad b) datova struktura htab_t
* Author: MICHAL BALOGH <xbalog06@stud.fit.vutbr.cz
* Faculty: FIT VUT
* Date: 28.03.2023

* Comments: prekladane pomocou gcc 9.4.0
***************************************************************/

#include "proj2.h"

int main(int argc, char *argv[])
{
    int NZ, NU, TZ, TU, F;
    if(parse_args(argc, argv, &NZ, &NU, &TZ, &TU, &F) == 1)
    {   
        // Invalid Arguments
        return 1;
    }

    if (open_file(&output_file) != 0)
    {
        // Error opening file
        return 1;
    }
    fifo_queue_t *queue;
    queue = init_queue();
    
    customer(1, queue, 1);
    customer(2, queue, 1);
    customer(3, queue, 1);
    customer(4, queue, 3);
    
    get_in_queue(queue, 1, 1);
    get_in_queue(queue, 2, 1);
    get_in_queue(queue, 3, 1);
    get_in_queue(queue, 3, 2);
    leave_queue(queue, 3, 2);
    leave_queue(queue, 3, 3);
    get_in_queue(queue, 3, 3);
    get_in_queue(queue, 1, 4);

    destroy_queue(queue);

    cleanup();

    return 0;
}