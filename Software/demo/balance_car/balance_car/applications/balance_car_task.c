#include "balance_car_task.h"
#include "bsp_usb.h"
#include "cmsis_os.h"
#include "FreeRtos.h"


#include "hal_kmd_interface.h"
#include "hal_foc_enum.h"
#include "vSkyAhrs.h"
#include "arm_math.h"

#define AngleToRadian (0.01745329251994329576923690768489f)


#define MaxLimit(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }

//�ڲ��ջ��Ѿ����ˣ��˴��ջ�����û��Ҫ̫��
#define BALANCE_CAR_TASK_TIME 5

//extern tHalFocInfo mHalFocInfo1;
//extern tHalFocInfo mHalFocInfo2;
//extern hal_kmd_user_struct tHalKmdUser;
extern vsky_ahrs_info mvsky_ahrs_info;

uint8_t check_cali[2] = {0,0};
uint8_t check_mode[2] = {0,0};
uint8_t check_encoder[2] = {0,0};
int     hal_cpr[2] = {0,0};


float  kmd_foc_vel_kp = 1.5;
float  kmd_foc_vel_ki = 0.00;
//����ÿ5ms�Զ��ϱ�һ�����ݣ������Ҫ�ϱ��������ݣ����һ��5ms�ϱ�HAL_KMD_FSM_HEARTBEAT0,�ڶ����ϱ�HAL_KMD_POS_HEARTBEAT1...
int    kmd_foc_report_ms = 5;
//������Ҫ�ϱ������ݣ��ڴ�ֻ��Ҫת��
int	   kmd_foc_report_ch = ((0<<(HAL_KMD_FSM_HEARTBEAT0-HAL_KMD_FSM_HEARTBEAT0))|
						    (0<<(HAL_KMD_POS_HEARTBEAT1-HAL_KMD_FSM_HEARTBEAT0))|
						    (1<<(HAL_KMD_VEL_HEARTBEAT2-HAL_KMD_FSM_HEARTBEAT0))|
						    (0<<(HAL_KMD_CUR_HEARTBEAT3-HAL_KMD_FSM_HEARTBEAT0))|
						    (0<<(HAL_KMD_BUS_HEARTBEAT4-HAL_KMD_FSM_HEARTBEAT0)));

//float   balance_pid_kp = 0.5;
//float   balance_pid_kd = 0.035;

float   balance_pid_kp = 60.0;
float   balance_pid_kd = 5.6;

//float   balance_pid_kp = 0;
//float   balance_pid_kd = 0;
//float   balance_last_angle = 0;
float   balance_out = 0;
float   balance_pout = 0;
float   balance_dout = 0;
float   balance_sin = 0;

float   velicoty_pid_kp = 10;
float   velicoty_pid_ki = 10;
float   velicoty_last_speed[2] = {0,0};
float   velicoty_out[2] = {0,0};

float   turn_pid_kp = 10;
float   turn_last_yaw;


osThreadId BalanceCarTaskHandle;
static void balance_task(void const *pvParameters);

void balance_task_creat(void)
{
    osThreadDef(BalanceCarTask,balance_task,osPriorityRealtime,0,128);
    BalanceCarTaskHandle = osThreadCreate(osThread(BalanceCarTask), NULL);
}

static void balance_task(void const *pvParameters)
{
    TickType_t peroid = osKernelSysTick();
	osDelay(3000);
    /*KmdFoc���ϵ���һ����ȡ�汾�ŵ�ͨ�ŷ�ʽ�趨�����ϵ�Ŀ��Ʒ�ʽ�������ӿڽ��޷����ڿ���*/
    tHalKmdUser.hal_kmd_get_version_hook(&mHalFocInfo1.__tx_frame,mHalFocInfo1.node_id);
    osDelay(1);
    tHalKmdUser.hal_kmd_get_version_hook(&mHalFocInfo2.__tx_frame,mHalFocInfo2.node_id);
    osDelay(1);
    /*���ͻ�ȡ������Ϣ����,������Ϣ��CAN�жϽ��պ������� mHalFocInfo1.__kmd_user.__config ������,������Ĳ���������int��float���Բ��� kmd_config_map[][2]����*/
    for(uint8_t i=0; i<KMD_CONFIG_MAP_MAX_LENGTH; i++)
        {
			uint8_t mData[4];
			if(kmd_config_map[i][1] == HAL_USER_CONFIG_VEL_GAIN)
			{
				float_to_data(kmd_foc_vel_kp,mData);
			    tHalKmdUser.kmd_config.hal_kmd_user_config_set_hook(&mHalFocInfo1.__tx_frame,mHalFocInfo1.node_id,kmd_config_map[i][1],&mData[0]);
				osDelay(1);
				float_to_data(kmd_foc_vel_kp,mData);
				tHalKmdUser.kmd_config.hal_kmd_user_config_set_hook(&mHalFocInfo2.__tx_frame,mHalFocInfo2.node_id,kmd_config_map[i][1],&mData[0]);
				osDelay(1);
			}
			if(kmd_config_map[i][1] == HAL_USER_CONFIG_VEL_INTEGRATOR_GAIN)
			{

				float_to_data(kmd_foc_vel_ki,mData);
			    tHalKmdUser.kmd_config.hal_kmd_user_config_set_hook(&mHalFocInfo1.__tx_frame,mHalFocInfo1.node_id,kmd_config_map[i][1],&mData[0]);
				osDelay(1);
				float_to_data(kmd_foc_vel_ki,mData);
				tHalKmdUser.kmd_config.hal_kmd_user_config_set_hook(&mHalFocInfo2.__tx_frame,mHalFocInfo2.node_id,kmd_config_map[i][1],&mData[0]);
				osDelay(1);
			}
			if(kmd_config_map[i][1] == HAL_USER_CONFIG_CAN_HEARTBEAT_MS)
			{

				int_to_data(kmd_foc_report_ms,mData);
			    tHalKmdUser.kmd_config.hal_kmd_user_config_set_hook(&mHalFocInfo1.__tx_frame,mHalFocInfo1.node_id,kmd_config_map[i][1],&mData[0]);
				osDelay(1);
				tHalKmdUser.kmd_config.hal_kmd_user_config_set_hook(&mHalFocInfo2.__tx_frame,mHalFocInfo2.node_id,kmd_config_map[i][1],&mData[0]);
				osDelay(1);
			}
			if(kmd_config_map[i][1] == HAL_USER_CONFIG_CAN_HEARTBEAT_CH)
			{

				int_to_data(kmd_foc_report_ms,mData);
			    tHalKmdUser.kmd_config.hal_kmd_user_config_set_hook(&mHalFocInfo1.__tx_frame,mHalFocInfo1.node_id,kmd_config_map[i][1],&mData[0]);
				osDelay(1);
				tHalKmdUser.kmd_config.hal_kmd_user_config_set_hook(&mHalFocInfo2.__tx_frame,mHalFocInfo2.node_id,kmd_config_map[i][1],&mData[0]);
				osDelay(1);
			}
        }

    /*���ͻ�ȡ������Ϣ����,������Ϣ��CAN�жϽ��պ������� mHalFocInfo1.__kmd_user.__config ������,������Ĳ���������int��float���Բ��� kmd_config_map[][2]����*/
    for(uint8_t i=0; i<KMD_CONFIG_MAP_MAX_LENGTH; i++)
        {
            tHalKmdUser.kmd_config.hal_kmd_user_config_get_hook(&mHalFocInfo1.__tx_frame,mHalFocInfo1.node_id,kmd_config_map[i][1]);
            osDelay(1);
            tHalKmdUser.kmd_config.hal_kmd_user_config_get_hook(&mHalFocInfo2.__tx_frame,mHalFocInfo2.node_id,kmd_config_map[i][1]);
            osDelay(1);
        }
//    /*����ǵ�����ȡĳ�������������*/
//    {
//        /*�Ƿ��Ѿ�У׼*/
//        tHalKmdUser.kmd_config.hal_kmd_user_config_get_hook(&mHalFocInfo1.__tx_frame,mHalFocInfo1.node_id,HAL_USER_CONFIG_CALIB_VALID);
//        osDelay(1);
//        tHalKmdUser.kmd_config.hal_kmd_user_config_get_hook(&mHalFocInfo2.__tx_frame,mHalFocInfo2.node_id,HAL_USER_CONFIG_CALIB_VALID);
//        osDelay(1);
//        /*����ģʽ*/
//        tHalKmdUser.kmd_config.hal_kmd_user_config_get_hook(&mHalFocInfo1.__tx_frame,mHalFocInfo1.node_id,HAL_USER_CONFIG_CONTROL_MODE);
//        osDelay(1);
//        tHalKmdUser.kmd_config.hal_kmd_user_config_get_hook(&mHalFocInfo2.__tx_frame,mHalFocInfo2.node_id,HAL_USER_CONFIG_CONTROL_MODE);
//        osDelay(1);
//    }
    /*�ȴ����ݷ���*/
    osDelay(10);

    /*��ѯ����*/
    for(uint8_t i=0; i<KMD_CONFIG_MAP_MAX_LENGTH; i++)
        {
            if(kmd_config_map[i][1] == HAL_USER_CONFIG_CONTROL_MODE)
                {
                    //�Ƿ�����ת��ģʽ
                    if(mHalFocInfo1.__kmd_user.__config[i].value_int == HAL_CONTROL_MODE_CURRENT)
                        {
                            check_mode[0] = 1;
                        }
                    if(mHalFocInfo2.__kmd_user.__config[i].value_int == HAL_CONTROL_MODE_CURRENT)
                        {
                            check_mode[1] = 1;
                        }
                }
            if(kmd_config_map[i][1] == HAL_USER_CONFIG_CALIB_VALID)
                {
                    //�Ƿ��Ѿ�У׼
                    if(mHalFocInfo1.__kmd_user.__config[i].value_int == 1)
                        {
                            check_cali[0] = 1;
                        }
                    if(mHalFocInfo2.__kmd_user.__config[i].value_int == 1)
                        {
                            check_cali[1] = 1;
                        }
                }
            if(kmd_config_map[i][1] == HAL_USER_CONFIG_ENCODER_TYPE)
                {
                    //���������Ƿ�ΪHALL
                    if(mHalFocInfo1.__kmd_user.__config[i].value_int == HAL_ENCODER_HALL)
                        {
                            check_encoder[0] = 1;
                        }
                    if(mHalFocInfo2.__kmd_user.__config[i].value_int == HAL_ENCODER_HALL)
                        {
                            check_encoder[1] = 1;
                        }
                }
            if(kmd_config_map[i][1] == HAL_USER_CONFIG_ENCODER_CPR)
                {
                    /*�����챵��������CPR = 6*������*/
                    /*��챵���ı�������������Ϊ 6*������������У׼������ֵ����������Զ�����*/
                    hal_cpr[0] = mHalFocInfo1.__kmd_user.__config[i].value_int;
                    hal_cpr[1] = mHalFocInfo2.__kmd_user.__config[i].value_int;
                }
            /*��Ҫ�ģ��㻹���Լ��ջ������������Լ��KmdFoc��ֵ�Ƿ���Ч�ı��棬��������ʹ����λ����δ���浽Flash*/
            /************************************************************************************************/
        }
    if((check_cali[0]==0)|(check_cali[1]==0))
        {
            //debug_printf()
            while(1);
        }
    if((check_mode[0]==0)|(check_mode[1]==0))
        {
            //debug_printf()
            while(1);

        }
    if((check_encoder[0]==0)|(check_encoder[1]==0))
        {
            //debug_printf()
            while(1);

        }
    if((check_cali[0]==0)|(check_cali[1]==0))
        {
            //debug_printf()
            while(1);
        }
    if((hal_cpr[0]==90)&(hal_cpr[1]==90))
        {
            tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_enable_hook(&mHalFocInfo1.__tx_frame,mHalFocInfo1.node_id);
            osDelay(1);
            tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_enable_hook(&mHalFocInfo2.__tx_frame,mHalFocInfo2.node_id);
            osDelay(1);
        }
    else
        {
            //debug_printf()
            while(1);
        }
    //�趨һ����С��ת�ٽ��в���
    {
        //���ҵ����ת��Ӧ�÷���
        tHalKmdUser.hal_kmd_set_current_hook(&mHalFocInfo1.__tx_frame,mHalFocInfo1.node_id,0);
        tHalKmdUser.hal_kmd_set_current_hook(&mHalFocInfo2.__tx_frame,mHalFocInfo2.node_id,0);
    }
    {
	
    }
	balance_out = 0;
	balance_pout = 0;
	balance_dout = 0;
    while(1)
        {
            //�ȴ�������У׼Ok���ٿ�ʼ����
            if(SKY_IMU_CALI_OK == mvsky_ahrs_info.imu_cali)
                {
                    /*����ƽ�⻷-˫�������*/
                    {
						if(fabs(mvsky_ahrs_info.vAngle_data[1]-0.4)<45)
						{
							balance_sin = sinf((mvsky_ahrs_info.vAngle_data[1]-0.40)*AngleToRadian);
							//���->>һ�㶼���õ� kp*Angle,���֮��ʹ�� kp*sin(Angle)��³���Ի����
							balance_pout =    balance_pid_kp*balance_sin;
							//�Ƕ�΢��->>ת�٣����ֱ����Gyro���΢����Ի�ø�׼ȷ�Ľ��ٶ�
							balance_dout =    balance_pid_kd*(mvsky_ahrs_info.vGyro_data[1]);
							MaxLimit(balance_pout,40);//35A
							MaxLimit(balance_dout,40);
							balance_out = balance_pout+balance_dout;
							MaxLimit(balance_out,40);
						}
						else
						{
							balance_sin = 0;
							balance_dout = 0;
							balance_out = 0;
						}
                    }
                    /*�����ٶȻ�*/
                    {
                        //��ʱ��д�����Լ��Ŀ�����

                    }
                    /*����ת��*/
                    {
                         //��ʱ��д�����Լ��Ŀ�����
                    }

                    /*�����ٶ�*/
                    {
                        //���ҵ����ת��Ӧ�÷���
                        tHalKmdUser.hal_kmd_set_current_hook(&mHalFocInfo1.__tx_frame,mHalFocInfo1.node_id,-balance_out);
                        tHalKmdUser.hal_kmd_set_current_hook(&mHalFocInfo2.__tx_frame,mHalFocInfo2.node_id,balance_out);
                    }
                }
            else
                {
                    balance_out = 0;
                    /*�����ٶ�*/
                    {
                        //���ҵ����ת��Ӧ�÷���
                        tHalKmdUser.hal_kmd_set_current_hook(&mHalFocInfo1.__tx_frame,mHalFocInfo1.node_id,-balance_out);
                        tHalKmdUser.hal_kmd_set_current_hook(&mHalFocInfo2.__tx_frame,mHalFocInfo2.node_id,balance_out);
                    }
                }
            osDelayUntil(&peroid,BALANCE_CAR_TASK_TIME);
        }
}