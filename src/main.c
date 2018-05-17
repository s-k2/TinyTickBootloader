#include <avr/io.h>

#include "config.h"
#include "protocol.h"
#include "write.h"

// jump to bootloader's reset vector
const char jump_to_boot_main[2] 
	__attribute__ ((section(".rjmp_loader_seg"))) = 
	{ RJUMP_BOOTLOADER_OPCODE_LOW, RJUMP_BOOTLOADER_OPCODE_HIGH };

// noop and start with bootloader again 
// (will overwritten by a rjmp if a progrmm has been loaded)
const char leave_bootloader_rjmp[2] 
	__attribute__ ((section(".leave_bootloader_seg"))) = { 0, 0 };

void leave_bootloader()
{
	DDRA = 0;

	// This is a bad trick... We know that our vectors-table is the first
	// code of the bootloader (and it gets moved to BOOTLOADER_ADDRESS != 0
	// by the linker). Two bytes before that address the rjmp to the 
	// program to load is located. So we just rjmp there and from 
	// BOOTLOADER_ADDRESS - 2 we jump to the real program
	asm volatile ("rjmp __vectors - 2");
}

void main()
{
	INDICATOR_LED_INIT();
	
	serial_init();

	serial_recv();
	serial_send(recv_char);

	if(recv_char == '{') {
		loader();
	}

	leave_bootloader();
}
