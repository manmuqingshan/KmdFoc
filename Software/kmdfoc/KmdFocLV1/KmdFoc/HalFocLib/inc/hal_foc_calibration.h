#ifndef _HAL_FOC_CALIBRATION_H__
#define _HAL_FOC_CALIBRATION_H__

#include <hal_foc_struct.h>


// ��ʼУ׼
void hal_calibration_start(tHalCalibStruct *pCalib);

// ����У׼
void hal_calibration_end(tHalCalibStruct *pCalib);

// У׼ѭ��
void hal_calibration_loop(tHalFocStruct *pHalFoc);

#endif
