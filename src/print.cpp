#include "print.hpp"
//
void uart_print_string(void* uart, uint8_t* string)
{
	uint8_t i = string_len(string);
	uart_print_sized(uart, string, i);
}
uint8_t uint8_to_string(uint8_t data, uint8_t* string)
{
	uint8_t return_value = 0;
	uint8_t temp_buf[3];
	uint8_t index = 0;
	temp_buf[0] = data % 10;
	temp_buf[1] = ((data/10)%10);
	temp_buf[2] = (data/100);
	for(uint8_t j=0; j<3; j++)
	{
		if(temp_buf[j] > 0) index = j;
	}
	return_value = index+1;
	for(int8_t j=index; j>(-1); j--)
	{
		*string = temp_buf[j] + 0x30;
		string++;
	}
	*string = 0;
	return return_value;
}
void uint8_to_hex_string(uint8_t data, uint8_t* string)
{
    uint8_t temp;
	*(string++) = 0x30;
	*(string++) = 0x78;
	for(uint8_t i=2; i!=0; i--)
	{
		temp = (data >> (4*(i-1)))&0xf;
		if(temp < 10) *(string++) = temp + 0x30;
		else *(string++) = temp + 0x57;
	}
	*string = 0;
}
void uint16_to_string(uint16_t data, uint8_t* string)
{
	uint8_t temp_buf[5];
	uint8_t index = 0;
	temp_buf[0] = data % 10;
	for(int8_t j=1; j<5; j++)
	{
		data /= 10;
		temp_buf[j] = data % 10;
	}
	for(int8_t j=0; j<5; j++)
	{
		if(temp_buf[j] > 0) index = j;
	}
	//*size = index+1;
	for(int8_t j=index; j>(-1); j--)
	{
		*string = temp_buf[j] + 0x30;
		string++;
	}
	*string = 0;
}
void uint16_to_hex_string(uint16_t data, uint8_t* string)
{
	uint8_t temp;
	*(string++) = 0x30;
	*(string++) = 0x78;
	for(uint8_t i=4;i>0;i--)
	{
		temp = (data >> (4*(i-1)))&0xf;
		if(temp < 10) *(string++) = temp + 0x30;
		else *(string++) = temp + 0x57;
	}
	*string = 0;
}
uint8_t uint32_to_string(uint32_t data, uint8_t* string_ptr)
{
	uint8_t return_value = 0;
	uint8_t temp_buf[10];
	uint8_t index = 0;
	temp_buf[0] = data % 10;
	for(int8_t j=1; j<10; j++)
	{
		data /= 10;
		temp_buf[j] = data % 10;
	}
	for(int8_t j=0; j<10; j++)
	{
		if(temp_buf[j] > 0) index = j;
	}
	return_value = index+1;
	for(int8_t j=index; j>(-1); j--)
	{
		*string_ptr = temp_buf[j] + 0x30;
		string_ptr++;
	}
	*string_ptr = 0;
	return return_value;
}
void uint32_to_hex_string(uint32_t data, uint8_t* string)
{
	uint8_t temp;
	*(string++) = 0x30;
	*(string++) = 0x78;
	for(uint8_t i=8;i>0;i--)
	{
		temp = (data >> (4*(i-1)))&0xf;
		if(temp < 10) *(string++) = temp + 0x30;
		else *(string++) = temp + 0x57;
	}
	*string = 0;
}
uint8_t char_to_hex_dig(uint8_t char_in, uint8_t* byte_out)
{
	if(char_in > 0x29)
	{
		if(char_in < 0x3a)
		{
			*byte_out = char_in - 0x30;
			return 0;
		}
		else
		{
			switch(char_in)
			{
				case 'a':
				case 'A':
					*byte_out = 0xa;
					return 0;
				case 'b':
				case 'B':
					*byte_out = 0xb;
					return 0;
				case 'c':
				case 'C':
					*byte_out = 0xc;
					return 0;
				case 'd':
				case 'D':
					*byte_out = 0xd;
					return 0;
				case 'e':
				case 'E':
					*byte_out = 0xe;
					return 0;
				case 'f':
				case 'F':
					*byte_out = 0xf;
					return 0;
				default:
					return 2;
			}
		}
	}
	else return 1;
}
uint8_t char_to_dec_dig(uint8_t char_in, uint8_t* byte_out)
{
	if((char_in > 0x29)&&(char_in < 0x3a))
	{
		*byte_out = char_in - 0x30;
		return 0;
	}
	else return 1;
}
void uint64_to_hex_string(uint64_t data, uint8_t* string)
{
	uint8_t temp;
	*(string++) = 0x30;
	*(string++) = 0x78;
	for(uint8_t i=16;i>0;i--)
	{
		temp = (data >> (4*(i-1)))&0xf;
		if(temp < 10) *(string++) = temp + 0x30;
		else *(string++) = temp + 0x57;
	}
	*string = 0;
}
/*
void print_hex_uint8(void* uart, uint8_t data)
{
	uint8_t temp[2];
	temp[0] = (data&0xf0)>>4;
	temp[1] = data&0x0f;
	for(uint8_t i=0; i<2; i++)
	{
		if(temp[i] < 10) uart_print_char(uart, temp[i] + 0x30);
		else uart_print_char(uart, temp[i] + 0x57);
	}
}
void print_straight_address(void* uart, uint8_t* addr)
{
	print_hex_uint8(uart, addr[5]);
	uart_print_char(uart, 0x3a);
	print_hex_uint8(uart, addr[4]);
	uart_print_char(uart, 0x3a);
	print_hex_uint8(uart, addr[3]);
	uart_print_char(uart, 0x3a);
	print_hex_uint8(uart, addr[2]);
	uart_print_char(uart, 0x3a);
	print_hex_uint8(uart, addr[1]);
	uart_print_char(uart, 0x3a);
	print_hex_uint8(uart, addr[0]);
}
void print_reverse_address(void* uart, uint8_t* addr)
{
	print_hex_uint8(uart, addr[0]);
	uart_print_char(uart, 0x3a);
	print_hex_uint8(uart, addr[1]);
	uart_print_char(uart, 0x3a);
	print_hex_uint8(uart, addr[2]);
	uart_print_char(uart, 0x3a);
	print_hex_uint8(uart, addr[3]);
	uart_print_char(uart, 0x3a);
	print_hex_uint8(uart, addr[4]);
	uart_print_char(uart, 0x3a);
	print_hex_uint8(uart, addr[5]);
}*/
uint8_t float32_to_string(float data, uint8_t* string, uint8_t after_dot)
{
	uint32_t temp_loc_1;
	uint8_t counter = 0, delta;
	if(data < 0)
	{
		string[counter++] = '-';
		data *= -1.0;
	}
	temp_loc_1 = (uint32_t)data;
	delta = uint32_to_string(temp_loc_1, &string[counter]);
	counter += delta;
	string[counter++] = '.';
	data = data - (float)temp_loc_1;
	for(uint8_t i=0; i<after_dot; i++)
	{
		data = data * 10.0;
		temp_loc_1 = (uint32_t)data;
		string[counter++] = temp_loc_1 + 0x30;
		data = data - (float)temp_loc_1;
	}
	string[counter] = 0;
	return counter;
}
bool string_compare(uint8_t* first, uint8_t* second, uint8_t len)
{
	for(uint8_t i=0; i<len; i++)
	{
		if(first[i] != second[i]) return false;
	}
	return true;
}
uint8_t string_len(uint8_t* data)
{
	uint8_t i=0;
	while(data[i++]){};
	return i-1;
}

