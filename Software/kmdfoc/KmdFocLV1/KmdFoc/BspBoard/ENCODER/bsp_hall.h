#ifndef _BSP_HALL_H
#define _BSP_HALL_H

#include "gd32c10x.h"
#include "stdbool.h"

/**
 * @description: ��ʼ��HALL�������ӿ�
 * @return {*}
 */
void bsp_hall_init(void);


/**
 * @description: ��ȡHALL��������ֵ
 * @param {uint32_t} *pReadRaw
 * @return {*}
 */
bool bsp_hall_read_raw(uint32_t *pReadRaw);


#endif
