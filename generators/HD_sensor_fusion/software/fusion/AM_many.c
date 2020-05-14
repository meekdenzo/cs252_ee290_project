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
    int cycle = 10;
    int class;
    uint64_t q[2*cycle][bit_dim + 1];
    for (int x=0; x<(2*N); x++) {
        for (int y=0; y<bit_dim+1; y++){
            q[x][y] = projM_pos_GSR[x][y];
        }
    }
    //CPU associative memory
    #if PROFILE == 1
        uint64_t AM_start = read_cycles();
    #endif
    for (int x=0; x<cycle; x++) {
        class = associative_memory_64bit(q[x], aM);
        printf("%d\n", x);
    }
    #if PROFILE == 1
        printf("AM cycles cpu: %llu\n", read_cycles() - AM_start);
        AM_start = read_cycles();
    #endif
    //}

    //Hwacha associative memory
    for (int y=cycle; y<(2*cycle); y++) {
        class = associative_memory_64bit_vec(q[x], aM);
        printf("%d\n", y);
    }
    #if PROFILE == 1
        printf("AM cycles Hwacha: %llu\n", read_cycles() - AM_start);
    #endif

    return 0; 
}
