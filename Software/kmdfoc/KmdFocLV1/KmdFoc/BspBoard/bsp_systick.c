#include "bsp_systick.h"

#define SYSTICK_US_TICK         (SystemCoreClock / 1000000U)
#define SYSTICK_MS_TICK         (SystemCoreClock / 1000U)
  

volatile uint64_t SysTickCnt = 0;

/**
 * @description: ��ʼ��Systick
 * @return {*}
 */
void bsp_systick_init(void)
{
    // Ϊ 1000Hz �ж����� systick ��ʱ��
    if (SysTick_Config(SYSTICK_MS_TICK))
    {
        /* capture error */
        while (1)
        {
        }
    }
    // ���� systick ����������ȼ�
    NVIC_SetPriority(SysTick_IRQn, 0x02U);

    /* ----------------------------------------------------------------------------
    TIMER1 Configuration:
    TIMER1CLK = SystemCoreClock/120 = 1MHz
    ---------------------------------------------------------------------------- */
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
uint64_t bsp_systick_get_tick_ms(void)
{
    return SysTickCnt;
}

/**
 * @description: ����ϵͳʱ���ms
 * @param {uint32_t} tick
 * @return {*}
 */
uint64_t bsp_systick_get_ms_since(uint64_t tick_ms)
{
    return (uint64_t)(SysTickCnt - tick_ms);
}

/**
 * @description: systick us��ʱ����
 * @param {uint32_t} uS
 * @return {*}
 */
void bsp_systick_delay_us(uint64_t us)
{
    uint64_t elapsed = 0;
    uint64_t last_count = SysTick->VAL;
    for (;;)
    {
        uint64_t current_count = SysTick->VAL;
        uint64_t elapsed_uS;
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
void bsp_systick_delay_ms(uint64_t ms)
{
    while (ms--)
    {
        bsp_systick_delay_us(1000);
    }
}
