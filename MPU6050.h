#ifndef MPU6050_H
#define MPU6050_H


#include "I2Cdev.h"


// #define _POSIX_C_SOURCE 200809L //Used for calculating time

#define MPU6050_DEF_I2C_ADDRESS 0x68 // I2C address of MPU6050

// Registers for direct access of measurements
#define MPU6050_ACCEL_XOUT_H 		0x3B
#define MPU6050_ACCEL_XOUT_L 		0x3C
#define MPU6050_ACCEL_YOUT_H 		0x3D
#define MPU6050_ACCEL_YOUT_L 		0x3E
#define MPU6050_ACCEL_ZOUT_H 		0x3F
#define MPU6050_ACCEL_ZOUT_L 		0x40
#define MPU6050_TEMP_OUT_H 			0x41
#define MPU6050_TEMP_OUT_L 			0x42
#define MPU6050_GYRO_XOUT_H 		0x43
#define MPU6050_GYRO_XOUT_L 		0x44
#define MPU6050_GYRO_YOUT_H 		0x45
#define MPU6050_GYRO_YOUT_L 		0x46
#define MPU6050_GYRO_ZOUT_H 		0x47
#define MPU6050_GYRO_ZOUT_L 		0x48

#define MPU6050_PWR_MGMT_1          0x6B

#define MPU6050_RA_CONFIG           0x1A
#define MPU6050_RA_GYRO_CONFIG      0x1B
#define MPU6050_RA_ACCEL_CONFIG     0x1C

#define MPU6050_GYRO_FS_250         0x00
#define MPU6050_GYRO_FS_500         0x01
#define MPU6050_GYRO_FS_1000        0x02
#define MPU6050_GYRO_FS_2000        0x03

#define MPU6050_ACCEL_FS_2          0x00
#define MPU6050_ACCEL_FS_4          0x01
#define MPU6050_ACCEL_FS_8          0x02
#define MPU6050_ACCEL_FS_16         0x03

#define MPU6050_GCONFIG_FS_SEL_BIT      4
#define MPU6050_GCONFIG_FS_SEL_LENGTH   2

#define MPU6050_CFG_DLPF_CFG_BIT    2
#define MPU6050_CFG_DLPF_CFG_LENGTH 3

#define MPU6050_DLPF_BW_256         0x00
#define MPU6050_DLPF_BW_188         0x01
#define MPU6050_DLPF_BW_98          0x02
#define MPU6050_DLPF_BW_42          0x03
#define MPU6050_DLPF_BW_20          0x04
#define MPU6050_DLPF_BW_10          0x05
#define MPU6050_DLPF_BW_5           0x06


#define RAD_T_DEG 57.29577951308 //Radians to degrees (180/PI)
#define GRAVITY_ACCEL 9.8067

#define MPU6050_GYRO_FS MPU6050_GYRO_FS_250 
#define MPU6050_ACCEL_FS MPU6050_ACCEL_FS_2 
#define MPU6050_DLPF_BW MPU6050_DLPF_BW_5 


//Select the appropriate settings
#if MPU6050_GYRO_FS == MPU6050_GYRO_FS_500 
	#define MPU6050_GYRO_SENS 65.5
	// #define MPU6050_GYRO_CONFIG MPU6050_GYRO_FS_500
#elif MPU6050_GYRO_FS == MPU6050_GYRO_FS_1000 
	#define MPU6050_GYRO_SENS 32.8
	// #define MPU6050_GYRO_CONFIG MPU6050_GYRO_FS_1000
#elif MPU6050_GYRO_FS == MPU6050_GYRO_FS_2000 
	#define MPU6050_GYRO_SENS 16.4
	// #define MPU6050_GYRO_CONFIG MPU6050_GYRO_FS_2000
#else //Otherwise, default to 0
	#define MPU6050_GYRO_SENS 131.0
	// #define MPU6050_GYRO_CONFIG MPU6050_GYRO_FS_250
#endif
// #undef MPU6050_GYRO_RANGE_MODE


#if MPU6050_ACCEL_FS == MPU6050_ACCEL_FS_4
	#define MPU6050_ACCEL_SENS 8192.0
	// #define MPU6050_ACCEL_CONFIG MPU6050_ACCEL_FS_4
#elif MPU6050_ACCEL_FS == MPU6050_ACCEL_FS_8
	#define MPU6050_ACCEL_SENS 4096.0
	// #define ACCEL_CONFIG MPU6050_ACCEL_FS_8
#elif MPU6050_ACCEL_FS == MPU6050_ACCEL_FS_16
	#define MPU6050_ACCEL_SENS 2048.0
	// #define ACCEL_CONFIG MPU6050_ACCEL_FS_16
#else //Otherwise, default to 0
	#define MPU6050_ACCEL_SENS 16384.0
	// #define ACCEL_CONFIG MPU6050_ACCEL_FS_2
#endif
// #undef MPU6050_ACCEL_RANGE_MODE


class MPU6050 : private I2Cdev {
public:
	typedef struct {
		float x_accel;
		float y_accel;
		float z_accel;
		float tempr;
		float x_rot;
		float y_rot;
		float z_rot;
	} MPU6050_data_t;


	MPU6050(int8_t bus, int8_t addr, MPU6050_data_t *offset = NULL);
#ifdef ESP_PLATFORM
    MPU6050(
        uint8_t devAddr, 
        gpio_num_t sda_pin, 
        gpio_num_t scl_pin, 
        i2c_mode_t mode, 
        uint32_t i2c_freq, 
        MPU6050_data_t *offset);
#else
	MPU6050(int8_t bus, int8_t addr, MPU6050_data_t *offset = NULL);
#endif

	void read_data(MPU6050_data_t *buffer);

	void calibrate();

	void set_offsets(MPU6050_data_t *offsets);
	void get_offsets(MPU6050_data_t *buf);


private:
	MPU6050_data_t cal_offsets;
};


#endif