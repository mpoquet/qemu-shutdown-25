#include "utils.h"
#include "peripherals/mini_uart.h"
#include "peripherals/gpio.h"
#include <stdint.h>

void uart_send ( char c )
{
	while(1) {
		if(get32(AUX_MU_LSR_REG)&0x20)
			break;
	}
	put32(AUX_MU_IO_REG,c);
}

char uart_recv ( void )
{
	while(1) {
		if(get32(AUX_MU_LSR_REG)&0x01)
			break;
	}
	return(get32(AUX_MU_IO_REG)&0xFF);
}

void uart_send_string(char* str)
{
	for (int i = 0; str[i] != '\0'; i ++) {
		uart_send((char)str[i]);
	}
}

void uart_init ( void )
{
	unsigned int selector;

	selector = get32(GPFSEL1);
	selector &= ~(7<<12);                   // clean gpio14
	selector |= 2<<12;                      // set alt5 for gpio14
	selector &= ~(7<<15);                   // clean gpio15
	selector |= 2<<15;                      // set alt5 for gpio15
	put32(GPFSEL1,selector);

	put32(GPPUD,0);
	delay(150);
	put32(GPPUDCLK0,(1<<14)|(1<<15));
	delay(150);
	put32(GPPUDCLK0,0);

	put32(AUX_ENABLES,1);                   //Enable mini uart (this also enables access to it registers)
	put32(AUX_MU_CNTL_REG,0);               //Disable auto flow control and disable receiver and transmitter (for now)
	put32(AUX_MU_IER_REG,0);                //Disable receive and transmit interrupts
	put32(AUX_MU_LCR_REG,3);                //Enable 8 bit mode
	put32(AUX_MU_MCR_REG,0);                //Set RTS line to be always high
	put32(AUX_MU_BAUD_REG,270);             //Set baud rate to 115200

	put32(AUX_MU_CNTL_REG,3);               //Finally, enable transmitter and receiver
}
/**
 * Prints a 64-bit unsigned integer in hexadecimal format over UART.
 *
 * This function sends the prefix "0x" followed by 16 hexadecimal digits
 * representing the value of `num`. It processes the number from the
 * most significant nibble (4 bits) to the least significant one.
 *
 * Example: uart_hex_puts(0x1234ABCD); outputs "0x000000001234ABCD"
 *
 * @param num The 64-bit unsigned integer to print.
 */
void uart_hex_puts(uint64_t num) {
    char hex_chars[] = "0123456789ABCDEF";
    uart_send_string("0x");
    for (int i = 60; i >= 0; i -= 4) {
        uart_send(hex_chars[(num >> i) & 0xF]);
    }
    uart_send_string("\r\n");
}

