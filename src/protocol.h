#ifndef COMMUNICATION_H
#define COMMUNICATION_H

extern char recv_char;

extern char block_number;
extern char block[130];

extern void serial_init();
extern void serial_recv();
extern void serial_send(char ch);

extern void loader();
extern void recv_block();

#endif /* COMMUNICATION_H */
