
#include "bsp_gpio.h"
#include "hal_foc_fsm.h"
#include "bsp_systick.h"



const uint32_t 	kmdLedOffTimer		= 3000;		//��˸��ر�ʱ��
static uint32_t kmdLedCallBackTimer = 250;		//��˸����
static uint32_t kmdLedBrightCount   = 16;		//��˸����
static uint32_t kmdLedOffCount		= 12; 		//��˸���ӳ�


void bsp_led_config(uint32_t run_count,uint16_t led_show);


/**
 * @description: ��ʼ��LED
 * @return {*}
 */
void bsp_led_init(void)
{
	// LED ACT init
	rcu_periph_clock_enable(LED_RCU_GROUP);
	// ��ʼϨ��״̬
	gpio_bit_set(LED_GROUP,LED_Pin);
	gpio_init(LED_GROUP, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, LED_Pin);
	bsp_led_set(0);
}



/**
 * @description: led״̬���ƺ���
 * @return {*}
 */
void bsp_led_callback()
{
	static uint32_t led_control_control = 0;
	static uint32_t led_callback_count = 0;
	//kmdLedCallBackTimer ms
	if(bsp_systick_get_ms_since(led_callback_count) >= kmdLedCallBackTimer)
	{
		if(led_control_control<kmdLedBrightCount*2)
		{
			bsp_led_config(led_control_control,kmdLedBrightCount);
		}
		else
		{
			bsp_led_set(0);
		}
		led_control_control++;
		if(led_control_control>=((kmdLedBrightCount*2)+kmdLedOffCount))
		{
			led_control_control = 0;
		}
		led_callback_count = bsp_systick_get_tick_ms();
	}
}



/**
 * @description: LED ָʾ��
 * @param {uint32_t} run_count
 * @param {uint16_t} led_show
 * @return {*}
 */
void bsp_led_config(uint32_t run_count,uint16_t led_show)
{
	uint32_t led_control;
	if(run_count%2==1)
	{
		//��ǰӦ�����Ƶ�LED
		led_control = (run_count)/2+1;
		bsp_led_set((led_control<=led_show));
	}
	else
	{
		bsp_led_set(0);
	}
}


/**
 * @description: LED״̬����
 * @param {uint32_t} timer
 * @param {uint32_t} brightCount
 * @return {*}
 */
void bsp_led_state_set(uint32_t timer,uint32_t brightCount)
{
	// ��˸����
	kmdLedCallBackTimer = timer;
	// ��˸����
	kmdLedBrightCount	= brightCount;
}

