#include "bsp_svpwm.h"
#include "bsp_interface.h"
#include "bsp_systick.h"

uint16_t adc_value[1];
static void bsp_dma_config(void);
static void bsp_adc0_config(void);
static void bsp_adc1_config(void);

/**
 * @description: svpwm��ʼ������
 * @return {*}
 */
void bsp_svpwm_init(void)
{
    timer_parameter_struct timer_initpara;
    timer_oc_parameter_struct timer_ocinitpara;
    timer_break_parameter_struct timer_breakpara;

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_TIMER0);

    // �� PA8/PA9/PA10(TIMER0/CH0/CH1/CH2) ����Ϊ���ù���
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    // �� PB13/PB14/PB15(TIMER0/CH0N/CH1N/CH2N) ����Ϊ���ù���
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

    timer_deinit(TIMER0);
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler = 0;
    timer_initpara.alignedmode = TIMER_COUNTER_CENTER_UP;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = HAL_PWM_ARR_DEF - 1;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    timer_channel_output_struct_para_init(&timer_ocinitpara);
    timer_ocinitpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_ENABLE;
    timer_ocinitpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);
    timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocinitpara);
    timer_channel_output_config(TIMER0, TIMER_CH_2, &timer_ocinitpara);
    timer_channel_output_config(TIMER0, TIMER_CH_3, &timer_ocinitpara);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, (HAL_PWM_ARR_DEF >> 1));
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_ENABLE);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, (HAL_PWM_ARR_DEF >> 1));
    timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_ENABLE);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, (HAL_PWM_ARR_DEF >> 1));
    timer_channel_output_mode_config(TIMER0, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_2, TIMER_OC_SHADOW_ENABLE);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, (HAL_PWM_ARR_DEF - 5));
    timer_channel_output_mode_config(TIMER0, TIMER_CH_3, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);

    // ����TIMER�жϹ���
    timer_break_struct_para_init(&timer_breakpara);
    // �Զ�������á��жϡ�����ʱ�����������
    /*
     *	DTCFG[7:5] = 3��b0xx��DT value = DTCFG[7:0] * tDT��tDT=tDTS��
     *	DTCFG[7:5] = 3��b10x��DT value = (64+DTCFG[5:0]) * tDT��tDT = tDTS*2��
     *	DTCFG[7:5] = 3��b110��DT value = (32+DTCFG[4:0]) * tDT��tDT = tDTS*8��
     *	DTCFG[7:5] = 3��b111��DT value = (32+DTCFG[4:0]) * tDT��tDT = tDTS*16��
     */
    timer_breakpara.runoffstate = TIMER_ROS_STATE_DISABLE;
    timer_breakpara.ideloffstate = TIMER_IOS_STATE_DISABLE;
    // FD6288�Ѿ��Դ����������ܣ���������оƬ�����е�������
    // b0xx
    timer_breakpara.deadtime = 0;
    // timer_breakpara.deadtime = 30;//250ns
    // timer_breakpara.deadtime = 60;//500ns
    // timer_breakpara.deadtime = 90;//750ns
    // timer_breakpara.deadtime = 120;//1000ns
    // b1xx
    // timer_breakpara.deadtime = 0x80+11;//1250ns
    // timer_breakpara.deadtime = 0x80+26;//1500ns
    // timer_breakpara.deadtime = 0x80+41;//1750ns
    // timer_breakpara.deadtime = 0x80+56;//2000ns
    timer_breakpara.breakpolarity = TIMER_BREAK_POLARITY_LOW;
    timer_breakpara.outputautostate = TIMER_OUTAUTO_DISABLE;
    timer_breakpara.protectmode = TIMER_CCHP_PROT_OFF;
    timer_breakpara.breakstate = TIMER_BREAK_DISABLE; 
    timer_break_config(TIMER0, &timer_breakpara);

    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_DISABLE);

    timer_primary_output_config(TIMER0, ENABLE);
    timer_auto_reload_shadow_enable(TIMER0);

    bsp_dma_config();
    bsp_adc0_config();
    bsp_adc1_config();

    bsp_systick_delay_ms(10);
    timer_enable(TIMER0);
}

/**
 * @description: ��ʼ��DMA����
 * @return {*}
 */
static void bsp_dma_config(void)
{
    /* ���� DMA0 ʱ�� */
    rcu_periph_clock_enable(RCU_DMA0);

    /* ADC_DMA_channel ���� */
    dma_parameter_struct dma_data_parameter;

    /* ADC DMA_channel ���� */
    dma_deinit(DMA0, DMA_CH0);

    /* ��ʼ�� DMA ������ģʽ */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr = (uint32_t)(adc_value);
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number = 1;
    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_parameter);

    dma_circulation_enable(DMA0, DMA_CH0);

    /* ���� DMA ͨ�� */
    dma_channel_enable(DMA0, DMA_CH0);
}

/**
 * @description: ��ʼ��ADC
 * @return {*}
 */
static void bsp_adc0_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_ADC0);
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV2);

    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_3); // VBUS
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0); // IA
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_1); // IB

    adc_deinit(ADC0);
    adc_mode_config(ADC_DAUL_INSERTED_PARALLEL);

    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* ADC ����ͨ������ */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_3, ADC_SAMPLETIME_7POINT5);

    /* ADC ����ͨ������*/
    adc_channel_length_config(ADC0, ADC_INSERTED_CHANNEL, 2);
    adc_inserted_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAMPLETIME_7POINT5);
    adc_inserted_channel_config(ADC0, 1, ADC_CHANNEL_1, ADC_SAMPLETIME_7POINT5);

    /* ADC ���津������ */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* ADC ���봥������ */
    adc_external_trigger_source_config(ADC0, ADC_INSERTED_CHANNEL, ADC0_1_EXTTRIG_INSERTED_T0_CH3);
    adc_external_trigger_config(ADC0, ADC_INSERTED_CHANNEL, ENABLE);

    /* ADC �ж� */
    nvic_irq_enable(ADC0_1_IRQn, 0, 0);
    adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOIC);
    adc_interrupt_enable(ADC0, ADC_INT_EOIC);

    /* ���� ADC �ӿ� */
    adc_enable(ADC0);
    bsp_systick_delay_ms(1);

    /* ADCУ׼�͸�λУ׼ */
    adc_calibration_enable(ADC0);

    /* ADC DMA ����ʹ�� */
    adc_dma_mode_enable(ADC0);

    /* ADC �������ʹ�� */
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}

/**
 * @description: ��ʼ��ADC
 * @return {*}
 */
static void bsp_adc1_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_ADC1);

    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_2); // IC

    adc_deinit(ADC1);

    adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);

    /* ADC ����ͨ������ */
    adc_channel_length_config(ADC1, ADC_INSERTED_CHANNEL, 1);
    adc_inserted_channel_config(ADC1, 0, ADC_CHANNEL_2, ADC_SAMPLETIME_7POINT5);

    /* ADC ���봥������ */
    adc_external_trigger_source_config(ADC1, ADC_INSERTED_CHANNEL, ADC0_1_EXTTRIG_INSERTED_NONE);
    adc_external_trigger_config(ADC1, ADC_INSERTED_CHANNEL, ENABLE);

    /* ���� ADC �ӿ� */
    adc_enable(ADC1);
    bsp_systick_delay_ms(1);

    /* ADCУ׼�͸�λУ׼ */
    adc_calibration_enable(ADC1);

    /* ADC �������ʹ�� */
    adc_software_trigger_enable(ADC1, ADC_INSERTED_CHANNEL);
}

/**
 * @description: ����SVPWM
 * @return {*}
 */
void bsp_svpwm_switch_on(void)
{
    /* ��������ռ�ձ�Ϊ 50% */
    TIMER_CH0CV(TIMER0) = (HAL_PWM_ARR_DEF >> 1);
    TIMER_CH1CV(TIMER0) = (HAL_PWM_ARR_DEF >> 1);
    TIMER_CH2CV(TIMER0) = (HAL_PWM_ARR_DEF >> 1);

    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_ENABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_ENABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_ENABLE);

    /* �ȴ��µ� PWM ���� */
    timer_flag_clear(TIMER0, TIMER_FLAG_UP);
    while (RESET == timer_flag_get(TIMER0, TIMER_FLAG_UP))
    {
    };
    timer_flag_clear(TIMER0, TIMER_FLAG_UP);
}

/**
 * @description: �ر�SVPWM
 * @return {*}
 */
void bsp_svpwm_switch_off(void)
{
    /* ��������ռ�ձ�Ϊ 50% */
    TIMER_CH0CV(TIMER0) = (HAL_PWM_ARR_DEF >> 1);
    TIMER_CH1CV(TIMER0) = (HAL_PWM_ARR_DEF >> 1);
    TIMER_CH2CV(TIMER0) = (HAL_PWM_ARR_DEF >> 1);

    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_DISABLE);

    /* �ȴ��µ� PWM ���� */
    timer_flag_clear(TIMER0, TIMER_FLAG_UP);
    while (RESET == timer_flag_get(TIMER0, TIMER_FLAG_UP))
    {
    };
    timer_flag_clear(TIMER0, TIMER_FLAG_UP);
}

/**
 * @description: SVPWM��ֵ
 * @param {uint16_t} duty_a
 * @param {uint16_t} duty_b
 * @param {uint16_t} duty_c
 * @return {*}
 */
void bsp_svpwm_apply_duty(uint16_t duty_a, uint16_t duty_b, uint16_t duty_c)
{
    TIMER_CH0CV(TIMER0) = duty_a;
    TIMER_CH1CV(TIMER0) = duty_b;
    TIMER_CH2CV(TIMER0) = duty_c;
}

/**
 * @description: foc�����жϻص���ȡadc����
 * @return {*}
 */
void bsp_svpwm_irq_callback(void)
{
    uint16_t adc_vbus;
    uint16_t adc_cur_value[3];
    // Clear EOIC
    ADC_STAT(ADC0) &= ~((uint32_t)ADC_INT_FLAG_EOIC);
    adc_vbus = adc_value[0];
    // ��ȡ����ֵ
    adc_cur_value[0] = adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_0);
    adc_cur_value[1] = adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_1);
    adc_cur_value[2] = adc_inserted_data_read(ADC1, ADC_INSERTED_CHANNEL_0);

    bsp_svpwm_callback(adc_vbus,
                       &adc_cur_value[0]);
}
