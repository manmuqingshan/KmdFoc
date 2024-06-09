#include "key_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
mx_key_info key_info_status;
const mx_key_info* get_key_info_pointer(void)
{
	return &key_info_status;
}

uint8_t key_scan(void)
{
	if(KEY0==PRESS_LEVEL)
	{
		if(KEY0==PRESS_LEVEL)return KEY0_PRESS;
	}
	return 0;
}

osThreadId key_sacn_TaskHandle;
void key_task_creat(void)
{
    osThreadDef(key_task,key_scan_task,osPriorityRealtime,0,128);
    key_sacn_TaskHandle = osThreadCreate(osThread(key_task), NULL);
}
void key_scan_task(void const *pvParameters)
{
	uint8_t key;
	TickType_t PreviousWakeTime;
	TickType_t mxTimeIncrement=pdMS_TO_TICKS(TIME_CYCLE);//����ʱʱ��ת��Ϊʱ�����
	PreviousWakeTime = xTaskGetTickCount();//��ȡ��ǰʱ�����
	while(1)
	{
		key = key_scan();
		switch(key)
		{
			case 0:if(key_info_status.res_count<4)key_info_status.res_count++;break;	//������λ������λ��ֹͣ����
			case KEY0_PRESS:
			{	
				key_info_status.res_count = 0;//������λ��������
				key_info_status.key_info.time_count++;
				if(key_info_status.key_info.time_count >= TIME_P_COUNT)key_info_status.key_info.states = KEY_PRESS;//����̰�ʱ��
				if(key_info_status.key_info.time_count >= TIME_L_COUNT)key_info_status.key_info.states = KEY_PRESS_L_TIME;//���ﳤ��ʱ��
			};break;
			default:break;
		}
		if(key_info_status.res_count==4)//��������λ
		{
				key_info_status.key_info.states = KEY_UPSPRING;//��������λ
				key_info_status.key_info.time_count = 0;	//������������
		}
		vTaskDelayUntil(&PreviousWakeTime,mxTimeIncrement);
	}
}
