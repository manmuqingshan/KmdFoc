#ifndef _BSP_AS5047_H_
#define _BSP_AS5047_H_

#include "main.h"

#define BSP_AS5047P_NOP (0x0000)      // No operation
#define BSP_AS5047P_ERRFL (0x0001)    // Error register
#define BSP_AS5047P_PROG (0x0003)     // Programming register
#define BSP_AS5047P_DIAAGC (0x3FFC)   // Diagnostic and AGC
#define BSP_AS5047P_MAG (0x3FFD)      // CORDIC magnitude
#define BSP_AS5047P_ANGLEUNC (0x3FFE) // Measured angle without dynamic angle error compensation
#define BSP_AS5047P_ANGLECOM (0x3FFF) // Measured angle with dynamic angle error compensation

#define BSP_AS5047_ENCODER_CPR_BIT 14                            // 14BIT
#define BSP_AS5047_ENCODER_CPR (1 << BSP_AS5047_ENCODER_CPR_BIT) // 14BIT

/**
 * @description: ��ʼ��AS5047P�������ӿ�
 * @return {*}
 */
void bsp_as5047p_init(void);

/**
 * @description: ��ȡAS5047P�������Ƕ�
 * @param {uint32_t} *pReadRaw
 * @return {*}
 */
bool bsp_as5047p_read_raw(uint32_t *pReadRaw);

#endif
