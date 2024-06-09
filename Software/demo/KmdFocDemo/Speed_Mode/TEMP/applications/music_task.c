#include "music_task.h"

#include "bsp_buzzer.h"
#include "cmsis_os.h"
#include "mx_music_config.h"

osThreadId musicTaskHandle;
void music_task_creat(void)
{
    osThreadDef(musicTask,musicPlay_task,osPriorityRealtime,0,128);
    musicTaskHandle = osThreadCreate(osThread(musicTask), NULL);
}
//��������������
//useFreq������Ƶ��
void music_update(unsigned short useFreq)   //useFreq�Ƿ���Ƶ�ʣ�����ʵ����һ��������Ƶ�ʡ�
{
    uint32_t period;
    if((useFreq<=BUZZER_HARDWARE_MAX_FREQ/65536UL)||(useFreq>20000))
        {
            buzzer_off();
        }
    else
        {
            period=(BUZZER_HARDWARE_MAX_FREQ/useFreq)-1;//ϵͳʱ�ӳ�����ʵ����Ƶ�ʣ�װ��ARR
            buzzer_on(period,period/1000);
        }
}
//Play Music
void musicPlay_task(void const *pvParameters)
{
    static uint16_t i=0;
    while(1)
        {
            if(i<HuaFeng_Length)
                {
                    if(HuaFeng[i].mTime>0)
                        {
                            music_update(HuaFeng[i].mName);
                            osDelay(HuaFeng[i].mTime);
                            i++;
                        }
                }
            else if(i>=HuaFeng_Length)
                {
                    osDelay(500);
                    i=0;
                }
        }
}
