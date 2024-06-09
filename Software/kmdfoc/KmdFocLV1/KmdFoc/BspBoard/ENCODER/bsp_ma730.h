
#ifndef _BSP_MA730_H_
#define _BSP_MA730_H_

#include "main.h"


#define BSP_MA730_READ  (0X0000)

#define BSP_MA730_ENCODER_CPR_BIT 	14								//14BIT
#define BSP_MA730_ENCODER_CPR 		(1<<BSP_MA730_ENCODER_CPR_BIT)	//14BIT


/**
 * @description:��ʼ���������ӿ� 
 * @return {*}
 */
void bsp_ma730_init(void);


/**
 * @description: ��ȡMA730�������Ƕ�
 * @param {uint32_t} *pReadRaw
 * @return {*}
 */
bool bsp_ma730_read_raw(uint32_t *pReadRaw);

#endif
