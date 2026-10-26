.section .iram1.benchmark_mat_mul_float_asm, "ax"                              # Code segment
.global benchmark_mat_mul_float_asm                 # Make this label visible to the linker
.type benchmark_mat_mul_float_asm, @function        # Declare label type: function

# uint32_t benchmark_mat_mul_float_asm(uint32_t size)
#   s0 = size, s1 = C, s2 = B, s3 = A, 12(sp) = volatile checksum
.balign 4
benchmark_mat_mul_float_asm:
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
	auipc	ra,0x1810
	jalr	-1414(ra) # 42011b1c <srand>
	mul	s0,s2,s2
	slli	s0,s0,0x2
	mv	a0,s0
	jal	40805cde <malloc>
	mv	s7,a0
	mv	a0,s0
	jal	40805cde <malloc>
	mv	s6,a0
	mv	a0,s0
	jal	40805cde <malloc>
	mv	s5,a0
	li	s8,0
	j	40802130 <benchmark_mat_mul_float+0xb4>
	auipc	ra,0x1810
	jalr	-1446(ra) # 42011b26 <rand>
	auipc	ra,0xff7ff
	jalr	-2024(ra) # 400008ec <__floatsisf>
	mul	s0,s4,s2
	add	s0,s0,s1
	slli	s0,s0,0x2
	add	s9,s7,s0
	lui	s3,0x42029
	lw	a1,1228(s3) # 420294cc <esp_err_msg_esp_https_ota_include+0x4>
	auipc	ra,0xff7ff
	jalr	-1968(ra) # 40000940 <__mulsf3>
	sw	a0,0(s9)
	auipc	ra,0x1810
	jalr	-1494(ra) # 42011b26 <rand>
	auipc	ra,0xff7fe
	jalr	2024(ra) # 400008ec <__floatsisf>
	add	s9,s6,s0
	lw	a1,1228(s3)
	auipc	ra,0xff7ff
	jalr	-2004(ra) # 40000940 <__mulsf3>
	sw	a0,0(s9)
	add	s0,s0,s5
	li	a5,0
	sw	a5,0(s0)
	addi	s1,s1,1
	bltu	s1,s2,408020cc <benchmark_mat_mul_float+0x50>
	addi	s8,s8,1
	mv	s4,s8
	bgeu	s8,s2,4080213a <benchmark_mat_mul_float+0xbe>
	li	s1,0
	j	4080212a <benchmark_mat_mul_float+0xae>
	li	a0,300
	auipc	ra,0x1818
	jalr	1260(ra) # 4201a62a <vTaskDelay>
	li	a1,1
	li	a0,5
	auipc	ra,0x180b
	jalr	-1114(ra) # 4200ccf0 <set_gpio>
	li	s10,0
	j	408021a4 <benchmark_mat_mul_float+0x128>
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
	auipc	ra,0xff7fe
	jalr	1990(ra) # 40000940 <__mulsf3>
	mv	a1,a0
	mv	a0,s4
	auipc	ra,0xff7fe
	jalr	1750(ra) # 4000085c <__addsf3>
	sw	a0,0(s0)
	addi	s1,s1,1
	bltu	s1,s2,40802156 <benchmark_mat_mul_float+0xda>
	addi	s9,s9,1
	mv	s3,s9
	bgeu	s9,s2,408021a2 <benchmark_mat_mul_float+0x126>
	li	s1,0
	j	40802192 <benchmark_mat_mul_float+0x116>
	addi	s10,s10,1
	mv	s8,s10
	bgeu	s10,s2,408021ae <benchmark_mat_mul_float+0x132>
	li	s9,0
	j	40802198 <benchmark_mat_mul_float+0x11c>
	li	a1,0
	li	a0,5
	auipc	ra,0x180b
	jalr	-1218(ra) # 4200ccf0 <set_gpio>
	li	a5,0
	sw	a5,12(sp)
	li	s3,0
	j	408021e4 <benchmark_mat_mul_float+0x168>
	mul	a5,s1,s2
	add	a5,a5,s0
	slli	a5,a5,0x2
	add	a5,a5,s5
	lw	a1,12(sp)
	lw	a0,0(a5)
	auipc	ra,0xff7fe
	jalr	1674(ra) # 4000085c <__addsf3>
	sw	a0,12(sp)
	addi	s0,s0,1
	bltu	s0,s2,408021c4 <benchmark_mat_mul_float+0x148>
	addi	s3,s3,1
	mv	s1,s3
	bgeu	s3,s2,408021ee <benchmark_mat_mul_float+0x172>
	li	s0,0
	j	408021de <benchmark_mat_mul_float+0x162>
	mv	a0,s7
	jal	40805cec <cfree>
	mv	a0,s6
	jal	40805cec <cfree>
	mv	a0,s5
	jal	40805cec <cfree>
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
.size benchmark_mat_mul_float_asm, .-benchmark_mat_mul_float_asm
