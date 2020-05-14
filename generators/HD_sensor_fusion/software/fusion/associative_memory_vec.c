#include "associative_memory_vec.h"
#include "fusion_funcs.h"
//#include "fusion_funcs_vec.h"


int associative_memory_64bit_vec(uint64_t q_64[bit_dim + 1], uint64_t aM_64[][bit_dim + 1]){
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
	vec_hamming_distance(q_64, aM_64, sims);
 
	//Classification with Hamming Metri
	class = min_dist_hamm(sims);

 

	return class;

}


