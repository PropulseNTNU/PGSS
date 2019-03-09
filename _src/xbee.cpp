#include "xbee.h"

uint8_t read_buffer(uint8_t *buffer,   int buffer_size, 
			 uint8_t *sensors,  int sensors_size,
			 uint16_t *package_number) {

	
	uint8_t data;
	int package_size = sizeof(*package_number) + sensors_size; 

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
		else if (j < package_size) {
			sensors[j - sizeof(*package_number)] = data;
		}
	}
	return i-1;
}

