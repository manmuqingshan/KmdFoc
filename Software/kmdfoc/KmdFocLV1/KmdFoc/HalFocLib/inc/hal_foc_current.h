#ifndef _HAL_FOC_CALC_H_
#define _HAL_FOC_CALC_H_

#include <hal_foc_struct.h>

static inline void hal_foc_update_current_gain(tHalUsrConfig *pUsrConfig)
{
	pUsrConfig->current_ctrl_p_gain = pUsrConfig->motor_phase_inductance * pUsrConfig->current_ctrl_bandwidth;
	pUsrConfig->current_ctrl_i_gain = pUsrConfig->motor_phase_resistance * pUsrConfig->current_ctrl_bandwidth;
}


// FOC����ص����������жϽӿ����ṩ��ȷ�ı����������ߵ�ѹ�������������ֵ����
void hal_foc_calc_callback(tHalFocStruct *const pHalFoc, uint16_t adc_vbus, uint16_t *adc_cur_value);

// 0����У׼��������
void hal_foc_zero_current_start(tHalFocCurrentStruct *pFocCurrent);

// ʹ�����
void hal_foc_arm(const tHalFocInterface *const pCallbcakFun);

// ʧ�����
void hal_foc_disarm(const tHalFocInterface *const pCallbcakFun);

// FOC�ջ�������ز�������
void hal_foc_current_reset(tHalFocCurrentStruct *pFocCurrent);

// ʩ�ӿ�����ѹ���ṩ��У׼ʹ��
void hal_apply_voltage_timings(tHalFocCurrentStruct *pFocCurrent, float vbus, float v_d, float v_q, float pwm_phase);

// FOC�ջ�,����������ƣ�FOC�任 Clarke�任->>Park�任->>Inverse park�任->>Apply duty
void hal_foc_current(tHalFocCurrentStruct *pFocCurrent);

#endif

