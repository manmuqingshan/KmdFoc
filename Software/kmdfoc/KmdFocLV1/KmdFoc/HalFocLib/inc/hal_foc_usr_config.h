#ifndef _HAL_FOC_USR_CONFIG_H_
#define _HAL_FOC_USR_CONFIG_H_

#include <hal_foc_struct.h>

// ����Ĭ�ϲ���
void hal_usr_set_default_config(tHalFocUser *const pFocUser);

// ��FLASH��ȡ����
int hal_usr_read_config(tHalFocUser * const pFocUser);

// ���������FLASH
int hal_usr_save_config(tHalFocUser * const pFocUser);


#endif
