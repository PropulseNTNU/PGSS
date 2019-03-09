#ifndef GS_H
#define GS_H
#include <stdint.h>

/*
 * Read last data package from buffer to flags[], sensors[] and package_number.
 * Return index of package start byte '<' in buffer
 */

uint8_t read_buffer( uint8_t  *buffer,			int buffer_size,
					 uint8_t  *sensors,			int sensors_size,
					 uint8_t  *flags,			int flags_size,
					 uint16_t *package_number);


#endif