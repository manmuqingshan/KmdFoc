#ifndef _BSP_FLASH_H_
#define _BSP_FLASH_H_

#include <stdint.h>
#include <stdbool.h>



//GD32C1x 128������ҳ,����ҳ��СΪ1KB�����洢�����ÿҳ�����Ե���������
#define BSP_FMC_PAGE_SIZE 		((uint32_t)0x400U)	
#define BSP_FMC_PAGE_NUM		(128)

#define BSP_FMC_PAGE_MIN		((uint32_t)(0x8000000 + 100 * FMC_PAGE_SIZE))
#define BSP_FMC_PAGE_MAX		((uint32_t)(0x8000000 + BSP_FMC_PAGE_NUM * FMC_PAGE_SIZE))


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
int bsp_flash_write(const uint32_t address,uint32_t *const pData,uint32_t data_num,uint32_t page_num);


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
int bsp_flash_read_(const uint32_t address,uint32_t *const pData,uint32_t data_num);


#endif
