## Hwacha computeNgram code

.text

.globl hamming_v
#.globl xor_ngram_v


#

.align 3
hamming_v:
	vpset vp0
	vld vv0, va0			# vv0 = aM[i][j]
	vld vv1, va1			# vv1 = q[j]
	vxor vv0, vv0, vv1		# vv3 = permuted(q[0]) ^ q[1]
	vsd vv0, va2			# write vv0 into tmp
	vstop

.align 3
popcount_v:
	vpset vp0
	vld vv0, va0			# vv0 = aM[i][j]
	vld vv1, va1			# vv1 = q[j]
	vxor vv0, vv0, vv1		# vv3 = permuted(q[0]) ^ q[1]
	vsd vv0, va2			# write vv0 into tmp
	vstop

	#vsrl vv0, vv0, vs1		# permute: q[0] >> 1, logical fill w/0s every 64 bits
	#vsrl vv0, vv0, vs1		# permute: q[0] >> 1, logical fill w/0s every 64 bits
	#vxor vv0, vv0, vv2		# vv3 = permuted(q[0]) ^ q[2]
	#vsd vv0, va0			# write vv3 into q[0]
