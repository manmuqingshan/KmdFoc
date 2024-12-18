#include "vSkyAhrs.h"
#include "MadgwickAHRS.h"

float imu_scale_factor[3][3] = {BOARD_INSTALL_SPIN_MATRIX};

void vsky_cnversion(float *input, float *out, float scale_factor[3][3], float *cali_offset);

void vsky_imu_offset_config(vsky_ahrs_info *ahrs_info, float *acc_offset, float *gyro_offset, float *mag_offset)
{
    for (uint8_t i = 0; i < 3; i++)
        {
            ahrs_info->vAccel_offset[i] = acc_offset[i];
            ahrs_info->vGyro_offset[i] = gyro_offset[i];
            ahrs_info->vMag_offset[i] = mag_offset[i];
        }
}

float vsky_ahrs_updata(float *accel, float *gyro, float *angle, float Cycle)
{
#ifdef MadgwickAHRS_USER
    return MadgwickAHRSupdateIMU(accel, gyro, angle, Cycle);
#elif
    return MahonyAHRSupdateIMU(accel, gyro, angle, Cycle);
#endif
}
void vsky_ahrs_clear(void)
{
#ifdef MadgwickAHRS_USER
    MadgwickAHRSClear();
#elif
    MahonyAHRSClear();
#endif
}

void vsky_imu_updata(vsky_ahrs_info *ahrs_info, float *acc, float *gyro, float *mag, uint64_t timer_us)
{

    vsky_cnversion(acc,ahrs_info->vAaccel_data,  &imu_scale_factor[0], ahrs_info->vAccel_offset);
    vsky_cnversion(gyro,ahrs_info->vGyro_data,  &imu_scale_factor[0], ahrs_info->vGyro_offset);
    // vsky_cnversion(ahrs_info->vMag_data, mag, &imu_scale_factor[0], ahrs_info->vMag_offset);
    switch (ahrs_info->imu_cali)
        {
        case SKY_IMU_CALI_NULL:
        {
            //记录开始时间，以控制需要等待数据稳定的场合
            ahrs_info->vFirst_time_us = timer_us;

            ahrs_info->vImu_time_us = timer_us - ahrs_info->vFirst_time_us;
            for (uint8_t i = 0; i < 3; i++)
                {
                    ahrs_info->vGyro_cali[i] = 0;
                }
            ahrs_info->imu_cali = SKY_IMU_CALI_READY;

            break;
        }
        break;
        case SKY_IMU_CALI_READY:
        {
            ahrs_info->vImu_time_us = timer_us - ahrs_info->vFirst_time_us;
            //计算周期
            ahrs_info->vCycle_s = (float)(ahrs_info->vImu_time_us - ahrs_info->vImu_time_last_us) * 0.000001;
            vsky_ahrs_updata(ahrs_info->vAaccel_data, ahrs_info->vGyro_data, ahrs_info->vAngle_data, ahrs_info->vCycle_s);
            //等待数据稳定
            if (ahrs_info->vImu_time_us > VSKY_AHRS_CALI_READY_US)
                {

                    ahrs_info->imu_cali = SKY_IMU_CALI_GYRO_START;
                    //记录开始时间，以控制需要等待数据稳定的场合
                    ahrs_info->vFirst_time_us = timer_us;
                }
        }
        break;
        case SKY_IMU_CALI_GYRO_START:
        {
            ahrs_info->vImu_time_us = timer_us - ahrs_info->vFirst_time_us;
            //计算周期
            ahrs_info->vCycle_s = (float)(ahrs_info->vImu_time_us - ahrs_info->vImu_time_last_us) * 0.000001;
            vsky_ahrs_updata(ahrs_info->vAaccel_data, ahrs_info->vGyro_data, ahrs_info->vAngle_data, ahrs_info->vCycle_s);
            for (uint8_t i = 0; i < 3; i++)
                {
                    ahrs_info->vGyro_cali[i] = ahrs_info->vAngle_data[i];
                }
            {

                ahrs_info->vCail_gyro = 0;

                ahrs_info->imu_cali = SKY_IMU_CALI_GYRO_WHILE;
                //记录开始时间，以控制需要等待数据稳定的场合
                ahrs_info->vFirst_time_us = timer_us;
            }
            break;
        }
        break;
        case SKY_IMU_CALI_GYRO_WHILE:
        {
            ahrs_info->vAaccel_data[0] = 0;
            ahrs_info->vAaccel_data[1] = 0;
            ahrs_info->vAaccel_data[2] = 0;
            ahrs_info->vMag_data[0] = 0;
            ahrs_info->vMag_data[1] = 0;
            ahrs_info->vMag_data[2] = 0;
            ahrs_info->vImu_time_us = timer_us - ahrs_info->vFirst_time_us;
            //计算周期
            ahrs_info->vCycle_s = (float)(ahrs_info->vImu_time_us - ahrs_info->vImu_time_last_us) * 0.000001;
            ahrs_info->vCail_gyro += vsky_ahrs_updata(ahrs_info->vAaccel_data, ahrs_info->vGyro_data, ahrs_info->vAngle_data, ahrs_info->vCycle_s);
            //
            if (ahrs_info->vImu_time_us > VSKY_AHRS_CALI_END_US)
                {

                    ahrs_info->imu_cali = SKY_IMU_CALI_GYRO_END;
                    //记录开始时间，以控制需要等待数据稳定的场合
                    ahrs_info->vFirst_time_us = timer_us;
                }
        }
        break;
        case SKY_IMU_CALI_GYRO_END:
        {
            if (ahrs_info->vCail_gyro != 0)
                {
                    for (uint8_t i = 0; i < 3; i++)
                        {
							
                            ahrs_info->vGyro_offset[i] = (ahrs_info->vAngle_data[i] - ahrs_info->vGyro_cali[i]) / ahrs_info->vCail_gyro + ahrs_info->vGyro_offset[i];
                            ahrs_info->vGyro_cali[i] = 0;
                            //此处切换了输入，重新计算
							vsky_ahrs_clear();
                        }

                    ahrs_info->imu_cali = SKY_IMU_CALI_READY_OK;
                    //记录开始时间，以控制需要等待数据稳定的场合
                    ahrs_info->vFirst_time_us = timer_us;
                }
        }
        break;
        case SKY_IMU_CALI_READY_OK:
        {
            //刚切换校准值，会有一段不稳定的时间
            ahrs_info->vImu_time_us = timer_us - ahrs_info->vFirst_time_us;
            //计算周期
            ahrs_info->vCycle_s = (float)(ahrs_info->vImu_time_us - ahrs_info->vImu_time_last_us) * 0.000001;
            vsky_ahrs_updata(ahrs_info->vAaccel_data, ahrs_info->vGyro_data, ahrs_info->vAngle_data, ahrs_info->vCycle_s);
            //等待一定时间，使陀螺仪数据稳定
            if (ahrs_info->vImu_time_us > VSKY_AHRS_CALI_READY_OK_US)
                {
                    ahrs_info->imu_cali = SKY_IMU_CALI_OK;
                    //记录开始时间，以控制需要等待数据稳定的场合
                    ahrs_info->vFirst_time_us = timer_us;
                }
        }
        break;
        case SKY_IMU_CALI_OK:
        {
            ahrs_info->vImu_time_us = timer_us - ahrs_info->vFirst_time_us;
            //计算周期
            ahrs_info->vCycle_s = (float)(ahrs_info->vImu_time_us - ahrs_info->vImu_time_last_us) * 0.000001;
            vsky_ahrs_updata(ahrs_info->vAaccel_data, ahrs_info->vGyro_data, ahrs_info->vAngle_data, ahrs_info->vCycle_s);
        }
        break;
        }
    ahrs_info->vImu_time_last_us = timer_us - ahrs_info->vFirst_time_us;
}

void vsky_cnversion(float *input, float *out, float scale_factor[3][3], float *cali_offset)
{
    for (uint8_t i = 0; i < 3; i++)
        {
            out[i] = input[0] * scale_factor[i][0] + input[1] * scale_factor[i][1] + input[2] * scale_factor[i][2] - cali_offset[i];
        }
}
