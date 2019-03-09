#ifndef XBEE_H
#define XBEE_H
#include <stdint.h>

enum sensorData { TIMESTAMP, BME_TEMP, IMU_TEMP, 
                  PRESSURE, ALTITUDE, 
                  ACC_X, ACC_Y, ACC_Z, 
                  ROLL, PITCH, YAW, 
                  MAG_X, MAG_Y, MAG_Z,
                  ANGULAR_VEL_X,ANGULAR_VEL_Y,ANGULAR_VEL_Z,
                  GRAVITY_ACC_X,GRAVITY_ACC_Y,GRAVITY_ACC_Z, 
                  LINEAR_ACCEL_X,LINEAR_ACCEL_Y,LINEAR_ACCEL_Z,
                  QUATERNION_X, QUATERNION_Y, QUATERNION_Z, QUATERNION_W,
                  STATE, NUM_TYPES
              };



/*
 * Read last data package from buffer to flags[], sensors[] and package_number.
 * Return index of package start byte '<' in buffer
 */
uint8_t read_buffer( uint8_t  *buffer, int buffer_size, 
					 uint8_t  *sensors, int sensors_size,
					 uint16_t *package_number);


#endif