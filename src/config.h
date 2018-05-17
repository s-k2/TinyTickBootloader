#ifndef CONFIG_H
#define CONFIG_H

// serial-config
#define SERIAL_BIT_TIME 416 // in microseconds

// tiny-config
#define TX_DDR DDRB
#define TX_PORT PORTB
#define TX_BIT PB4

#define RX_DDR DDRB
#define RX_PIN PINB
#define RX_PORT PORTB
#define RX_BIT PB6

#define PAGE_COUNT 64 // number of pages
#define PAGE_SIZE_BYTES 64 // tiny 44 has a page-size of 32 words

#define BLOCK_SIZE 128 // bytes one XMODEM-block contains
#define BLOCK_COUNT (PAGE_COUNT * PAGE_SIZE_BYTES / BLOCK_SIZE) // how many 128-byte blocks are there?

// the last block that is not occupied by the bootloader
// BUT: its last word is reserved for use by the loader as
//      it contains the address of user-code's reset vector
#define BOOTLOADER_PAGES 6 // number of pages the bootloader needs
#define LAST_NONBOOT_BLOCK (BLOCK_COUNT - BOOTLOADER_PAGES - 1)
#define FIRST_BOOTLOADER_BLOCK (BLOCK_COUNT - BOOTLOADER_PAGES)

#define BOOTLOADER_ADDRESS (FIRST_BOOTLOADER_BLOCK * BLOCK_SIZE) // as byte address (not words!)

// jump from 0x0000 to first bootloader instruction
#define RJUMP_BOOTLOADER_OPCODE_LOW ((BOOTLOADER_ADDRESS / 2 - 1) & 0xff)
#define RJUMP_BOOTLOADER_OPCODE_HIGH (0xc0 | (((BOOTLOADER_ADDRESS / 2 - 1) >> 8) & 0x0f))

#define INDICATOR_LED_INIT() (DDRA |= (1 << 3))
#define INDICATOR_LED_ON() (PORTA |= (1 << 3))
#define INDICATOR_LED_OFF() (PORTA &= ~(1 << 3))

#endif /* CONFIG_H */
