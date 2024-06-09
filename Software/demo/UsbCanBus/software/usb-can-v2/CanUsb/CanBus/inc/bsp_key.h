/** 
 * @License      : 
 * @
 * @	<one line to give the program's name and a brief idea of what it does.>
 * @	Copyright (C) 2022  vSeasky.Liu liuwei_seasky@163.com
 * @	This program is free software: you can redistribute it and/or modify
 * @	it under the terms of the GNU General Public License as published by
 * @	the Free Software Foundation, either version 3 of the License, or
 * @	(at your option) any later version.
 * @
 * @	This program is distributed in the hope that it will be useful,
 * @	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * @	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * @	GNU General Public License for more details.
 * @
 * @	You should have received a copy of the GNU General Public License
 * @	along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * @
 * @Author       : Copyright (c) 2022, vSeasky.Liu liuwei_seasky@163.com.
 * @Github       : https://github.com/SEASKY-Master
 * @Date         : 2022-05-27 14:01:11
 * @FilePath     : \MDK-ARMe:\KmdFoc\Tools\UsbCanBus\software\CanUsb-V4\CanUsb\CanBus\inc\bsp_key.h
 * @Description  : 
 */
#ifndef _BSP_KEY_H_
#define _BSP_KEY_H_

#include "stdio.h"
#include "stdint.h"

#define MAX_KEY_NUM 1 //��������
#define KEY_TASK_TIME_CYCLE   2   //��λms
#define KEY_TASK_TIME_P_COUNT 10*2  // TIME_P_TIME/TIME_CYCLE	20ms
#define KEY_TASK_TIME_L_COUNT 750*2 // TIME_L_TIME/TIME_CYCLE	1500ms

#define KEY_PRESS_MAX_COUNT 2000*2 //�������ʱ	4s
#define KEY_RES_MAX_COUNT 	2000*2 //��λ���ʱ	4s
#define KEY_RES_COUNT 		6*2    //������λ�������ʱ��		12ms
#define KEY_RES_NUM_COUNT 	250*2  //��������ֹͣ����ʱ��		500ms
#define KEY_MAX_NUM 100

#define KEY_TASK_PRESS_S 1    //�������·����ź�
#define KEY_TASK_UPSPRING_S 0 //�������𷵻��ź�

#define KEY_TASK_PRESS_LEVEL 0 //���尴������ʱ�ĵ�ƽ

typedef enum
{
	KEY_TASK_UPSPRING = 0,     //��������
	KEY_TASK_PRESS,        	   //��������
	KEY_TASK_PRESS_L_TIME,     //��������
} key_task_states;
typedef struct
{
	key_task_states states;  //����״̬
	uint8_t click_state_num; //�����̰�����
	uint16_t time_count;     //��������ʱ�����
	uint16_t res_count;      //��������ʱ�����
	uint8_t click_num;       //����״̬Ϊ�̰�����
} key_task_time_info;
typedef struct
{
	uint32_t GPIO_Group;
	uint32_t GPIO_Pin;
	uint16_t key_gpio_pin;       //����İ���GPIO
	key_task_time_info key_info; //�����㷨��Ϣ
} mx_key_task_info;

/** 
 * @Description  : ��ʼ����������Gpio
 * @return        (*)
 */	
void bsp_key_init(void);


/** 
 * @Description  : ����ɨ���㷨��Ӧ��ʱ2ms���ڵ���
 * @return        (*)
 */
void key_enc_scanf(void);
#endif