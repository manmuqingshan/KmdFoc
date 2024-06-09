

#ifndef _HAL_FOC_INTERACT_H_
#define _HAL_FOC_INTERACT_H_

#include <hal_foc_struct.h>


// �������״̬
bool hal_report_state_error(tHalFocInterface  *const pCallbackFun,
							uint32_t reportCanId,
							uint32_t ecode);

// ����У׼����
bool hal_report_calibration(tHalFocInterface  *const pCallbackFun,
							uint32_t reportCanId,
							uint32_t reportId, 
							uint8_t *data);

// ״̬��飬���������ϱ�
void hal_check_loop(tHalFocStruct *const pHalFoc);

// ������Ϣ���
bool hal_receive_callback(tHalFocStruct *pHalFoc, hal_frame_struct *rx_frame);

#endif /* __CAN_H__ */
