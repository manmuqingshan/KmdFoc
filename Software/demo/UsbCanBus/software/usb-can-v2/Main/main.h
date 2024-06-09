/*** 
 * @Author                         : Seasky.Liu
 * @Date                           : Do not edit
 * @LastEditTime                   : Do not edit
 * @LastEditors                    : your name
 * @Description                    : https://github.com/SEASKY-Master
 * @FilePath                       : Do not edit
 * @symbol_custom_string_obkoro1          : ��Ȩ����:@Seasky.liu
 * @��ϵ��ʽ:liuwei_seasky@163.com
 * @��ԴЭ��:����ӿ�ԴЭ�飨��Ŀ�ֿ�����˵������δ�����������Ͻ�������ҵ��;
 * @************************************************************************
 * @           If you want a thing done well, do it yourself.
 * @************************************************************************
 */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "gd32c10x.h"

//#define __DEBUG__

#ifdef __DEBUG__
	#define DEBUG(...)		printf(__VA_ARGS__);
#else
	#define DEBUG(...)
#endif

#define USART2_TX_BUFF_SIZE    64U
#define USART2_RX_BUFF_SIZE    64U


#ifdef KMD_USB_USER
bool bsp_usb_transmit(uint8_t *pData,uint16_t len);
#endif

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
