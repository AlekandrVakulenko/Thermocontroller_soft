//----------------------------------------
.global DelayAsmX5
DelayAsmX5:
	subi r24, 4
	sbci r25, 0
	delayx5loop:
	subi r24, 1
	sbci r25, 0
	nop
	brcc delayx5loop
	nop
	nop
	nop
	nop
ret
//----------------------------------------




