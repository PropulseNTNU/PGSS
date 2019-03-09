#include "gs.h"

void byte_to_bools(uint8_t bools[], const uint8_t number_of_bools, const uint8_t *byte_with_bools) {
	for (int i = 0; i < number_of_bools; ++i) {
		bools[i] = *byte_with_bools & (1 << i);
	}
}


void read_flags(uint8_t *data, uint8_t count, uint16_t *package_number, int size_compressed_flags, uint8_t *flags, int flags_size) {
	uint8_t number_of_bools;

	//Calculate number of bools in compressed byte. It is 8 unless receiving the last byte of flags.
	if (count < sizeof(*package_number) - 1 + size_compressed_flags) {
		number_of_bools = 8;
	}
	else {
		number_of_bools = flags_size % 8;
	}
	//Decompress
	byte_to_bools(&flags[(count - sizeof(*package_number)) * 8], number_of_bools, data);
}



uint8_t read_buffer(uint8_t *buffer,   int buffer_size, 
			 uint8_t *sensors,  int sensors_size,
			 uint8_t *flags,	int flags_size,
			 uint16_t *package_number) {

	//Calculating size of compressed flags
	uint8_t size_compressed_flags;
	if (flags_size == 0) {
		size_compressed_flags = 0;
	}
	else {
		size_compressed_flags = (flags_size - 1) / 8 + 1;
	}

	
	uint8_t data;
	int package_size = sizeof(*package_number) + size_compressed_flags + sensors_size; 

	//Find end byte in buffer
	int i = buffer_size;
	do {
		data = buffer[--i];
		if (i < 0) { return buffer_size; }
	} while (data != '>');

	i = i - package_size; //Start index

	//Read package data
	for (uint8_t j = 0; j < package_size; ++j) {
		data = buffer[i + j];

		if (j < sizeof(*package_number)) {
			((uint8_t*) package_number)[j] = data;
		}
		else if (j < sizeof(*package_number) + size_compressed_flags) {
			read_flags(&data, j, package_number, size_compressed_flags, flags, flags_size);
		}
		else if (j < package_size) {
			sensors[j - sizeof(*package_number) - size_compressed_flags] = data;
		}

	}
	return i-1;
}

