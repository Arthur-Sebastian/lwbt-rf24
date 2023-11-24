; LWBT-RF24
; data utility functions
; (C) Artur Sebastian Miller 2023

	.section .text

	.global btle_crc
	.global btle_whiten

; Bluetooth Core 4.2, Vol 6, Part B, 3.1.1
; ----
; prototype:
; uint32_t btle_crc(uint8_t *, uint8_t);
; ----
; r24:r25 - data buffer pointer
; r22     - buffer length (bytes)
btle_crc:
	; move to Z register
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
	; pointer access, post increment
	ld		r18, Z+
	ldi		r19, 0x01
btle_crc_l2:
	; shift right, extract carry
	lsr		r22
	ror		r23
	ror		r24
	rol		r20
	; xor with carry
	eor		r20, r18
	lsr		r20
	brcc		btle_crc_next
	eor		r22, r26
	eor		r23, r27
btle_crc_next:
	lsr		r18
	lsl		r19
	brne		btle_crc_l2
	dec		r25
	brne		btle_crc_l1
	; adjust byte order
	mov		r25, r22
	mov		r22, r24
	mov		r24, r25
	ret

; Bluetooth Core 4.2, Vol 6, Part B, 3.2
; ----
; prototype:
; void btle_whiten(uint8_t *, uint8_t, uint8_t);
; ----
; r24:r25 - data buffer pointer
; r22     - buffer length (bytes)
; r20     - channel index
btle_whiten:
	; move to Z register
	movw		r30, r24
	; whitening polynomial
	ldi		r21, 0x88
	; 8 bit LFSR
	ori		r20, 0x40
btle_whiten_l1:
	eor		r18, r18
	ldi		r19, 0x01
btle_whiten_l2:
	sbrs		r20, 0
	rjmp		btle_whiten_next
	eor		r20, r21
	or		r18, r19
btle_whiten_next:
	; shift right, extract carry
	lsr		r20
	lsl		r19
	brne		btle_whiten_l2
	ld		r24, Z
	eor		r24, r18
	st		Z+, r24
	dec		r22
	brne		btle_whiten_l1
	ret
