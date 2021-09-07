#include <stdint.h>
#include "print.hpp"
//
template <typename T> class fifo
{
	private:
		uint32_t size = 0;
		uint32_t write_index = 0;
		uint32_t read_index = 0;
		//uint32_t user_index = 0;
		T* buffer = 0;
		uint32_t count_read(void)
		{
			uint32_t loc_temp;
			uint32_t loc_read = this->read_index;
			uint32_t loc_write = this->write_index;
			if(loc_write < loc_read) loc_temp = this->size - loc_read + loc_write;
			else loc_temp = loc_write - loc_read;
			return loc_temp;
		};
		uint32_t count_write(void)
		{
			uint32_t loc_temp;
			uint32_t loc_read = this->read_index;
			uint32_t loc_write = this->write_index;
			if(loc_write < loc_read) loc_temp = loc_read - loc_write - 1;
			else loc_temp = this->size - loc_write + loc_read - 1;
			return loc_temp;
		};
	public:
		fifo(T* memory_ptr, uint32_t memory_size)
		{
			this->buffer = memory_ptr;
			this->size = memory_size;
			this->reset();
		};
		uint32_t write(T* data_in_ptr, uint32_t data_in_size)
		{
			uint32_t temp_loc = this->count_write();
			if(data_in_size > temp_loc) data_in_size = temp_loc;
			for(uint32_t i = 0; i < data_in_size; i++)
			{
				this->buffer[this->write_index++] = data_in_ptr[i];
				if(this->write_index >= this->size) this->write_index = 0;
			}
			return data_in_size;
		};
		uint32_t read(T* data_out_ptr, uint32_t data_out_size)
		{
			uint32_t temp_loc = this->count_read();
			if(data_out_size > temp_loc) data_out_size = temp_loc;
			for(uint32_t i = 0; i < data_out_size; i++)
			{
				data_out_ptr[i] = this->buffer[this->read_index++];
				if(this->read_index >= this->size) this->read_index = 0;
			}
			return data_out_size;
		};
		void reset(void)
		{
			this->write_index = 0;
			this->read_index = 0;
		};
		void print_debug(void* uart)
		{
			char string[11];
			uart_print_string(uart, "wr ");
			uint32_to_hex_string(this->write_index, string);
			uart_print_string(uart, string);
			uart_print_string(uart, "\trd ");
			uint32_to_hex_string(this->read_index, string);
			uart_print_string(uart, string);
			uart_print_string(uart, "\n\r");
		};
};

