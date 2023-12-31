#include "MPU6050.h"

#ifdef ESP_PLATFORM
#include <freertos/task.h>
#endif


#ifdef ESP_PLATFORM
MPU6050::MPU6050(
	uint8_t devAddr, 
	gpio_num_t sda_pin, 
	gpio_num_t scl_pin, 
	i2c_mode_t mode, 
	uint32_t i2c_freq, 
	MPU6050_data_t *offset)
	: I2Cdev(
		devAddr,
		sda_pin,
		scl_pin,
		mode,
		i2c_freq) 
#else
MPU6050::MPU6050(int8_t bus, int8_t addr, MPU6050_data_t *offset) 
	: I2Cdev(bus, address) 
#endif
{
    write_byte(MPU6050_PWR_MGMT_1, 0);
    write_bits(MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, MPU6050_ACCEL_FS);
    write_bits(MPU6050_RA_ACCEL_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, MPU6050_GYRO_FS);
    write_bits(MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, MPU6050_DLPF_BW_5);

    if(offset != NULL)
        this->cal_offsets = *offset;
}

void MPU6050::read_data(MPU6050_data_t *buffer) {
	uint8_t data[14];

    read_bytes(MPU6050_ACCEL_XOUT_H, 14, data);

	buffer->x_accel = ((int16_t) ((data[0]  << 8) | data[1])) / MPU6050_ACCEL_SENS - cal_offsets.x_accel;
    buffer->y_accel = ((int16_t) ((data[2]  << 8) | data[3])) / MPU6050_ACCEL_SENS - cal_offsets.y_accel;
    buffer->z_accel = ((int16_t) ((data[4]  << 8) | data[5])) / MPU6050_ACCEL_SENS - cal_offsets.z_accel;
    buffer->tempr  = ((int16_t) ((data[6]  << 8) | data[7])) / 340.0 + 36.53;
    buffer->x_rot  = ((int16_t) ((data[8]  << 8) | data[9])) / MPU6050_GYRO_SENS - cal_offsets.x_rot;
    buffer->y_rot  = ((int16_t) ((data[10] << 8) | data[11]))/ MPU6050_GYRO_SENS - cal_offsets.y_rot;
    buffer->z_rot  = ((int16_t) ((data[12] << 8) | data[13]))/ MPU6050_GYRO_SENS - cal_offsets.z_rot;
}

void MPU6050::calibrate() {
    MPU6050_data_t readings, temp_offset;

    cal_offsets.x_rot = 0.0;
    cal_offsets.x_accel = 0.0;
    cal_offsets.y_rot = 0.0;
    cal_offsets.y_accel = 0.0;
    cal_offsets.z_rot = 0.0;
    cal_offsets.z_accel = 0.0;

    while(1) {
        temp_offset.x_rot = 0.0;
        temp_offset.x_accel = 0.0;
        temp_offset.y_rot = 0.0;
        temp_offset.y_accel = 0.0;
        temp_offset.z_rot = 0.0;
        temp_offset.z_accel = 0.0;

#ifdef ESP_PLATFORM
        vTaskDelay(1000 / portTICK_PERIOD_MS);
#else
		usleep(1000000);
#endif

        for(int i = 0; i < 500; i++) {
			read_data(&readings);

            temp_offset.x_rot 	+= readings.x_rot;
            temp_offset.x_accel += readings.x_accel;
            temp_offset.y_rot 	+= readings.y_rot;
            temp_offset.y_accel += readings.y_accel;
            temp_offset.z_rot 	+= readings.z_rot;
            temp_offset.z_accel += readings.z_accel;

#ifdef ESP_PLATFORM
            vTaskDelay(10 / portTICK_PERIOD_MS);
#else
		    usleep(1000000);
#endif
        }

        temp_offset.x_rot 	/= 500;
        temp_offset.x_accel /= 500;
        temp_offset.y_rot 	/= 500;
        temp_offset.y_accel /= 500;
        temp_offset.z_rot 	/= 500;
        temp_offset.z_accel /= 500;
        temp_offset.z_accel -= 0.999;
        
		read_data(&readings);

        if (readings.x_rot - temp_offset.x_rot <  0.1 && 
            readings.x_rot - temp_offset.x_rot > -0.1 &&
            readings.y_rot - temp_offset.y_rot <  0.1 && 
            readings.y_rot - temp_offset.y_rot > -0.1 &&
            readings.z_rot - temp_offset.z_rot <  0.1 && 
            readings.z_rot - temp_offset.z_rot > -0.1 &&
            readings.x_accel - temp_offset.x_accel <  0.01 && 
            readings.x_accel - temp_offset.x_accel > -0.01 &&
            readings.y_accel - temp_offset.y_accel <  0.01 && 
            readings.y_accel - temp_offset.y_accel > -0.01 &&
            readings.z_accel - temp_offset.z_accel < 1.00 && 
            readings.z_accel - temp_offset.z_accel > 0.99)
        {
            cal_offsets = temp_offset;
            return;
        }
    }
}

void MPU6050::set_offsets(MPU6050_data_t *offsets) {
    if(offsets != NULL)
        this->cal_offsets = *offsets;
}

void MPU6050::get_offsets(MPU6050_data_t *buf) {
    *buf = cal_offsets;
}
