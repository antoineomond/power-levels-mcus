.section .text.benchmark_mat_mul_float_asm_nosf, "ax"                              # Code segment
.global benchmark_mat_mul_float_asm_nosf                 # Make this label visible to the linker
.type benchmark_mat_mul_float_asm_nosf, @function        # Declare label type: function

# float benchmark_mat_mul_float_asm_nosf(uint32_t size)
#   s2 = size, s7 = A, s6 = B, s5 = C, 12(sp) = volatile float checksum (returned in a0)
.balign 4
benchmark_mat_mul_float_asm_nosf:
	addi	sp,sp,-64
	sw	ra,60(sp)
	sw	s0,56(sp)
	sw	s1,52(sp)
	sw	s2,48(sp)
	sw	s3,44(sp)
	sw	s4,40(sp)
	sw	s5,36(sp)
	sw	s6,32(sp)
	sw	s7,28(sp)
	sw	s8,24(sp)
	sw	s9,20(sp)
	sw	s10,16(sp)
	mv	s2,a0
	li	a5,0
	sw	a5,12(sp)
	li	a0,42
	call	srand
	mul	s0,s2,s2
	slli	s0,s0,0x2
	mv	a0,s0
	call	malloc
	mv	s7,a0
	mv	a0,s0
	call	malloc
	mv	s6,a0
	mv	a0,s0
	call	malloc
	mv	s5,a0
	li	s8,0
	j	.Linit_i_cond
.Linit_j_body:
	call	rand
	call	__floatsisf
	mul	s0,s4,s2
	add	s0,s0,s1
	slli	s0,s0,0x2
	add	s9,s7,s0
	lui	s3,%hi(.LC_rand_scale)
	lw	a1,%lo(.LC_rand_scale)(s3)		# 1.0f/(float)(RAND_MAX/16)
	call	__mulsf3
	sw	a0,0(s9)
	call	rand
	call	__floatsisf
	add	s9,s6,s0
	lw	a1,%lo(.LC_rand_scale)(s3)		# 1.0f/(float)(RAND_MAX/16)
	call	__mulsf3
	sw	a0,0(s9)
	add	s0,s0,s5
	li	a5,0
	sw	a5,0(s0)
	addi	s1,s1,1
.Linit_j_cond:
	bltu	s1,s2,.Linit_j_body
	addi	s8,s8,1
.Linit_i_cond:
	mv	s4,s8
	bgeu	s8,s2,.Linit_done
	li	s1,0
	j	.Linit_j_cond
.Linit_done:
	li	a0,300
	call	vTaskDelay
	li	a1,1
	li	a0,5
	call	set_gpio
	li	s10,0
	j	.Lmul_i_cond
.Lmul_k_body:
	mul	a5,s8,s2
	add	s0,a5,s3
	slli	s0,s0,0x2
	add	s0,s0,s5
	lw	s4,0(s0)
	add	a5,a5,s1
	slli	a5,a5,0x2
	add	a5,a5,s7
	mul	a4,s1,s2
	add	a4,a4,s3
	slli	a4,a4,0x2
	add	a4,a4,s6
	lw	a1,0(a4)
	lw	a0,0(a5)
	#call	__mulsf3
	mv	a1,a0
	mv	a0,s4
	#call	__addsf3
	sw	a0,0(s0)
	addi	s1,s1,1
.Lmul_k_cond:
	bltu	s1,s2,.Lmul_k_body
	addi	s9,s9,1
.Lmul_j_cond:
	mv	s3,s9
	bgeu	s9,s2,.Lmul_j_done
	li	s1,0
	j	.Lmul_k_cond
.Lmul_j_done:
	addi	s10,s10,1
.Lmul_i_cond:
	mv	s8,s10
	bgeu	s10,s2,.Lmul_done
	li	s9,0
	j	.Lmul_j_cond
.Lmul_done:
	li	a1,0
	li	a0,5
	call	set_gpio
	li	a5,0
	sw	a5,12(sp)
	li	s3,0
	j	.Lsum_i_cond
.Lsum_j_body:
	mul	a5,s1,s2
	add	a5,a5,s0
	slli	a5,a5,0x2
	add	a5,a5,s5
	lw	a1,12(sp)
	lw	a0,0(a5)
	call	__addsf3
	sw	a0,12(sp)
	addi	s0,s0,1
.Lsum_j_cond:
	bltu	s0,s2,.Lsum_j_body
	addi	s3,s3,1
.Lsum_i_cond:
	mv	s1,s3
	bgeu	s3,s2,.Lsum_done
	li	s0,0
	j	.Lsum_j_cond
.Lsum_done:
	mv	a0,s7
	call	free
	mv	a0,s6
	call	free
	mv	a0,s5
	call	free
	lw	a0,12(sp)
	lw	ra,60(sp)
	lw	s0,56(sp)
	lw	s1,52(sp)
	lw	s2,48(sp)
	lw	s3,44(sp)
	lw	s4,40(sp)
	lw	s5,36(sp)
	lw	s6,32(sp)
	lw	s7,28(sp)
	lw	s8,24(sp)
	lw	s9,20(sp)
	lw	s10,16(sp)
	addi	sp,sp,64
	ret
.size benchmark_mat_mul_float_asm_nosf, .-benchmark_mat_mul_float_asm_nosf

.section .rodata.benchmark_mat_mul_float_asm_nosf, "a"
.balign 4
.LC_rand_scale:
	.word	0x32000000		# 2^-27 = 1.0f/(float)(RAND_MAX/16), RAND_MAX = 0x7fffffff
