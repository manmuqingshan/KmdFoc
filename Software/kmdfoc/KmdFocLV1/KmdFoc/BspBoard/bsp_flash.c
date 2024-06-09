#include <gd32c10x.h>
#include "bsp_flash.h"

/**
 * @description: Flashд����
 * @param {uint32_t} address
 * 				->>��ȡ��ַ
 * @param {uint32_t} *pData
 * 				->>��д������
 * @param {uint32_t} data_num
 * 				->>д��FLASH��ҳ��
 * @param {uint32_t} page_num
 * 				->>��Ҫд���ҳ����
 * @return {*}
 */
int bsp_flash_write(const uint32_t address,uint32_t *const pData,uint32_t data_num,uint32_t page_num)
{
	uint32_t  		erase_check;
	fmc_state_enum  status;
	//�ٽ������뱣��
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
	
	fmc_unlock();
//    ob_unlock();
	
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR);	
	erase_check = 0;
	//������ҳ����
	for (int i = 0; i < page_num; i++)
	{
		status = fmc_page_erase(address + BSP_FMC_PAGE_SIZE * i);
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR);
		if(status!=FMC_READY)
		{
			erase_check++;
		}
	}
	
	if (erase_check == 0)
	{
		for (uint16_t i = 0; i < data_num; i++)
		{
			status = fmc_word_program(address + i * 4, *(pData + i));
			fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR);
			if (status != FMC_READY)
			{
				break;
			}
		}
	}
	fmc_lock();
//    ob_lock();
	//�˳��ٽ������뱣��
	__set_PRIMASK(primask);
	return (status != FMC_READY);
}


/**
 * @description: FLASH��ȡ����
 * @param {uint32_t} address
 * 				->>flash ��ַ
 * @param {uint32_t} *pData
 * 				->>���ݵ�ַ
 * @param {uint32_t} data_num
 * 				->>��Ҫ��ȡ�����ݴ�С
 * @return {*}
 */
int bsp_flash_read_(const uint32_t address,uint32_t *const pData,uint32_t data_num)
{
	int ret = false;
	ret = memcpy(&pData[0], (uint8_t*)address,data_num*4);
    return ret;
}


