#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "util.h"
#include "fusion_funcs.h"
#include "associative_memory.h"
#include "associative_memory_vec.h"
#include "init.h"
#include "vec-util.h"
//the data.h and mems_<early/late>.h file can be created directly in MATLAB (after the simulation)
//using the function "data_file_creator.m"
#include "data.h"
#include "mems_early.h"

int main(){
    //initialize
    int class;
    uint64_t q[bit_dim + 1];
    for (int y=0; y<bit_dim+1; y++){
        q[y] = projM_pos_GSR[y];
    }
    //CPU associative memory
    #if PROFILE == 1
        uint64_t AM_start = read_cycles();
    #endif
    class = associative_memory_64bit(q, aM);
    #if PROFILE == 1
        printf("AM cycles cpu: %llu\n", read_cycles() - AM_start);
        AM_start = read_cycles();
    #endif
    //}

    //Hwacha associative memory
    class = associative_memory_64bit_vec(q, aM);
    #if PROFILE == 1
        printf("AM cycles Hwacha: %llu\n", read_cycles() - AM_start);
    #endif

    return 0; 
}
