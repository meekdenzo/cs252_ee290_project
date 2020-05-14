#include "associative_memory32.h"
#include "fusion_funcs32.h"


int associative_memory_32bit(uint32_t q_32[bit_dim + 1], uint32_t aM_32[][bit_dim + 1]){
/*************************************************************************
	DESCRIPTION:  tests the accuracy based on input testing queries

	INPUTS:
		q_64        : query hypervector
		aM_64		: Trained associative memory
	OUYTPUTS:
		class       : classification result
**************************************************************************/

	int sims[classes] = {0};
	int class;


	//Computes Hamming Distances
	hamming_dist32(q_32, aM_32, sims);
 
	//Classification with Hamming Metri
	class = min_dist_hamm32(sims);

 

	return class;

}


