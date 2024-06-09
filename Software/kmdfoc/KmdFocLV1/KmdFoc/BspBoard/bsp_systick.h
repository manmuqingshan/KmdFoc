#ifndef _BSP_SYSTICK_H__
#define _BSP_SYSTICK_H__

#include "main.h"

extern volatile uint64_t SysTickCnt;

/**
 * @description: ��ʼ��Systick
 * @return {*}
 */
void bsp_systick_init(void);

/**
 * @description: ��ȡϵͳʱ��us
 * @return {*}
 */
uint64_t bsp_systick_get_tick_us(void);

/**
 * @description: ��ȡϵͳʱ���us
 * @param {uint64_t} tick_us
 * @return {*}
 */
uint64_t bsp_systick_get_us_since(uint64_t tick_us);

/**
 * @description: ��ȡϵͳʱ��ms
 * @return {*}
 */
uint64_t bsp_systick_get_tick_ms(void);


/**
 * @description: ����ϵͳʱ���ms
 * @param {uint32_t} tick
 * @return {*}
 */
uint64_t bsp_systick_get_ms_since(uint64_t tick_ms);

/**
 * @description: systick us��ʱ����
 * @param {uint32_t} uS
 * @return {*}
 */
void bsp_systick_delay_us(uint64_t us);


/**
 * @description: ms��ʱ����
 * @param {uint32_t} mS
 * @return {*}
 */
void bsp_systick_delay_ms(uint64_t ms);

#endif
