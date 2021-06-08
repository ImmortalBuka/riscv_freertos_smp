#ifndef _PRINT_H_
#define _PRINT_H_
//
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
//
extern void uart_print_sized(void* uart, const char* string, uint8_t size);
void uart_print_string(void* uart, const char* string);
uint8_t uint8_to_string(uint8_t data, char* string);
void uint8_to_hex_string(uint8_t data, char* string);
void uint16_to_string(uint16_t data, char* string);
void uint16_to_hex_string(uint16_t data, char* string);
uint8_t uint32_to_string(uint32_t data, char* string_ptr);
void uint32_to_hex_string(uint32_t data, char* string);
void print_hex_uint8(void* uart, uint8_t data);
uint8_t char_to_hex_dig(uint8_t char_in, uint8_t* byte_out);
uint8_t char_to_dec_dig(uint8_t char_in, uint8_t* byte_out);
void uint64_to_hex_string(uint64_t data, char* string);
void print_straight_address(void* uart, uint8_t* addr);
void print_reverse_address(void* uart, uint8_t* addr);
uint8_t float32_to_string(float data, char* string, uint8_t after_dot);
uint8_t string_len(const char* data);
bool string_compare(const char* first, const char* second, uint8_t len);
//
#ifdef __cplusplus
}
#endif
#endif//_PRINT_H_
/*
text colors:

\e[39m Default foreground color
\e[30m Black
\e[31m Red
\e[32m Green
\e[33m Yellow
\e[34m Blue
\e[35m Magenta
\e[36m Cyan
\e[37m Light gray
\e[90m Dark gray
\e[91m Light red
\e[92m Light green
\e[93m Light yellow
\e[94m Light blue
\e[95m Light magenta
\e[96m Light cyan
\e[97m White

background colors:

\e[49m Default background color
\e[40m Black
\e[41m Red
\e[42m Green
\e[43m Yellow
\e[44m Blue
\e[45m Magenta
\e[46m Cyan
\e[47m Light gray
\e[100m Dark gray
\e[101m Light red
\e[102m Light green
\e[103m Light yellow
\e[104m Light blue
\e[105m Light magenta
\e[106m Light cyan
\e[107m White
*/
