#include <stdio.h>
#include <math.h>
#include <string.h>
#include "init.h"
#include "fusion_funcs32.h"

#define ROUND(num) ((num - floorf(num) > 0.5f) ? ceilf(num) : floorf(num))

uint32_t read_cycles32() {
    uint32_t cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}

int min_dist_hamm32(int distances[classes]){
/*************************************************************************
	DESCRIPTION: computes the minimum Hamming Distance.

	INPUTS:
		distances     : distances associated to each class
	OUTPUTS:
		min_index     : the class related to the minimum distance
**************************************************************************/
	int min = distances[0];
	int min_index = 0;

	for(int i = 1; i < classes; i++){

		if(min > distances[i]){

			min = distances[i];
			min_index = i;

		}

	}

	return min_index;
}



void hamming_dist32(uint32_t q[bit_dim + 1], uint32_t aM[][bit_dim + 1], int sims[classes]){
/**************************************************************************
	DESCRIPTION: computes the Hamming Distance for each class.

	INPUTS:
		q        : query hypervector
		aM		 : Associative Memory matrix

	OUTPUTS:
		sims	 : Distances' vector
***************************************************************************/

	int r_tmp = 0;

	uint32_t tmp = 0;
	for(int i = 0; i < classes; i++){
		for(int j = 0; j < bit_dim + 1; j++){

			tmp = q[j] ^ aM[i][j];

			r_tmp += numberOfSetBits(tmp);
		}
		sims[i] = r_tmp;
		r_tmp = 0;
	}
}

void computeNgram32(int channels, int cntr_bits, float buffer[], uint32_t iM[][bit_dim + 1], uint32_t projM_pos[][bit_dim + 1], uint32_t projM_neg[][bit_dim + 1], uint32_t query[bit_dim + 1]){
/*************************************************************************
	DESCRIPTION: computes the N-gram

	INPUTS:
		buffer   :  input data
		iM       :	Item Memory for the IDs of channels
		chAM     :  Continuous Item Memory for the values of a channel
	OUTPUTS:
		query    :  query hypervector
**************************************************************************/

    #if PROFILE == 1
        uint32_t cpu_start = read_cycles32();
    #endif

    uint32_t chHV, chHV2;

    int cntr_init = (1 << (cntr_bits-1)) - (channels+1)/2 - 1;
    uint32_t cntr[cntr_bits];
    uint32_t temp, carry;

	//Spatial Encoder: captures the spatial information for a given time-aligned samples of channels
	for(int i = 0; i < bit_dim + 1; i++){

        //fastest componentwise majority: bit serial, word parallel
        //initialize counter to 2^(cntr_bits)/2 - channels/2
        //that way, the counter msb uint becomes the final query HV

        for(int n = 0; n < cntr_bits; n++) {
            if ((cntr_init  & (1 << n)) == 0) {
                cntr[n] = 0;
            } else {
                //cntr[n] = 0xFFFFFFFFFFFFFFFFULL;
            }
        }

        for(int j = 0; j < channels+1; j++) {

            // calc chHV
            if(j == channels) {
                chHV ^= chHV2;
            } else {
                // slight hit if we don't check against 0 exactly also
                //chHV = buffer[j] == 0.0 ? iM[j][i] : (iM[j][i] ^ (buffer[j] > 0.0 ? projM_pos[j][i] : projM_neg[j][i]));
                chHV = iM[j][i] ^ (buffer[j] >= 0.0 ? projM_pos[j][i] : projM_neg[j][i]);
            }
            if(j == 1) chHV2 = chHV;

            // incremental popcount
            carry = cntr[0] & chHV;
            cntr[0] ^= chHV; 
            for(int n = 1; n < cntr_bits; n++) {
                temp = cntr[n];
                cntr[n] ^= carry;
                carry &= temp;
            }
        }
        
        query[i] = cntr[cntr_bits-1];

	}

    #if PROFILE == 1
        printf("spatial encoding cycles: %llu\n", read_cycles32() - cpu_start);
    #endif

}

int numberOfSetBits32(uint32_t i)
{
/*************************************************************************
    DESCRIPTION:   computes the number of 1's

    INPUTS:
        i        :  the i-th variable that composes the hypervector

**************************************************************************/

     i = i - ((i >> 1) & 0x55555555);
     i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
     return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}
