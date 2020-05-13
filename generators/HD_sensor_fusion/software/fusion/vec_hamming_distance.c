#include <string.h>
#include "util.h"
#include "fusion_funcs.h"
#include "init.h"
#include "mems_early.h"

#include "vec-util.h"

void vec_hamming_distance(uint64_t q[bit_dim + 1], uint64_t aM[][bit_dim + 1], int sims[classes]){
/**************************************************************************
    DESCRIPTION: computes the Hamming Distance for each class.

    INPUTS:
        q        : query hypervector
        aM       : Associative Memory matrix

    OUTPUTS:
        sims     : Distances' vector
***************************************************************************/

    int r_tmp = 0;
    void * AM_addr;
    uint64_t tmp[bit_dim + 1] = 0;
    int consumed;
    //CHANGE NUMBER OF VECTOR REGISTERS NEEDED
    asm volatile ("vsetcfg %0" : : "r" (VCFG(2, 0, 0, 1)));
    //uint64_t one = 0x1ULL;
    //asm volatile ("vmcs vs1, %0" : : "r" (one));

    for(int i = 0; i < classes; i++){
        for(int j = 0; j < bit_dim+1; ){
            asm volatile ("vsetvl %0, %1" : "=r" (consumed) : "r" (bit_dim+1-j));
            asm volatile ("vmca va0, %0" : : "r" (aM[i][j]));
            asm volatile ("vmca va1, %0" : : "r" (q[j]));
            asm volatile ("vmca va2, %0" : : "r" (tmp[j]));
            asm volatile ("la %0, AM_v" : "=r" (AM_addr));
            asm volatile ("vf 0(%0)" : : "r" (AM_addr));
            //tmp = q[j] ^ aM[i][j];
            j += consumed;
        }
        for(int j = 0; j < bit_dim+1; j++){
            r_tmp += numberOfSetBits(tmp[j]);
        }
        sims[i] = r_tmp;
        r_tmp = 0;
    }

    //int r_tmp = 0;

    //uint64_t tmp = 0;
    //for(int i = 0; i < classes; i++){
    //    for(int j = 0; j < bit_dim + 1; j++){

    //        tmp = q[j] ^ aM[i][j];

    //        r_tmp += numberOfSetBits(tmp);
    //    }
    //    sims[i] = r_tmp;
    //    r_tmp = 0;
    //}
}

