#include "cmsis_os.h"
#include "bsp_led.h"
#include "tim.h"
/******************************************************************************
aRGB Ϊһ��ɫ��ģʽ��aRGB �ֱ������ alpha��͸���ȣ�Red����ɫ��Green����ɫ����
Blue����ɫ���ĸ�Ҫ��,һ�����Ǹ�ÿ��Ҫ������ʮ������ 0-255 ��ȡֵ��Χ��ͨ�� 16 ��
�Ʊ�ʾ���� 0x00-0xFF�����һ�� aRGB ֵ����ͨ����λʮ������������������ǰ����ÿ
��λ���ζ�Ӧ a��R��G��B�� �� aRGB �У�alpha ֵԽ��ɫ��Խ��͸����RGB ���ĸ�ֵԽ�󣬶�Ӧ��ɫ�ʾ�Խǿ������
����ɫ������ 8 λ 16 ���Ʊ�ʾΪ 0xFFFF0000������ɫ���Ա�ʾΪ 0xFF00FF00������ɫ
���Ա�ʾΪ 0xFF0000FF����ɫ����ɫ����ɫ�ϳɣ����Կ��Ա�ʾΪ 0xFF00FFFF��
******************************************************************************/
//uint32_t RGB_flow_color[RGB_FLOW_COLOR_LENGHT + 1] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF0000FF};
void aRGB_led_show(uint32_t aRGB)
{
    static uint8_t alpha;
    static uint16_t red,green,blue;

    alpha = (aRGB & 0xFF000000) >> 24;
    red = ((aRGB & 0x00FF0000) >> 16) * alpha;
    green = ((aRGB & 0x0000FF00) >> 8) * alpha;
    blue = ((aRGB & 0x000000FF) >> 0) * alpha;

    __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1, blue);
    __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2, green);
    __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_3, red);
}
//void homework_flow_led(void)
//{
//	    uint16_t i, j;
//        float delta_alpha, delta_red, delta_green, delta_blue;
//        float alpha,red,green,blue;
//        uint32_t aRGB;
//        for(i = 0; i < RGB_FLOW_COLOR_LENGHT; i++)
//        {
//            alpha = (RGB_flow_color[i] & 0xFF000000) >> 24;
//            red = ((RGB_flow_color[i] & 0x00FF0000) >> 16);
//            green = ((RGB_flow_color[i] & 0x0000FF00) >> 8);
//            blue = ((RGB_flow_color[i] & 0x000000FF) >> 0);

//            delta_alpha = (float)((RGB_flow_color[i + 1] & 0xFF000000) >> 24) - (float)((RGB_flow_color[i] & 0xFF000000) >> 24);
//            delta_red = (float)((RGB_flow_color[i + 1] & 0x00FF0000) >> 16) - (float)((RGB_flow_color[i] & 0x00FF0000) >> 16);
//            delta_green = (float)((RGB_flow_color[i + 1] & 0x0000FF00) >> 8) - (float)((RGB_flow_color[i] & 0x0000FF00) >> 8);
//            delta_blue = (float)((RGB_flow_color[i + 1] & 0x000000FF) >> 0) - (float)((RGB_flow_color[i] & 0x000000FF) >> 0);

//            delta_alpha /= RGB_FLOW_COLOR_CHANGE_TIME;
//            delta_red /= RGB_FLOW_COLOR_CHANGE_TIME;
//            delta_green /= RGB_FLOW_COLOR_CHANGE_TIME;
//            delta_blue /= RGB_FLOW_COLOR_CHANGE_TIME;
//            for(j = 0; j < RGB_FLOW_COLOR_CHANGE_TIME; j++)
//            {
//                alpha += delta_alpha;
//                red += delta_red;
//                green += delta_green;
//                blue += delta_blue;

//                aRGB = ((uint32_t)(alpha)) << 24 | ((uint32_t)(red)) << 16 | ((uint32_t)(green)) << 8 | ((uint32_t)(blue)) << 0;
//                aRGB_led_show(aRGB);
//                osDelay(1);
//            }
//		}
//}
