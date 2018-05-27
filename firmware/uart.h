/* Junior Design Sp2018 Final Project
 * Robot Firmware - UART Functions
 * Nick Ames 2018
 */

/* Setup UART at BAUD 8N1. */
void init_uart(void);

/* Send a character out of the UART. */
void uart_putchar(char c);

/* Get a character from the uart. */
char uart_getchar(void);

/* Send a PROGMEM string literal from the uart.
 * Usage:
 *   log("string to send");
 */
#define log(str) log_helper(PSTR(str))
void log_helper(const char *str);
