	.data			// pc = 0
array:	.word	3		// pc = 0
	.word	123
	.word	4346
array2:	.word	0x11111111	// pc = 12
	.text			
main:				// pc = 16
	addiu	$2, $0, 1024	// pc = 16
	addu	$3, $2, $2	// pc = 20
	or	$4, $3, $2	// pc = 24
	sll	$6, $5, 16	// pc = 28
	addiu	$7, $6, 9999	// pc = 32
	subu	$8, $7, $2	// pc = 36
	nor	$9, $4, $3	// pc = 40
	ori	$10, $2, 255	// pc = 44
	srl	$11, $6, 5	// pc = 48
	la	$4, array2	// pc = 52
	and	$13, $11, $5	// pc = 56
	andi	$14, $4, 100	// pc = 60
	lui	$17, 100	// pc = 64
	addiu	$2, $0, 0xa	// pc = 68
