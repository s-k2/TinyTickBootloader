#include <avr/io.h>


.extern z_low
.extern z_high
.extern current_word_low
.extern current_word_high
.extern spmcsr_value

.global erase_page
erase_page:
	ldi r18, 3			; SPMCSR = erase page
	rcall do_spm
	ldi r18, 17			; SPMCSR = re-enable page
	rjmp do_spm

; void write_word(char low, char high) -> low: r24, high: r22
.global write_word
write_word:
	ldi r18, 1			; SPMCSR = write word
	rjmp do_spm			; return in do_spm

.global write_page
write_page:
	ldi r18, 5			; SPMCSR = write page
	rcall do_spm
	ldi r18, 17			; SPMCSR = re-enable page
	rjmp do_spm


; expects data-words to be in r24 (low) and r22 (high)
; and spcsr-value to be in r18
.global do_spm
do_spm:
	push r1

	lds r30, z_low	; set address where we want to do spm with
	lds r31, z_high

check_spmcsr:		; wait until last flash-action finished
	in r0, 0x37; _SFR_IO_ADDR(SPMCR)
	sbrc r0, 0
	rjmp check_spmcsr

check_eeprom:		; wait until eprom-actoin finished
	sbic	_SFR_IO_ADDR(EECR), EEPE
	rjmp check_eeprom

	mov r0, r24
	mov r1, r22			; load data

	out 0x37, r18		; load argument to SPMCR

	spm		; do the spm
	nop
	nop

	pop r1

	ret
