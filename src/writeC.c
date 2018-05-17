#include "write.h"

#include <avr/boot.h>

#include "config.h"
#include "protocol.h"

char real_reset_dest[2];

void write_flash();

void write_block()
{
	// patch reset-vector destination to our bootloader
	if(block_number == 0) {
		real_reset_dest[0] = block[2];
		block[2] = RJUMP_BOOTLOADER_OPCODE_LOW;
		real_reset_dest[1] = block[3];
		block[3] = RJUMP_BOOTLOADER_OPCODE_HIGH;
	}

	if(block_number == LAST_NONBOOT_BLOCK) {
		unsigned short reset_address  =
			-((BOOTLOADER_ADDRESS - 2) / 2) +  // rjump from the word one before the bootloader-code
			((real_reset_dest[1] & 0x0f) << 8 | real_reset_dest[0]); // to 0x0000 + the offset the code wants to rjmp to
		block[128] = reset_address & 0xff;
		block[129] = (((reset_address >> 8) & 0x0f) | 0xc0);
	}

	// now the blocks are prepared and can be written
	write_flash();
}


extern void erase_page();
extern void write_word(char low, char high);
extern void write_page();

char z_low, z_high; // address of the flash-location, in bytes

void write_flash()
{
	z_low = block_number << 7;
	z_high = block_number >> 1;

	for(char i = 0; i < 128; i += 2) {
		if((i & (PAGE_SIZE_BYTES - 1)) == 0) {
			erase_page();
		}

		write_word(block[i + 2], block[i + 3]);


		// due to word-wise writing we have written 
		// the whole page if i has a value of PAGE_SIZE - 1
		if((i & (PAGE_SIZE_BYTES - 1)) == PAGE_SIZE_BYTES - 2) {
			register char tmp_z_low = z_low;

			z_low &= ~(PAGE_SIZE_BYTES - 1);
			write_page();

			z_low = tmp_z_low;
		}

		z_low += 2;
	}
}
