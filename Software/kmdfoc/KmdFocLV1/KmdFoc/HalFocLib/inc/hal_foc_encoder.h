#ifndef _HAL_FOC_ENCODER_H_
#define _HAL_FOC_ENCODER_H_

#include <hal_foc_struct.h>


// ��������������&��λ
void hal_encoder_reset_def(tHalEncoderStruct *pEncoder);


// �������ǶȻ�ȡ������
void hal_encoder_sample(tHalEncoderStruct *pEncoder);

#endif
