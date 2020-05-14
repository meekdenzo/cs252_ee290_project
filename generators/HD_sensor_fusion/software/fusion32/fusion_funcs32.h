#ifndef FUSION_FUNCS_H_
#define FUSION_FUNCS_H_
 
#include "init32.h"

uint32_t read_cycles32();
void hamming_dist32(uint32_t q[bit_dim + 1], uint32_t aM[][bit_dim + 1], int sims[classes]);
int min_dist_hamm32(int distances[classes]);
void computeNgram32(int channels, int cntr_bits, float buffer[], uint32_t iM[][bit_dim + 1], uint32_t projM_pos[][bit_dim + 1], uint32_t projM_neg[][bit_dim + 1], uint32_t query[bit_dim + 1]);
int numberOfSetBits32(uint32_t i);

static int verifyuint32_t(int n, uint32_t* test, uint32_t* verify) {
    int i;
    // Unrolled for faster verification
    for (i = 0; i < n/2*2; i+=2) {
        uint32_t t0 = test[i], t1 = test[i+1];
        uint32_t v0 = verify[i], v1 = verify[i+1];
        int eq1 = t0 == v0, eq2 = t1 == v1;
        if (!(eq1 & eq2)) return i+1+eq1;
    }
    if (n % 2 != 0 && test[n-1] != verify[n-1])
        return n;
    return 0;
}

#endif
