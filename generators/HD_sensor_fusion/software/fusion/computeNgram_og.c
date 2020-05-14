#include <string.h>
#include "util.h"
#include "fusion_funcs.h"
#include "init.h"
#include "mems_early.h"

#include "vec-util.h"

void computeNgram_og(int channels, int cntr_bits, float buffer[], uint64_t iM[][bit_dim + 1], uint64_t projM_pos[][bit_dim + 1], uint64_t projM_neg[][bit_dim + 1], uint64_t query[bit_dim + 1]){
/*************************************************************************
    DESCRIPTION: computes the N-gram

    INPUTS:
        buffer   :  input data
        iM       :  Item Memory for the IDs of channels
        chAM     :  Continuous Item Memory for the values of a channel
    OUTPUTS:
        query    :  query hypervector
**************************************************************************/

    #if PROFILE == 1
        uint64_t cpu_start = read_cycles();
    #endif

    uint64_t chHV2;
    uint64_t chHV[bit_dim + 1];

    //int cntr_init = (1 << (cntr_bits-1)) - (channels+1)/2 - 1;
    //uint64_t cntr[cntr_bits];
    //uint64_t temp, carry;

    //Spatial Encoder: captures the spatial information for a given time-aligned samples of channels
    for(int i = 0; i < bit_dim + 1; i++){

        //fastest componentwise majority: bit serial, word parallel
        //initialize counter to 2^(cntr_bits)/2 - channels/2
        //that way, the counter msb uint becomes the final query HV

        //for(int n = 0; n < cntr_bits; n++) {
            //if ((cntr_init  & (1 << n)) == 0) {
            //    cntr[n] = 0;
            //} else {
            //    cntr[n] = 0xFFFFFFFFFFFFFFFFULL;
            //}
        //}
        //printf("YAY\n");
        for(int j = 0; j < channels+1; j++) {

            // calc chHV
            if(j == channels) {
                chHV[i] ^= chHV2;
            } else {
                // slight hit if we don't check against 0 exactly also
                //chHV = buffer[j] == 0.0 ? iM[j][i] : (iM[j][i] ^ (buffer[j] > 0.0 ? projM_pos[j][i] : projM_neg[j][i]));
                chHV[i] = iM[j][i] ^ (buffer[j] >= 0.0 ? projM_pos[j][i] : projM_neg[j][i]);
            }

            if(j == 1) chHV2 = chHV[i];

            // incremental popcount
            //carry = cntr[0] & chHV;
            //cntr[0] ^= chHV; 
            //for(int n = 1; n < cntr_bits; n++) {
            //    temp = cntr[n];
            //    cntr[n] ^= carry;
            //    carry &= temp;
            //}
        }
        //printf("YAY2\n");
        uint64_t majority = 0;
        //componentwise majority: inserts the value of the i-th bit of each chHV row in the variable "majority"
        //and then computes the number of 1's with the function numberOfSetBits.
        for(int z = 63; z >= 0; z--){

            for(int j = 0 ; j < channels + 1; j++){

                majority = majority | (((chHV[i] & ( 1 << z)) >> z) << j);

            }
            if (numberOfSetBits(majority) > (channels/2)) query[i] = query[i] | ( 1 << z ) ;

            majority = 0;
        }

    }

    #if PROFILE == 1
        printf("spatial encoding cycles: %llu\n", read_cycles() - cpu_start);
    #endif

}