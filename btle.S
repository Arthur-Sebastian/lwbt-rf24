#include <avr/io.h>

	.section .text
	.global btle_crc
btle_crc:
	movw		r30, r24
	mov		r25, r22
	; CRC polynomial
	ldi		r26, 0xDA
	ldi		r27, 0x60
	; 24 bit LFSR
	ldi		r22, 0xAA
	ldi		r23, 0xAA
	ldi		r24, 0xAA
btle_crc_l1:
	ld		r18, Z+
	ldi		r19, 0x01
btle_crc_l2:
	; perform multi-byte shift
	lsr		r22
	ror		r23
	ror		r24
	; extract carry
	rol		r20
	eor		r20, r18
	; check lowest bit
	lsr		r20
	brcc		btle_crc_next
	; toggle polynomial bits
	eor		r22, r26
	eor		r23, r27
btle_crc_next:
	lsr		r18
	lsl		r19
	brne		btle_crc_l2
	dec		r25
	breq		btle_crc_ret
	rjmp		btle_crc_l1
btle_crc_ret:
	ret
