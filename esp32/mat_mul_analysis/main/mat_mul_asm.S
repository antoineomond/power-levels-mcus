.text                              # Code segment
.global benchmark_mat_mul_asm                 # Make this label visible to the linker
.type benchmark_mat_mul_asm, @function        # Declare label type: function

benchmark_mat_mul_asm:
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
	sw	zero,12(sp)
	li	a0,42
	call srand
	# mul	s1,s0,s0
	# slli	s1,s1,0x2
	# mv	a0,s1
	# auipc	ra,0xfe7f9
	# jalr	1880(ra) # 4080568c <malloc>
	# mv	s3,a0
	# mv	a0,s1
	# auipc	ra,0xfe7f9
	# jalr	1868(ra) # 4080568c <malloc>
	# mv	s2,a0
	# mv	a0,s1
	# auipc	ra,0xfe7f9
	# jalr	1856(ra) # 4080568c <malloc>
	# mv	s1,a0
	# li	s7,0
	# j	4200bfa0 <benchmark_mat_mul+0x94>
	# jal	42011244 <rand>
	# mul	s4,s6,s0
	# add	s4,s4,s5
	# slli	s4,s4,0x2
	# add	a4,s3,s4
	# srai	a5,a0,0x1f
	# srli	a5,a5,0x18
	# add	a0,a0,a5
	# zext.b	a0,a0
	# sub	a0,a0,a5
	# sw	a0,0(a4)
	# jal	42011244 <rand>
	# add	a4,s2,s4
	# srai	a5,a0,0x1f
	# srli	a5,a5,0x18
	# add	a0,a0,a5
	# zext.b	a0,a0
	# sub	a0,a0,a5
	# sw	a0,0(a4)
	# add	s4,s4,s1
	# sw	zero,0(s4)
	# addi	s5,s5,1
	# bltu	s5,s0,4200bf5a <benchmark_mat_mul+0x4e>
	# addi	s7,s7,1
	# mv	s6,s7
	# bgeu	s7,s0,4200bfaa <benchmark_mat_mul+0x9e>
	# li	s5,0
	# j	4200bf9a <benchmark_mat_mul+0x8e>
	# li	a1,1
	# li	a0,5
	# jal	4200be82 <set_gpio>
	# li	t1,0
	# j	4200bff2 <benchmark_mat_mul+0xe6>
	# mul	a5,a6,s0
	# add	a4,a5,a0
	# slli	a4,a4,0x2
	# add	a4,a4,s1
	# lw	a2,0(a4)
	# add	a5,a5,a3
	# slli	a5,a5,0x2
	# add	a5,a5,s3
	# lw	a1,0(a5)
	# mul	a5,a3,s0
	# add	a5,a5,a0
	# slli	a5,a5,0x2
	# add	a5,a5,s2
	# lw	a5,0(a5)
	# mul	a5,a1,a5
	# add	a5,a5,a2
	# sw	a5,0(a4)
	# addi	a3,a3,1
	# bltu	a3,s0,4200bfb4 <benchmark_mat_mul+0xa8>
	# addi	a7,a7,1
	# mv	a0,a7
	# bgeu	a7,s0,4200bff0 <benchmark_mat_mul+0xe4>
	# li	a3,0
	# j	4200bfe0 <benchmark_mat_mul+0xd4>
	# addi	t1,t1,1
	# mv	a6,t1
	# bgeu	t1,s0,4200bffc <benchmark_mat_mul+0xf0>
	# li	a7,0
	# j	4200bfe6 <benchmark_mat_mul+0xda>
	# li	a1,0
	# li	a0,5
	# jal	4200be82 <set_gpio>
	# sw	zero,12(sp)
	# li	a1,0
	# j	4200c026 <benchmark_mat_mul+0x11a>
	# mul	a5,a2,s0
	# add	a5,a5,a4
	# slli	a5,a5,0x2
	# add	a5,a5,s1
	# lw	a5,0(a5)
	# lw	a3,12(sp)
	# add	a5,a5,a3
	# slli	a5,a5,0x1
	# srli	a5,a5,0x1
	# sw	a5,12(sp)
	# addi	a4,a4,1
	# bltu	a4,s0,4200c008 <benchmark_mat_mul+0xfc>
	# addi	a1,a1,1
	# mv	a2,a1
	# bgeu	a1,s0,4200c030 <benchmark_mat_mul+0x124>
	# li	a4,0
	# j	4200c020 <benchmark_mat_mul+0x114>
	# mv	a0,s3
	# auipc	ra,0xfe7f9
	# jalr	1640(ra) # 4080569a <cfree>
	# mv	a0,s2
	# auipc	ra,0xfe7f9
	# jalr	1630(ra) # 4080569a <cfree>
	# mv	a0,s1
	# auipc	ra,0xfe7f9
	# jalr	1620(ra) # 4080569a <cfree>
	# lw	a0,12(sp)
	# lw	ra,60(sp)
	# lw	s0,56(sp)
	# lw	s1,52(sp)
	# lw	s2,48(sp)
	# lw	s3,44(sp)
	# lw	s4,40(sp)
	# lw	s5,36(sp)
	# lw	s6,32(sp)
	# lw	s7,28(sp)
	# addi	sp,sp,64
	# ret
	
