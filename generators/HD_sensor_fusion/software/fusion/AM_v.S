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
	vld vv0, va2			# vv0 = tmp[j]
	vsrl vv1, vv0, vs1		# vv1 = i >> 1
	vand vv1, vv1, vs2		# vv1 = (i >> 1) & const5
	vsub vv0, vv0, vv1		# i = i - (i >> 1) & const5
	vand vv1, vv0, vs3		# vv1 = (i & const3)
	vsrl vv2, vv0, vs4		# vv2 = (i >> const4)
	vand vv2, vv2, vs3		# vv2 = ((i >> const4) & const3)
	vadd vv0, vv1, vv2		# i = (i & const3) + ((i >> const4) & const3)
	vsrl vv1, vv0, vs5		# vv1 = (i >> const5)
	vadd vv0, vv0, vv1		# i = (i & const5) + i
	vand vv0, vv0, vs6		# vv0 = ((i >> const4) & const6)
	vsd vv0, va2			# vv0 = tmp[j]
	vstop

	#vsrl vv0, vv0, vs1		# permute: q[0] >> 1, logical fill w/0s every 64 bits
	#vsrl vv0, vv0, vs1		# permute: q[0] >> 1, logical fill w/0s every 64 bits
	#vxor vv0, vv0, vv2		# vv3 = permuted(q[0]) ^ q[2]
	#vsd vv0, va0			# write vv3 into q[0]
