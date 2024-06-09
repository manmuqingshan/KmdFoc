#ifndef _HAL_FOC_INTERFACE_H_
#define _HAL_FOC_INTERFACE_H_

//��׼ͷ�ļ�
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _HAL_DEBUG__
#define HAL_DEBUG(...) printf(__VA_ARGS__);
#else
#define HAL_DEBUG(...)
#endif

#ifdef HAL_GD32_LIB_USED
#include "gd32c10x.h"
#endif

//ͨ�ŷ�ʽ
typedef enum eHalKmdMsgType
{
	KMD_MSG_FRAME_NULL = 0,
	KMD_MSG_FRAME_CAN_,
	KMD_MSG_FRAME_UART,
	KMD_MSG_FRAME_USB_,
} tHalKmdMsgType;

typedef struct hal_frame
{
	uint32_t can_id;		 // ����ID
	tHalKmdMsgType msg_type; // ����ͨ�ŷ�ʽ
	uint8_t can_dlc;
	uint8_t data[8];   // ��������
	uint16_t data_len; // ���ݳ���
} hal_frame_struct;

typedef struct eHalTransmitCtr
{
	uint32_t mTransmitTick_ms;
	const uint32_t mTransmitCycle_ms;
	// transmit
	bool (*const hal_transmit__)(uint32_t frameID, uint8_t *pData, uint8_t len);
	// ��ӡ���ݲ���
	bool (*const hal_debug_port)(uint16_t frameID, float *pData, uint16_t len);
	// Systick
	uint32_t (*const hal_systick_get_ms_tick_)(void);
	uint32_t (*const hal_systick_get_ms_since)(uint32_t tick);
} tHalTransmitCtr;

typedef struct eHalTransmitCallback
{
	tHalKmdMsgType type_transmit; //���ݷ�������
	tHalKmdMsgType type_debug;	  //���η�������
	//��������ͨ�ŷ�ʽ
	tHalTransmitCtr hal_can_;
	tHalTransmitCtr hal_uart;
	tHalTransmitCtr hal_usb_;
} tHalTransmitCallback;

typedef struct
{
	// ��λϵͳ
	void (*const hal_system_reset)(void);
	// Systick
	uint64_t (*const hal_systick_get_us_tick_)(void);
	uint64_t (*const hal_systick_get_us_since)(uint64_t tick_us);
	uint64_t (*const hal_systick_get_ms_tick_)(void);
	uint64_t (*const hal_systick_get_ms_since)(uint64_t tick_ms);
	void (*const hal_systick_delay_ms)(uint64_t ms);
	void (*const hal_systick_delay_us)(uint64_t us);
	// Flash
	int (*const hal_flash_write)(const uint32_t address, uint32_t *const pData, uint32_t data_num, uint32_t page_num);
	int (*const hal_flash_read_)(const uint32_t address, uint32_t *const pData, uint32_t data_num);
	// SVPWM
	void (*const hal_svpwm_init)(void);
	void (*const hal_svpwm_apply_duty)(uint16_t duty_a, uint16_t duty_b, uint16_t duty_c);
	void (*const hal_svpwm_switch_off)(void);
	void (*const hal_svpwm_switch_on_)(void);

	// LED״̬����
	void (*const hal_led_set)(uint32_t timer, uint32_t brightCount);
	// Encoder
	bool (*const hal_encoder_read_raw)(int encoder_type, uint32_t *pReadRaw);
	// ��ʼ�����ؽӿ�
	void (*const hal_board_init)(void);
	uint32_t (*const hal_get_mReceiveCycle_ms)(tHalTransmitCallback *pHalTransmitCallback, hal_frame_struct *phal_frame_struct);
	// ������Ϣ
	bool (*const hal_transmit_fun)(tHalTransmitCallback *pHalTransmitCallback, hal_frame_struct *phal_frame_struct);
	bool (*const hal_rx_transmit_fun)(tHalTransmitCallback *pHalTransmitCallback, hal_frame_struct *phal_frame_struct);
	// ��ӡ���ݲ���
	bool (*const hal_port_fun)(tHalTransmitCallback *pHalTransmitCallback, uint16_t frameID, float *pData, uint16_t len);

	void (*const hal_get_device_id)(uint8_t *device_id);

	tHalTransmitCallback hal_transmit_type;
} tHalFocInterface;

static inline uint32_t hal_cpu_enter_critical(void)
{

	uint32_t primask;
#ifdef HAL_GD32_LIB_USED
	primask = __get_PRIMASK();
	__disable_irq();
#endif
	return primask;
}

static inline void hal_cpu_exit_critical(uint32_t priority_mask)
{
#ifdef HAL_GD32_LIB_USED
	__set_PRIMASK(priority_mask);
#endif
}

// ��ʼ��
void hal_foc_init_main(void);

// foc��飬����ѭ��
void hal_foc_check_loop(void);

// mcuӲ������
bool hal_report_err_hanlder(void);


#endif
