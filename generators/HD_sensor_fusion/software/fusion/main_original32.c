#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "associative_memory32.h"
#include "fusion_funcs32.h"
#include "init.h"
//the data.h and mems_<early/late>.h file can be created directly in MATLAB (after the simulation)
//using the function "data_file_creator.m"
#include "data.h"
#include "mems_early.h"
//#include "mems_late.h"

int main(){
    uint32_t total_start = read_cycles32();
 
    float buffer[channels_EEG]; //EEG has the most channels
          
	uint32_t overflow = 0;
	uint32_t old_overflow = 0;
	uint32_t mask = 1;
	uint32_t q[N][bit_dim + 1];
    uint32_t q_GSR[bit_dim + 1], q_ECG[bit_dim+1], q_EEG[bit_dim+1] = {0};
	int class;
    int overflow_bits = dimension % 32;

    int numTests = 0;
    int correct = 0;

	//Spatial and Temporal Encoder: computes the N-gram.
	//N.B. if N = 1 we don't have the Temporal Encoder but only the Spatial Encoder.
    #if PROFILE == 1
        uint32_t spatial_start = read_cycles32();
    #endif

    //spatially encode first N samples
	for(int z = 0; z < N; z++){
        memcpy(buffer, TEST_SET_GSR[z], sizeof(TEST_SET_GSR[z]));
        computeNgram_og32(channels_GSR, cntr_bits_GSR, buffer, iM32_EEG, projM32_pos_GSR, projM32_neg_GSR, q_GSR);

        memcpy(buffer, TEST_SET_ECG[z], sizeof(TEST_SET_ECG[z]));
        computeNgram_og32(channels_ECG, cntr_bits_ECG, buffer, iM32_EEG, projM32_pos_ECG, projM32_neg_ECG, q_ECG);

        memcpy(buffer, TEST_SET_EEG[z], sizeof(TEST_SET_EEG[z]));
        computeNgram_og32(channels_EEG, cntr_bits_EEG, buffer, iM32_EEG, projM32_pos_EEG, projM32_neg_EEG, q_EEG);
        //printf("YAYendof1\n");
        //majority
        for (int b = bit_dim; b >= 0; b--) {
            q[z][b] = (q_GSR[b] & q_ECG[b]) | (q_ECG[b] & q_EEG[b]) | (q_EEG[b] & q_GSR[b]);
        }
    }

    #if PROFILE == 1
        printf("Initial Spatial cycles: %llu\n", read_cycles32() - spatial_start);
    #endif

	for(int ix = 0; ix < 30-N+1; ix++){
        #if PROFILE == 1
            uint32_t temporal_start = read_cycles32();
        #endif

        numTests++;

        #if N > 1
        //temporal encode
		for(int z = 1; z < N; z++){
            
            #if temporal_shift == 32
            //Here the hypervector q[0] is shifted by 64 bits as permutation (no circularity),
			//before performing the componentwise XOR operation with the new query (q[z]).
            //Much more hardware optimal!
            for(int b = bit_dim; b >= 0; b--){
                //if ((b % 32) == 0)
                //    q[0][b] = q[z][b] ^ 0ULL;
                //else
                    q[0][b] = q[z][b] ^ q[0][b-1];
            }

            #else
			//Here the hypervector q[0] is shifted by 1 position as permutation,
			//before performing the componentwise XOR operation with the new query (q[z]).
			//overflow = q[0][0] & mask;

			for(int i = 1; i < bit_dim; i++){

				old_overflow = overflow;
				overflow = q[0][i] & mask;
				q[0][i] = (q[0][i] >> 1) | (old_overflow << (32 - 1));
                q[0][i] = (q[0][i] >> 1);
				q[0][i] = q[z][i] ^ q[0][i];

			}

			old_overflow = overflow;
			overflow = (q[0][bit_dim] >> overflow_bits) & mask;
			q[0][bit_dim] = (q[0][bit_dim] >> 1) | (old_overflow << (32 - 1));
			q[0][bit_dim] = q[z][bit_dim] ^ q[0][bit_dim];

			q[0][0] = (q[0][0] >> 1) | (overflow << (32 - 1));
			q[0][0] = q[z][0] ^ q[0][0];
            #endif
 
		}
        #endif
	
        #if PROFILE == 1
            printf("Temporal cycles: %llu\n", read_cycles32() - temporal_start);

            uint32_t assoc_start = read_cycles32();
        #endif
 
	    //classifies the new N-gram through the Associative Memory matrix.
        class = associative_memory_32bit(q[0], aM32);

        #if PROFILE == 1
            printf("Assoc. Mem. cycles: %llu\n", read_cycles32() - assoc_start);
        #endif

        if (class == labels[ix]) correct++;
	
        #if PROFILE == 1
 	        printf("Sample %d (predicted, golden) class: (%d, %d)\n", ix, class, labels[ix]);
        #endif

        if (ix < 30-N) {
            //Move forward by updating q and spatially encoding ix+Nth sample
            #if PROFILE == 1
                spatial_start = read_cycles32();
            #endif

            #if N > 1
            for(int z = 0; z < N; z++) {
                memcpy(buffer, TEST_SET_GSR[ix+z+1], sizeof(TEST_SET_GSR[ix+z+1]));
                computeNgram_og32(channels_GSR, cntr_bits_GSR, buffer, iM32_EEG, projM32_pos_GSR, projM32_neg_GSR, q_GSR);
                memcpy(buffer, TEST_SET_ECG[ix+z+1], sizeof(TEST_SET_ECG[ix+z+1]));
                computeNgram_og32(channels_ECG, cntr_bits_ECG, buffer, iM32_EEG, projM32_pos_ECG, projM32_neg_ECG, q_ECG);
                memcpy(buffer, TEST_SET_EEG[ix+z+1], sizeof(TEST_SET_EEG[ix+z+1]));
                computeNgram_og32(channels_EEG, cntr_bits_EEG, buffer, iM32_EEG, projM32_pos_EEG, projM32_neg_EEG, q_EEG);
                for (int b = bit_dim; b >= 0; b--) {
                    q[z][b] = (q_GSR[b] & q_ECG[b]) | (q_ECG[b] & q_EEG[b]) | (q_EEG[b] & q_GSR[b]);
                }   
                //memcpy(q[z], q[z+1], sizeof(q[z]));
            }
            #endif

            //majority

            #if PROFILE == 1
                printf("Spatial update cycles: %llu\n", read_cycles32() - spatial_start);
            #endif

        }

	}

    printf("Total cycles: %llu\n", read_cycles32() - total_start);

    // accuracy count (can't print % since can't print floats in RISC-V)
    printf("Correct: %d out of %d\n", correct, numTests); 

    return 0; 
}

