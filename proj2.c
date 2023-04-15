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

    printf("%d\n", NZ);
    printf("%d\n", NU);
    printf("%d\n", TZ);
    printf("%d\n", TU);
    printf("%d\n", F);

    return 0;
}