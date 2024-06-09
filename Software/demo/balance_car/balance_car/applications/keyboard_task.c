#include "keyboard_task.h"
#include "usb_device.h"
#include "usbd_hid.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "key_task.h"
#include "keyboard_cfg.h"
extern USBD_HandleTypeDef hUsbDeviceFS;

osThreadId keyboard_TaskHandle;
void keyboard_task_creat(void)
{
	osThreadDef(keyboard_task_T,keyboard_task,osPriorityRealtime,0,128);
    keyboard_TaskHandle = osThreadCreate(osThread(keyboard_task_T), NULL);
}

uint8_t txbuffer[8]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//��
uint8_t	sendbuffer[8]={0x00,0x00,0x2c,0x00,0x00,0x00,0x00,0x00};//�ո�
void keyboard_task(void const *pvParameters)
{
	while(1)     
	{
		//���Խ׶�ʹ�ð�����������
		if(key_info_status.key_info.states!=KEY_UPSPRING)//PTT����
		{
			keypad_Update(sendbuffer);
			USBD_HID_SendReport(&hUsbDeviceFS,sendbuffer,8);//���ͱ���
		}
		else if(key_info_status.key_info.states==KEY_UPSPRING)
		{
			USBD_HID_SendReport(&hUsbDeviceFS,txbuffer,8);//���ͱ���			
		}
	}
}

