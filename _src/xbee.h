#ifndef XBEE_H
#define XBEE_H
#include <stdint.h>
#include <QByteArray>

/* Sensor data enum has to match the one defined on the
   flight computer. */

enum sensorData { TIMESTAMP, ALTITUDE,
                  ACC_Y, ROLL, PITCH, YAW,
                  STATE, LONGITUDE_GPS,
                  LATITUDE_GPS, NUM_TYPES
              };


enum sensorDataBle { TIME, TROLL, TPITCH, TYAW,
                TACCELERATION_X, TACCELERATION_Y, TACCELERATION_Z,
                TCOMPASS_X, TCOMPASS_Y, TCOMPASS_Z ,
                TEMPERATURES , THUMIDITY, TALTITUDE, TPRESSURE, TGAS, TEMP_ACCUR_SENS,
                NUMBER_OF_SENSORS};

/*
 * Read last data package from buffer to flags[], sensors[] and package_number.
 * Return index of package start byte '<' in buffer
 */
uint8_t read_buffer(QByteArray  &buffer, uint8_t  *sensors,
                    int sensors_size,
                    uint16_t *package_number);


#endif
