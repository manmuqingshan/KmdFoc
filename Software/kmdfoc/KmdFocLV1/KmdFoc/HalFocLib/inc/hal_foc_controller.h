#ifndef _HAL_FOC_CONTROLLER_H_
#define _HAL_FOC_CONTROLLER_H_

#include <hal_foc_struct.h>

// λ��ģʽʱ����Ŀ��λ��
void hal_ctr_move_to_pos(tHalFocStruct *pHalFoc, float goal_point, bool isTraj);

// FOC�ջ�������ز�������
void hal_ctr_reset(tHalFocStruct *pHalFoc);

// FOC ����PID����
tHalFocCtrOut * hal_ctr_loop(tHalFocStruct *pHalFoc, int control_mode);


#endif
