#ifndef _BSP_INTERFACE_H_
#define _BSP_INTERFACE_H_

#include <stdint.h>
#include "hal_foc_config.h"

typedef struct hal_frame bsp_frame;

typedef enum
{
	BSP_MSG_FRAME_NULL = 0,
	BSP_MSG_FRAME_CAN,
	BSP_MSG_FRAME_UART,
	BSP_MSG_FRAME_USB,
}BSP_KMD_COM_TYPE;


/**
 * @description: ��ʼ��Ӳ������
 * @return {*}
 */
void bsp_board_init(void);

/**
 * @description: �ṩ��hal�����Ϣ���
 * @param {bsp_frame} *rx_frame
 * @return {*}
 */
bool bsp_receive_callback(bsp_frame *rx_frame);

/**
 * @description: CAN��ʽ������Ϣ
 * @param {uint32_t} frameID
 * @param {uint8_t} *pData
 * @param {uint8_t} len
 * @return {*}
 */
bool bsp_can__transmit_callback(uint32_t frameID,uint8_t *pData,uint8_t len);

/**
 * @description: ���ڷ�ʽ����
 * @param {uint32_t} frameID
 * @param {uint8_t} *pData
 * @param {uint8_t} len
 * @return {*}
 */
bool bsp_uart_transmit_callback(uint32_t frameID,uint8_t *pData,uint8_t len);

/**
 * @description: USB��ʽ����
 * @param {uint32_t} frameID
 * @param {uint8_t} *pData
 * @param {uint8_t} len
 * @return {*}
 */
bool bsp_usb__transmit_callback(uint32_t frameID,uint8_t *pData,uint8_t len);

/**
 * @description: ���δ�ӡ�����ӿ�
 * @param {uint16_t} frameID
 * @param {float} *pData
 * @param {uint16_t} len
 * @return {*}
 */
bool bsp_uart_debug_port_callback(uint16_t frameID,float *pData, uint16_t len);


/**
 * @description: ϵͳ��λ����
 * @return {*}
 */
void bsp_system_reset(void);

/**
 * @description: svpwm�ж�\foc����ص�����
 * @param {uint16_t} adc_vbus
 * @param {uint16_t *} adc_cur_value
 * @return {*}
 */
void bsp_svpwm_callback(uint16_t adc_vbus,uint16_t * adc_cur_value);


#endif
