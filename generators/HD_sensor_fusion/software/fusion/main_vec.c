#include <stdio.h>
#include <stdint.h>
#include <string.h>
//#include "associative_memory.h"
#include "associative_memory_vec.h"
#include "fusion_funcs.h"
//#include "fusion_funcs_vec.h"
#include "vec_majority_3.h"
#include "init.h"
#include "util.h"
#include "vec-util.h"
//the data.h and mems_<early/late>.h file can be created directly in MATLAB (after the simulation)
//using the function "data_file_creator.m"
#include "data.h"
#include "mems_early.h"
//#include "mems_late.h"

int main(){
    uint64_t total_start = read_cycles();
 
    float buffer[channels_EEG]; //EEG has the most channels
          
	uint64_t overflow = 0;
	uint64_t old_overflow = 0;
	uint64_t mask = 1;
	uint64_t q[N][bit_dim + 1];
    uint64_t xor_cpu[bit_dim + 1];
    uint64_t xor_hwacha[bit_dim + 1];
    uint64_t q_GSR[bit_dim + 1], q_ECG[bit_dim+1], q_EEG[bit_dim+1] = {0};
	int class;
    int overflow_bits = dimension % 64;

    int numTests = 0;
    int correct = 0;

	//Spatial and Temporal Encoder: computes the N-gram.
	//N.B. if N = 1 we don't have the Temporal Encoder but only the Spatial Encoder.
    #if PROFILE == 1
        uint64_t spatial_start = read_cycles();
    #endif

    //spatially encode first N samples
	for(int z = 0; z < N; z++){
        memcpy(buffer, TEST_SET_GSR[z], sizeof(TEST_SET_GSR[z]));
        vec_computeNgram(channels_GSR, cntr_bits_GSR, buffer, iM_EEG, projM_pos_GSR, projM_neg_GSR, q_GSR);

        memcpy(buffer, TEST_SET_ECG[z], sizeof(TEST_SET_ECG[z]));
        vec_computeNgram(channels_ECG, cntr_bits_ECG, buffer, iM_EEG, projM_pos_ECG, projM_neg_ECG, q_ECG);

        memcpy(buffer, TEST_SET_EEG[z], sizeof(TEST_SET_EEG[z]));
        vec_computeNgram(channels_EEG, cntr_bits_EEG, buffer, iM_EEG, projM_pos_EEG, projM_neg_EEG, q_EEG);

        //majority
        vec_majority_3_asm(bit_dim+1, q[z], q_GSR, q_ECG, q_EEG);
    }

    #if PROFILE == 1
        printf("Initial Spatial cycles: %llu\n", read_cycles() - spatial_start);
    #endif

	for(int ix = 0; ix < 10-N+1; ix++){

        #if PROFILE == 1
            uint64_t temporal_start = read_cycles();
        #endif

        numTests++;

        #if N > 1
        //temporal encode hardcoded for N=3
            void * ngram_bind_addr;
            asm volatile ("vsetcfg %0" : : "r" (VCFG(N, 0, 0, 1)));
            uint64_t one = 0x1ULL;
            asm volatile ("vmcs vs1, %0" : : "r" (one));
            int consumed; 

            for(int b = 0; b < bit_dim+1; ){
                asm volatile ("vsetvl %0, %1" : "=r" (consumed) : "r" (bit_dim+1-b));
                asm volatile ("vmca va0, %0" : : "r" (&q[0][b]));
                asm volatile ("vmca va1, %0" : : "r" (&q[1][b]));
                asm volatile ("vmca va2, %0" : : "r" (&q[2][b]));
                asm volatile ("la %0, ngram_bind_v" : "=r" (ngram_bind_addr));
                asm volatile ("vf 0(%0)" : : "r" (ngram_bind_addr));
                b += consumed;
            }

        #endif
	
        #if PROFILE == 1
            printf("Temporal cycles: %llu\n", read_cycles() - temporal_start);

            uint64_t assoc_start = read_cycles();
        #endif
 
	    //classifies the new N-gram through the Associative Memory matrix.
        class = associative_memory_64bit_vec(q[0], aM);

        #if PROFILE == 1
            printf("Assoc. Mem. cycles: %llu\n", read_cycles() - assoc_start);
        #endif

        if (class == labels[ix]) correct++;
	
        #if PROFILE == 1
 	        printf("Sample %d (predicted, golden) class: (%d, %d)\n", ix, class, labels[ix]);
        #endif

        if (ix <10-N) {
            //Move forward by updating q and spatially encoding ix+Nth sample
            #if PROFILE == 1
                spatial_start = read_cycles();
            #endif

            #if N > 1
            for(int z = 0; z < N-1; z++) {
                memcpy(q[z], q[z+1], sizeof(q[z]));
            }
            #endif
            
            memcpy(buffer, TEST_SET_GSR[ix+N], sizeof(TEST_SET_GSR[ix+N]));
            vec_computeNgram(channels_GSR, cntr_bits_GSR, buffer, iM_EEG, projM_pos_GSR, projM_neg_GSR, q_GSR);

            memcpy(buffer, TEST_SET_ECG[ix+N], sizeof(TEST_SET_ECG[ix+N]));
            vec_computeNgram(channels_ECG, cntr_bits_ECG, buffer, iM_EEG, projM_pos_ECG, projM_neg_ECG, q_ECG);

            memcpy(buffer, TEST_SET_EEG[ix+N], sizeof(TEST_SET_EEG[ix+N]));
            vec_computeNgram(channels_EEG, cntr_bits_EEG, buffer, iM_EEG, projM_pos_EEG, projM_neg_EEG, q_EEG);

            //majority
            vec_majority_3_asm(bit_dim+1, q[N-1], q_GSR, q_ECG, q_EEG);

            #if PROFILE == 1
                printf("Spatial update cycles: %llu\n", read_cycles() - spatial_start);
            #endif
        }
    }
	

    printf("Total cycles: %llu\n", read_cycles() - total_start);

    // accuracy count (can't print % since can't print floats in RISC-V)
    printf("Correct: %d out of %d\n", correct, numTests); 

    return 0; 
}

