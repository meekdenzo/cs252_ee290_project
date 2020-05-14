#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "util.h"
#include "fusion_funcs.h"
#include "init.h"
#include "vec-util.h"
//the data.h and mems_<early/late>.h file can be created directly in MATLAB (after the simulation)
//using the function "data_file_creator.m"
#include "data.h"
#include "mems_early.h"
//#include "mems_late.h"

int main(){
 
    //float buffer[channels_EEG];
    uint64_t q[N][bit_dim + 1];
	//uint64_t q_cpu[bit_dim + 1], q_hwacha[bit_dim+1];
    for(int z = 0; z < N; z++){
        for (int y=0; y<bit_dim+1; y++){
            q[z][y] = projM_pos_GSR[z][y];
        }
    }
    //CPU 64-bit shift temporal encoder
    #if PROFILE == 1
        uint64_t temporal_start = read_cycles();
    #endif
    //for(int ix = 0; ix < NUMBER_OF_INPUT_SAMPLES-N+1; ix++){
    for(int z = 1; z < N; z++){
        //Here the hypervector q[0] is shifted by 64 bits as permutation (no circularity),
        //before performing the componentwise XOR operation with the new query (q[z]).
        //Much more hardware optimal!
        for(int b = bit_dim; b >= 0; b--){
            q[0][b] = ((b==0) ? (q[z][b] ^ 0ULL) : (q[z][b] ^ q[0][b-1]));
        }
    }
    #if PROFILE == 1
        printf("Temporal cycles cpu 64 bit shift: %llu\n", read_cycles() - temporal_start);
    #endif
    //}

    for(int z = N; z < N*2; z++){
        for (int y=0; y<bit_dim+1; y++){
            q[z][y] = projM_pos_GSR[z][y];
        }
    }
    //CPU bit shift temporal encoder
    #if PROFILE == 1
        uint64_t temporal_start = read_cycles();
    #endif
    //for(int ix = 0; ix < NUMBER_OF_INPUT_SAMPLES-N+1; ix++){
    for(int z = N+1; z < 2*N; z++){
        //Here the hypervector q[0] is shifted by 1 bits as permutation (no circularity),
        //before performing the componentwise XOR operation with the new query (q[z]).
        //Much more hardware optimal!
        for(int b = bit_dim; b >= 0; b--){
            q[0][i] = (q[0][i] >> 1);
            q[0][i] = q[z][i] ^ q[0][i];
        }
    }
    #if PROFILE == 1
        printf("Temporal cycles cpu 1-bit shift: %llu\n", read_cycles() - temporal_start);
    #endif
    //}

    //Hwacha temporal encoder

    //temporal encode hardcoded for N=3
    void * ngram_bind_addr;
    asm volatile ("vsetcfg %0" : : "r" (VCFG(N, 0, 0, 1)));
    uint64_t one = 0x1ULL;
    asm volatile ("vmcs vs1, %0" : : "r" (one));
    int consumed; 

    #if PROFILE == 1
        uint64_t temporal_start = read_cycles();
    #endif

    for(int b = 0; b < bit_dim+1; ){
        asm volatile ("vsetvl %0, %1" : "=r" (consumed) : "r" (bit_dim+1-b));
        asm volatile ("vmca va0, %0" : : "r" (&q[2*N][b]));
        asm volatile ("vmca va1, %0" : : "r" (&q[2*N+1][b]));
        asm volatile ("vmca va2, %0" : : "r" (&q[2*N+2][b]));
        asm volatile ("la %0, ngram_bind_v" : "=r" (ngram_bind_addr));
        asm volatile ("vf 0(%0)" : : "r" (ngram_bind_addr));
        b += consumed;
    }
    
    #if PROFILE == 1
        printf("Temporal cycles Hwacha: %llu\n", read_cycles() - temporal_start);
    #endif

    return 0; 
}
