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
 * @FilePath     : \MDK-ARMe:\KmdFoc\Tools\UsbCanBus\software\CanUsb-V4\CanUsb\CanBus\src\bsp_key.c
 * @Description  :  �˰���չʾ�˻���vSeasky�Լ���Ƶİ���ɨ���㷨��ʵ�ְ�����ͨ���˰���ɨ���㷨��������͵���Դռ�ã��������κ���ʱ��ʵ���ȶ��İ���������
 *                  ��������������ڶ̰����ǳ������ֻ����������µĴ�����
 */
#include "bsp_key.h"
#include "gd32c10x.h"

mx_key_task_info key_t[MAX_KEY_NUM];
/** 
 * @Description  : ��ʼ����������Gpio
 * @return        (*)
 */
void bsp_key_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN_1);
	key_t[0].GPIO_Group = GPIOB;
	key_t[0].GPIO_Pin = GPIO_PIN_1;
}

/** 
 * @Description  : ��ȡ������ƽ״̬��δ����
 * @param         (mx_key_task_info) key_scan_t
 * @return        (*)
 */
uint8_t key_task_scan(mx_key_task_info key_scan_t)
{
    if (gpio_input_bit_get(key_scan_t.GPIO_Group, key_scan_t.GPIO_Pin) == KEY_TASK_PRESS_LEVEL)
    {
        return KEY_TASK_PRESS_S;
    }
    return KEY_TASK_UPSPRING_S;
}

/** 
 * @Description  : ����ɨ���㷨��Ӧ��ʱ2ms���ڵ���
 * @return        (*)
 */
void key_enc_scanf(void)
{
	uint8_t key_count;
	uint8_t key[MAX_KEY_NUM];
    for (key_count = 0; key_count < MAX_KEY_NUM; key_count++)
    {
        // ��ȡ������ƽ
        key[key_count] = key_task_scan(key_t[key_count]);
        if (key[key_count] == KEY_TASK_PRESS_S)
        {
            // �������¼���
            if (key_t[key_count].key_info.time_count < KEY_PRESS_MAX_COUNT)
            {
                key_t[key_count].key_info.time_count++;
            }
        }
        else
        {
            // �����������
            if (key_t[key_count].key_info.res_count < KEY_RES_MAX_COUNT)
            {
                key_t[key_count].key_info.res_count++;
            }
        }
        // ���ﳤ��ʱ��
        if (key_t[key_count].key_info.time_count >= KEY_TASK_TIME_L_COUNT)
        {
            if (key_t[key_count].key_info.states == KEY_TASK_PRESS)
            {
                key_t[key_count].key_info.states = KEY_TASK_PRESS_L_TIME;
                key_t[key_count].key_info.click_num = 0;
                key_t[key_count].key_info.click_state_num = 0;
                // ������λ��������
                key_t[key_count].key_info.res_count = 0;
            }
        }
        // �̰�ʱ��
        else if (key_t[key_count].key_info.time_count >= KEY_TASK_TIME_P_COUNT)
        {
            // �״θ��°����¼�
            if (key_t[key_count].key_info.states == KEY_TASK_UPSPRING)
            {
                key_t[key_count].key_info.states = KEY_TASK_PRESS;
                // ��¼�������´���
                if (key_t[key_count].key_info.click_num < KEY_MAX_NUM)
                {
                    key_t[key_count].key_info.click_num++;
                }
                // ������λ��������
                key_t[key_count].key_info.res_count = 0;
            }
        }
        // ��������ʱ
        if (key_t[key_count].key_info.res_count >= KEY_RES_NUM_COUNT)
        {
            if (key_t[key_count].key_info.click_num != 0)
            {
                key_t[key_count].key_info.click_state_num =
                    key_t[key_count].key_info.click_num;
                key_t[key_count].key_info.click_num = 0;
            }
        }
        // ������λ����
        else if (key_t[key_count].key_info.res_count >= KEY_RES_COUNT)
        {
            if (key_t[key_count].key_info.states != KEY_TASK_UPSPRING)
            {
                key_t[key_count].key_info.states = KEY_TASK_UPSPRING; // ��������λ
                // �������¼�������
                key_t[key_count].key_info.time_count = 0; // ������������
            }
        }
    }
}