/*****************************(C) COPYRIGHT 2016 DJI****************************/
#include "BMI088Middleware.h"
#include "main.h"
#include "cmsis_os.h"

extern SPI_HandleTypeDef hspi1;

void bsp_bmi088_gpio_init(void)
{

}

void bsp_bmi088_com_init(void)
{


}

uint64_t bsp_bmi088_get_us(void)
{
	uint64_t vSysTick,vSysTickUs;
	vSysTick = SysTick->LOAD+1 - SysTick->VAL;
	vSysTickUs = vSysTick/168;
	return vSysTickUs;
}


void bsp_bmi088_delay_ms(uint16_t ms)
{
    osDelay(ms);
}
void bsp_bmi088_delay_us(uint16_t us)
{
    uint32_t ticks = 0;
    uint32_t told = 0;
    uint32_t tnow = 0;
    uint32_t tcnt = 0;
    uint32_t reload = 0;
    reload = SysTick->LOAD;
    ticks = us * 168;
    told = SysTick->VAL;
    while (1)
        {
            tnow = SysTick->VAL;
            if (tnow != told)
                {
                    if (tnow < told)
                        {
                            tcnt += told - tnow;
                        }
                    else
                        {
                            tcnt += reload - tnow + told;
                        }
                    told = tnow;
                    if (tcnt >= ticks)
                        {
                            break;
                        }
                }
        }
}

void BSP_BMI088_ACCEL_NS_L(void)
{
    HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_RESET);
}
void BSP_BMI088_ACCEL_NS_H(void)
{
    HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_SET);
}

void BSP_BMI088_GYRO_NS_L(void)
{
    HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_RESET);
}
void BSP_BMI088_GYRO_NS_H(void)
{
    HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_SET);
}

uint8_t bsp_bmi088_read_write_byte(uint8_t txdata)
{
    uint8_t rx_data;
    HAL_SPI_TransmitReceive(&hspi1, &txdata, &rx_data, 1, 1000);
    return rx_data;
}


