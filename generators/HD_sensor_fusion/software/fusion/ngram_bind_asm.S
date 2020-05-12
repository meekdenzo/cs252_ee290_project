## Hwacha computeNgram code

.text

.globl ngram_bind_v
.globl xor_ngram_v


#

.align 3
ngram_bind_v:
	vpset vp0
	vld vv0, va0			# q[0], ngram 0
	vld vv1, va1			# q[1], ngram 1
	vld vv2, va2			# q[2], ngram 2
	vsrl vv0, vv0, vs1		# permute: q[0] >> 1, logical fill w/0s every 64 bits
	vxor vv0, vv0, vv1		# q[0] = permutedd(q[0]) ^ q[1]
	vsrl vv0, vv0, vs1		# permute: q[0] >> 1, logical fill w/0s every 64 bits
	vxor vv0, vv0, vv2		# q[0] = permutedd(q[0]) ^ q[z]
	vsd vv0, va0
	vstop

#.align 3
xor_ngram_v:
	vpset vp0
	vld vv0, va0
	vld vv1, va1
	vxor vv2, vv0, vv1
	vsd vv2, va2
	vstop