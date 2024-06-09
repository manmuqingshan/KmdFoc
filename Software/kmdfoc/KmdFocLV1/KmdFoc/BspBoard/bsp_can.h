#ifndef _BSP_CAN_H_
#define _BSP_CAN_H_

#include "stdbool.h"
#include "stdint.h"
#include "hal_foc_interface.h"

/**
 * @description: CAN��ʼ��
 * @return {*}
 */
void bsp_can0_init(void);

/**
 * @description: CAN��Ϣ���ͺ���
 * @return {*}
 */
bool bsp_can0_transmit(hal_frame_struct *tx_frame);

/**
 * @description: can �жϻص�
 * @return {*}
 */
void bsp_can0_irq_callback(void);


#endif
