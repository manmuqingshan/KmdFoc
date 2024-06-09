/** 
 * @License      : 
 * @
 * @	<one line to give the program's name and a brief idea of what it does.>
 * @	Copyright (C) 2022  vSeasky.Liu liuwei_seasky@163.com
 * @	This program is free software: you can redistribute it and/or modify
 * @	it under the terms of the GNU General Public License as published by
 * @	the Free Software Foundation, either version 3 of the License, or
 * @	(at your option) any later version.
 * @
 * @	This program is distributed in the hope that it will be useful,
 * @	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * @	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * @	GNU General Public License for more details.
 * @
 * @	You should have received a copy of the GNU General Public License
 * @	along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * @
 * @Author       : Copyright (c) 2022, vSeasky.Liu liuwei_seasky@163.com.
 * @Github       : https://github.com/SEASKY-Master
 * @Date         : 2022-05-27 14:01:11
 * @FilePath     : \MDK-ARMe:\KmdFoc\Tools\UsbCanBus\software\CanUsb-V4\CanUsb\CanBus\src\bsp_systick.c
 * @Description  : 
 */
#include "bsp_systick.h"

#define SYSTICK_US_TICK         (SystemCoreClock / 1000000U)
#define SYSTICK_MS_TICK         (SystemCoreClock / 1000U)
  

volatile uint32_t SysTickCnt = 0;

/**
 * @description: ��ʼ��Systick
 * @return {*}
 */
void bsp_systick_init(void)
{
    if (SysTick_Config(SYSTICK_MS_TICK))
    {
        while (1)
        {
        }
    }
    // ���� systick ����������ȼ�
    NVIC_SetPriority(SysTick_IRQn, 0x02U);
    timer_parameter_struct timer_initpara;
    rcu_periph_clock_enable(RCU_TIMER1);
    timer_deinit(TIMER1);
    // ��ʼ�� TIMER ��ʼ�������ṹ
    timer_struct_para_init(&timer_initpara);
    // TIMER1 ����
    timer_initpara.prescaler = 120 - 1;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 0xFFFF;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_init(TIMER1, &timer_initpara);
    timer_enable(TIMER1);
}


/**
 * @description: ��ȡϵͳʱ��us
 * @return {*}
 */
uint64_t bsp_systick_get_tick_us(void)
{
	uint64_t vSysTick,vSysTickUs;
	vSysTick = SysTick->LOAD+1 - SysTick->VAL;
	vSysTickUs = (uint64_t)(vSysTick/SYSTICK_US_TICK+SysTickCnt*1000);
	return vSysTickUs;
}


/**
 * @description: ��ȡϵͳʱ���us
 * @param {uint64_t} tick_us
 * @return {*}
 */
uint64_t bsp_systick_get_us_since(uint64_t tick_us)
{
    return (bsp_systick_get_tick_us()-tick_us);
}


/**
 * @description: ��ȡϵͳʱ��ms
 * @return {*}
 */
uint32_t bsp_systick_get_tick_ms(void)
{
    return SysTickCnt;
}

/**
 * @description: ����ϵͳʱ���ms
 * @param {uint32_t} tick
 * @return {*}
 */
uint32_t bsp_systick_get_ms_since(uint32_t tick_ms)
{
    return (uint32_t)(SysTickCnt - tick_ms);
}

/**
 * @description: systick us��ʱ����
 * @param {uint32_t} uS
 * @return {*}
 */
void bsp_systick_delay_us(uint32_t us)
{
    uint32_t elapsed = 0;
    uint32_t last_count = SysTick->VAL;
    for (;;)
    {
        uint32_t current_count = SysTick->VAL;
        uint32_t elapsed_uS;
        // �����������ϴμ������������ʱ��
        if (last_count > current_count)
        {
            elapsed += last_count - current_count;
        }
        else if (last_count < current_count)
        {
            elapsed += last_count + (SYSTICK_MS_TICK - current_count);
        }
        last_count = current_count;
        // ת��Ϊ΢��
        elapsed_uS = elapsed / SYSTICK_US_TICK;
        if (elapsed_uS >= us)
        {
            break;
        }
        // ͨ��������ʱ������ӳ�
        us -= elapsed_uS;
        // Ϊ��һ�ε�������С��΢��
        elapsed %= SYSTICK_US_TICK;
    }
}

/**
 * @description: ms��ʱ����
 * @param {uint32_t} mS
 * @return {*}
 */
void bsp_systick_delay_ms(uint32_t ms)
{
    while (ms--)
    {
        bsp_systick_delay_us(1000);
    }
}
