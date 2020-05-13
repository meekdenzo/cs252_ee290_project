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
    void * hamming_addr;
    void * popcount_addr;
    uint64_t tmp[bit_dim + 1];
    int consumed;
    uint64_t i;
    //CHANGE NUMBER OF VECTOR REGISTERS NEEDED
    asm volatile ("vsetcfg %0" : : "r" (VCFG(2, 0, 0, 1)));

    //constants
    uint64_t const1 = 0x1ULL;
    uint64_t const2 = 0x5555555555555555;
    uint64_t const3 = 0x3333333333333333;
    uint64_t const4 = 0x2ULL;
    uint64_t const5 = 0x4ULL;
    uint64_t const6 = 0x0F0F0F0F0F0F0F0F;
    uint64_t const7 = 0x0101010101010101;
    uint64_t const8 = 0x38ULL;
    asm volatile ("vmcs vs1, %0" : : "r" (const1));


    for(int y = 0; y < classes; y++){
        for(int j = 0; j < bit_dim+1; ){
            asm volatile ("vsetvl %0, %1" : "=r" (consumed) : "r" (bit_dim+1-j));
            asm volatile ("vmca va0, %0" : : "r" (&aM[y][j]));
            asm volatile ("vmca va1, %0" : : "r" (&q[j]));
            asm volatile ("vmca va2, %0" : : "r" (&tmp[j]));
            asm volatile ("la %0, hamming_v" : "=r" (hamming_addr));
            asm volatile ("vf 0(%0)" : : "r" (hamming_addr));
            j += consumed;
        }
        for(int j = 0; j < bit_dim+1; j++){
            asm volatile ("vsetvl %0, %1" : "=r" (consumed) : "r" (bit_dim+1-j));
            i = tmp[j];
            i = i - ((i >> 1) & 0x5555555555555555);
            i = (i & 0x3333333333333333) + ((i >> 2) & 0x3333333333333333);
            r_tmp += ((((i + (i >> 4)) & 0x0F0F0F0F0F0F0F0F) * 0x0101010101010101) >> 56);
            //asm volatile ("la %0, popcount_v" : "=r" (popcount_addr));
            //asm volatile ("vf 0(%0)" : : "r" (popcount_addr));
            j += consumed;
        }
        sims[y] = r_tmp;
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

