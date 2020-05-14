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

    int r_tmp0=0;
    int r_tmp1=0;
    void * hamming_addr;
    void * popcount_addr;
    uint64_t tmp0[bit_dim + 1];
    uint64_t tmp1[bit_dim + 1];
    int consumed;
    int count=0;
    uint64_t i;
    //CHANGE NUMBER OF VECTOR REGISTERS NEEDED
    asm volatile ("vsetcfg %0" : : "r" (VCFG(3, 0, 0, 1)));

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
    asm volatile ("vmcs vs2, %0" : : "r" (const2));
    asm volatile ("vmcs vs3, %0" : : "r" (const3));
    asm volatile ("vmcs vs4, %0" : : "r" (const4));
    asm volatile ("vmcs vs5, %0" : : "r" (const5));
    asm volatile ("vmcs vs6, %0" : : "r" (const6));
    asm volatile ("vmcs vs7, %0" : : "r" (const7));
    asm volatile ("vmcs vs8, %0" : : "r" (const8));


    //for(int y = 0; y < classes; y++){
        for(int j = 0; j < bit_dim+1; ){
            asm volatile ("vsetvl %0, %1" : "=r" (consumed) : "r" (bit_dim+1-j));
            asm volatile ("vmca va0, %0" : : "r" (&aM[0][j]));
            asm volatile ("vmca va1, %0" : : "r" (&q[j]));
            asm volatile ("vmca va2, %0" : : "r" (&tmp0[j]));
            asm volatile ("vmca va3, %0" : : "r" (&tmp1[j]));
            asm volatile ("vmca va4, %0" : : "r" (&aM[1][j]));
            asm volatile ("la %0, hamming_v" : "=r" (hamming_addr));
            asm volatile ("vf 0(%0)" : : "r" (hamming_addr));
            //asm volatile ("vsetvl %0, %1" : "=r" (consumed) : "r" (bit_dim+1-j));
            //asm volatile ("vmca va2, %0" : : "r" (&tmp[j]));
            //asm volatile ("la %0, popcount_v" : "=r" (popcount_addr));
            //asm volatile ("vf 0(%0)" : : "r" (popcount_addr));
            j += consumed;
        }
        for(int j = 0; j < bit_dim+1; j++){
            if (tmp0[j] == tmp1[j]) {
                count += 1;
            }
            r_tmp0 += tmp0[j];
            r_tmp1 += tmp1[j];
        }
        printf("%d\n", count);
        sims[0] = r_tmp0;
        sims[1] = r_tmp1;
    //}

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

