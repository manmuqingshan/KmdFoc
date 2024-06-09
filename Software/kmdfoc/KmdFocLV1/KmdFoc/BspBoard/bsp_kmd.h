
#ifndef _BSP_KMD_H_
#define _BSP_KMD_H_

#include "bsp_protocol.h"
#include "bsp_interface.h"
#include <hal_foc_interface.h>

#define KMD_U32_LEN 		12
#define KMD_DEBUG_U32_LEN   24


typedef enum
{
	BSP_EQ_TYPE_NULL = 0,
	BSP_EQ_TYPE_KMD_FOC = 1,
}SkEquipmentEnum;


typedef struct
{
	protocol_struct pTxProtocol;
	protocol_struct pRxProtocol;
	protocol_struct pDebTxProtocol;
}kmd_usb;

/**
 * @description: ��ʼ��SeaskyЭ�������ڴ棬�ֶ������ڴ�
 * @return {*}
 */
void bsp_kmd_init(void);
/**
 * @description: USB�ʹ��ڹ��ý������ݴ�����
 * @param {uint8_t} *pData
 * @param {uint16_t} uLen
 * @param {BSP_KMD_COM_TYPE} pMsgType
 * @return {*}
 */
bool bsp_kmd_callback(uint8_t *pData,uint16_t uLen,BSP_KMD_COM_TYPE pMsgType);

/**
 * @description: USB�ʹ��ڹ��÷���Э�����ݴ�����
 * @param {BSP_KMD_COM_TYPE} tKmdCom_type
 * @return {*}
 */
bool bsp_kmd_transmit(hal_frame_struct *tx_frame,BSP_KMD_COM_TYPE tKmdCom_type);

/**
 * @description: ���ڷ�ʽ��ӡ��������
 * @param {uint16_t} frameID
 * @param {float} *pData
 * @param {uint16_t} len
 * @return {*}
 */
bool bsp_kmd_uart_debug(uint16_t frameID,float *pData, uint16_t len);

#endif
