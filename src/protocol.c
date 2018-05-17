
#include "protocol.h"

#include <avr/io.h>
#include <util/delay.h>

#include "config.h"
#include "main.h"
#include "write.h"

static inline void serial_bit_train()
{	
	// make the rx-pin an output
	RX_DDR |= (1 << RX_BIT);
	
	// send 20 bits
	for(register unsigned char i = 0; i < 20; i++) {
		RX_PORT |= (1 << RX_BIT);
		_delay_us(SERIAL_BIT_TIME);
		
		RX_PORT &= ~(1 << RX_BIT);
		_delay_us(SERIAL_BIT_TIME);
	}
}

inline void serial_init()
{
	serial_bit_train();

	// set tx-pin as output and set it high
	TX_DDR |= (1 << TX_BIT);
	TX_PORT |= (1 << TX_BIT);

	// set RX-pin as input and enable pull-up
	RX_DDR &= ~(1 << RX_BIT);
	RX_PORT |= (1 << RX_BIT);
}

inline void serial_send_1()
{
	TX_PORT |= (1 << TX_BIT);
	_delay_us(SERIAL_BIT_TIME);
}

inline void serial_send_0()
{
	TX_PORT &= ~(1 << TX_BIT);
	_delay_us(SERIAL_BIT_TIME);
}

void serial_send(char ch)
{
	serial_send_0(); // start-bit

	for(register char i = 0; i < 8; i++) {
		if(ch & 1)
			serial_send_1();
		else
			serial_send_0();
		ch >>= 1;
	}

	serial_send_1(); // stop-bit
}

char recv_char;

void serial_wait_with_timeout()
{
	volatile register int i = 0;
	while((++i) != 0) {
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		if(!(RX_PIN & (1 << RX_BIT)))
			return;		
	}
	// a timeout happened...
	//leave_bootloader();
	asm volatile ("rjmp leave_bootloader");
}

void serial_recv()
{
	serial_wait_with_timeout();

	INDICATOR_LED_ON();

	// wait until half of the first bit has been transmitted
	_delay_us(SERIAL_BIT_TIME + SERIAL_BIT_TIME / 2);

	for(register char i = 0; i < 8; i++) {
		recv_char >>= 1;
		if(RX_PIN & (1 << RX_BIT))
			recv_char |= 0x80;
		_delay_us(SERIAL_BIT_TIME);
	}

	INDICATOR_LED_OFF();
}

char block_number = 0;
char block[130];

void loader()
{
	serial_recv();
	serial_send(recv_char);

	if(recv_char != '{')
		return;

	serial_send(0x15);

	while(1) {
		serial_recv();

		if(recv_char == 0x01) {
			recv_block();		
		} else if(recv_char == 0x04) {
			// if the last possible block was not written, write it
			// to have a working rjmp to the loaded program-code
			if(block_number <= LAST_NONBOOT_BLOCK) {
				block_number = LAST_NONBOOT_BLOCK;
				write_block();
			}
			serial_send(0x06);

			return;
		}
	}

}

void recv_block()
{
	char checksum = 1;
	for(register char i = 0; i < 130; i++) {
		serial_recv();
		checksum += recv_char;
		block[i] = recv_char;
	}
	serial_recv();

	if(checksum == recv_char && // checksum corrent
		block[0] == block_number + 1 && // correct block-number
		block_number <= LAST_NONBOOT_BLOCK) // not overwriting bootloader
	{
		write_block();

		serial_send(0x06);
		block_number++;
	} else {
		serial_send(0x15);
	}

}
