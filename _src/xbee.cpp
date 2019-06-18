#include "xbee.h"
#include <qDebug>

uint8_t read_buffer(QByteArray &buffer,
			 uint8_t *sensors,  int sensors_size,
			 uint16_t *package_number) {

	uint8_t data;
	int package_size = sizeof(*package_number) + sensors_size; 

    int i = buffer.size();
	do {
        if (i - package_size <= 0) { return buffer.size(); }
        data = buffer.data()[--i];
    } while (data != '>'  || buffer.data()[i-package_size-1] != '<');

    i = i - package_size; //Start index

	//Read package data
    for (uint8_t j = 0; j < package_size; ++j) {
        data = buffer.data()[i + j];
        if (j < sizeof(*package_number))
			((uint8_t*) package_number)[j] = data;
        else if (j < package_size)
			sensors[j - sizeof(*package_number)] = data;
    }
	return i-1;
}
