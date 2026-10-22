.section .iram1.benchmark_mat_mul_asm_nomem, "ax"                              # Code segment
.global benchmark_mat_mul_asm_nomem                 # Make this label visible to the linker
.type benchmark_mat_mul_asm_nomem, @function        # Declare label type: function

# uint32_t benchmark_mat_mul_asm_nomem(uint32_t size)
#   s0 = size, s1 = C, s2 = B, s3 = A, 12(sp) = volatile checksum
.balign 4
benchmark_mat_mul_asm_nomem:
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
	mv	s0,a0
	sw	zero,12(sp)		# checksum = 0
	li	a0,42
	call	srand			# srand(SEED)
	mul	s1,s0,s0
	slli	s1,s1,0x2		# s1 = sizeof(uint32_t)*size*size
	mv	a0,s1
	call	malloc
	mv	s3,a0			# A
	mv	a0,s1
	call	malloc
	mv	s2,a0			# B
	mv	a0,s1
	call	malloc
	mv	s1,a0			# C

	# ---- Initialisation: A, B random, C = 0 ----
	li	s7,0			# i = 0
	j	.Linit_i_cond
.Linit_j_body:
	call	rand
	mul	s4,s6,s0
	add	s4,s4,s5
	slli	s4,s4,0x2		# s4 = (size*i + j) * 4
	add	a4,s3,s4
	srai	a5,a0,0x1f		# rand() % 256 (signed modulo)
	srli	a5,a5,0x18
	add	a0,a0,a5
	andi	a0,a0,255
	sub	a0,a0,a5
	sw	a0,0(a4)		# A[size*i + j]
	call	rand
	add	a4,s2,s4
	srai	a5,a0,0x1f
	srli	a5,a5,0x18
	add	a0,a0,a5
	andi	a0,a0,255
	sub	a0,a0,a5
	sw	a0,0(a4)		# B[size*i + j]
	add	s4,s4,s1
	sw	zero,0(s4)		# C[size*i + j] = 0
	addi	s5,s5,1			# j++
.Linit_j_cond:
	bltu	s5,s0,.Linit_j_body
	addi	s7,s7,1			# i++
.Linit_i_cond:
	mv	s6,s7
	bgeu	s7,s0,.Linit_done
	li	s5,0			# j = 0
	j	.Linit_j_cond

.Linit_done:
	li	a0,300
	call	vTaskDelay		# vTaskDelay(pdMS_TO_TICKS(3000))
	li	a1,1
	li	a0,5
	call	set_gpio		# set_gpio(EXPE_PIN, 1)

	# ---- C += A * B ----
	li	t1,0			# i = 0
	j	.Lmul_i_cond
.Lmul_k_body:
	mul	a5,a6,s0
	add	a4,a5,a0
	slli	a4,a4,0x2
	add	a4,a4,s1
	# lw	a2,0(a4)		# C[size*i + j]
	add	a5,a5,a3
	slli	a5,a5,0x2
	add	a5,a5,s3
	# lw	a1,0(a5)		# A[size*i + k]
	mul	a5,a3,s0
	add	a5,a5,a0
	slli	a5,a5,0x2
	add	a5,a5,s2
	# lw	a5,0(a5)		# B[size*k + j]
	mul	a5,a1,a5
	add	a5,a5,a2
	#sw	a5,0(a4)
	addi	a3,a3,1			# k++
.Lmul_k_cond:
	bltu	a3,s0,.Lmul_k_body
	addi	a7,a7,1			# j++
.Lmul_j_cond:
	mv	a0,a7
	bgeu	a7,s0,.Lmul_j_done
	li	a3,0			# k = 0
	j	.Lmul_k_cond
.Lmul_j_done:
	addi	t1,t1,1			# i++
.Lmul_i_cond:
	mv	a6,t1
	bgeu	t1,s0,.Lmul_done
	li	a7,0			# j = 0
	j	.Lmul_j_cond

.Lmul_done:
	li	a1,0
	li	a0,5
	call	set_gpio		# set_gpio(EXPE_PIN, 0)
	li	a0,300
	call	vTaskDelay		# vTaskDelay(pdMS_TO_TICKS(3000))

	# ---- Checksum ----
	sw	zero,12(sp)		# checksum = 0
	li	a1,0			# i = 0
	j	.Lsum_i_cond
.Lsum_j_body:
	mul	a5,a2,s0
	add	a5,a5,a4
	slli	a5,a5,0x2
	add	a5,a5,s1
	lw	a5,0(a5)		# C[size*i + j]
	lw	a3,12(sp)
	add	a5,a5,a3
	slli	a5,a5,0x1		# % (1<<31)
	srli	a5,a5,0x1
	sw	a5,12(sp)
	addi	a4,a4,1			# j++
.Lsum_j_cond:
	bltu	a4,s0,.Lsum_j_body
	addi	a1,a1,1			# i++
.Lsum_i_cond:
	mv	a2,a1
	bgeu	a1,s0,.Lsum_done
	li	a4,0			# j = 0
	j	.Lsum_j_cond

.Lsum_done:
	mv	a0,s3
	call	free			# free(A)
	mv	a0,s2
	call	free			# free(B)
	mv	a0,s1
	call	free			# free(C)
	lw	a0,12(sp)		# return checksum
	lw	ra,60(sp)
	lw	s0,56(sp)
	lw	s1,52(sp)
	lw	s2,48(sp)
	lw	s3,44(sp)
	lw	s4,40(sp)
	lw	s5,36(sp)
	lw	s6,32(sp)
	lw	s7,28(sp)
	addi	sp,sp,64
	ret
.size benchmark_mat_mul_asm_nomem, .-benchmark_mat_mul_asm_nomem
