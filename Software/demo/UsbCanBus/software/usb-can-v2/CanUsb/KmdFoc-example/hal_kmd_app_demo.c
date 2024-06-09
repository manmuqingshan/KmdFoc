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
 * @	You should have received a copy    of the GNU General Public License
 * @	along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * @
 * @Author       : Copyright (c) 2022, vSeasky.Liu liuwei_seasky@163.com.
 * @Github       : https://github.com/SEASKY-Master
 * @Date         : 2022-07-14 11:52:29
 * @FilePath     : \usb-can-v1\CanUsb\KmdFoc-example\hal_kmd_app_demo.c
 * @Description  : �˰���չʾ�� KmdFoc ��һЩ����ʹ�÷������Ա����������ٵĿ��������Ĳ�Ʒ
 */
#include "hal_kmd_app_demo.h"
#include "hal_kmd_interface.h"
#include "hal_kmd_foc.h"
#include "bsp_key.h"
#include "bsp_systick.h"

//�������������ģʽ����Ҫ�������
#define DEMO_MOTOR_NUM 4

#define kmd_delay_ms(t_ms) bsp_systick_delay_ms(t_ms)

#define KNOB_BUTTON_NUM (3.0f)                             //����Բ��ģ���3����ť,�����ʵ������ڴ�3����Ϊչʾ������
#define KNOB_BUTTON_ADD ((360.0f) / KNOB_BUTTON_NUM)       //ת������
#define KNOB_BUTTON_DIFF (KNOB_BUTTON_ADD /2) //�ж���ֵ��

//����ƫ�ƺ͵��������������⣬�������ת���ض�Ӧ�ĵ���������ȫһ�£�����ͬһ��������ֵ�����Լ��õ�����ʾ
#define OBSTACLE_MOMENT (15.0f)              //�������أ�����ת��
#define OBSTACLE_MOMENT_FRACTION (2.0 / 3.0) //��������

#define CURRENT_FILTER (0.65f) //�����˲�ϵ��

//����KPֵ������ֵ�ı���Ϊ���ز�
#define OBSTACLE_MUTUAL_KP1 (30.0f)
#define OBSTACLE_MUTUAL_KP2 (5.0f)

//���ÿ���������ݣ��������Ʒ�Χ���������ã����ܵ����쳣
#define VEL_LIMIT_MIN 0.5f
#define VEL_LIMIT_MAX 90.0f
#define CUR_LIMIT_MIN 2.0f
#define CUR_LIMIT_MAX 40.0f

typedef enum eKmdButtonEvent
{
    KMD_BT_EVENT_NULL = 0,
    KMD_BT_EVENT_GET_VERSION,      //��ȡ�汾�ţ��Թ̶�ͨ�ŷ�ʽ
    KMD_BT_EVENT_MOTOR_ENABLE,     //ʹ�ܵ����ʧ�ܵ��
    KMD_BT_ENVNT_CALI_MOTOR_ENTER, //���е��У׼��ֹͣУ׼
    KMD_BT_EVENT_GET_CONFIG,       //��ȡ���в���
    KMD_BT_ENVNT_SAVE_CONFIG,      //�������
    KMD_BT_ENVNT_RESET_CONFIG,     //��λ���������������
} tKmdButtonEvent;

typedef enum eKmdAppEnum
{
    KMD_APP_EVENT_NULL = 0,
    KMD_APP_EVENT_MOTOR_ENABLE,        //ʹ�ܵ����ʧ�ܵ��
    KMD_APP_POS_KNOB,                  //λ�ÿ��Ʒ�ʽ������ť
    KMD_APP_VEL_KNOB,                  //ת�ٷ�ʽʵ�ֲ�����ť
    KMD_APP_OBSTACLE,                  //����������ť
    KMD_APP_OBSTACLE_BACK,             //���������,���������ٶȶ��ѣ�
    KMD_APP_SPRING,                    //��������Pֵ�𵴺��ٶ����ƶ��ѣ�������ԣ�ʵ���ô�����
    KMD_APP_FREE_MODE,                 //�ֶ���ת������ת��,����ģʽ
    KMD_APP_OBSTACLE_MUTUAL,           //˫���ͬ�������طŴ�
    KMD_APP_OBSTACLE_MUTUAL_ONLY,      //���������1,���ظ���
    KMD_APP_OBSTACLE_MUTUAL_TURN_ONLY, //���������2���о����Ч��
    KMD_APP_ALL,                       //�ĸ��������
} tKmdAppEnum;

static tKmdAppEnum tKmdRunMode = KMD_APP_EVENT_NULL;
static tKmdAppEnum tLastKmdRunMode = KMD_APP_EVENT_NULL;

float ready_set_position[DEMO_MOTOR_NUM];
float ready_set_velocity[DEMO_MOTOR_NUM];
float ready_set_current_[DEMO_MOTOR_NUM];
float realit_position[DEMO_MOTOR_NUM];
float realit_velocity[DEMO_MOTOR_NUM];
float realit_current_[DEMO_MOTOR_NUM];
float ready_set_motor_angle[DEMO_MOTOR_NUM];
float realit_motor_angle[DEMO_MOTOR_NUM];
float filter_motor_curre[DEMO_MOTOR_NUM];
float holding_torque_angle[DEMO_MOTOR_NUM];

extern mx_key_task_info key_t[MAX_KEY_NUM];
extern hal_kmd_user_struct tHalKmdUser;
extern tHalFocInfo mHalFocInfo[4];

// ���÷��������Ի�ȡ���ʵʱ��Ϣ
static void config_heartbeat_ms(int ms, uint8_t motorNum);
// ����λ�û� kp
static void config_vel_gain(float vel_gain, uint8_t motorNum);
// ����λ�û� ki
static void config_vel_integrator_gain(float vel_integrator_gain, uint8_t motorNum);
// ����������������
static void config_heartbeat_ch(int ms, uint8_t motorNum);
// ���ù���ģʽ
static void config_kmd_work_mode(tHalControlMode work_mode, uint8_t motorNum);
// ���õ�������
static void config_current_limit(float curLimit, uint8_t motorNum);
// ����ת������
static void config_velocity_limit(float velLimit, uint8_t motorNum);
// ��ѯ�����ǰģʽ
static tHalFsmStat kmd_check_motor_state(uint8_t motorNum);
// ������Ƿ��Ѿ�У׼
static bool kmd_check_motor_calib_valid(uint8_t motorNum);
// ��ѯ��ȡ�������ò���,ʹ����Ӧ��֪����������
static config_data get_config_data(tHalUsrConfigsCmd tCmd, uint8_t motorNum);
// ���õ��������ʹ����Ӧ��֪�����Ӧ�Ĳ�������
static void set_config_data(tHalUsrConfigsCmd tCmd, config_data tData, uint8_t motorNum);
// �˴�ֻ�Ƿ��ͻ�ȡ�������Ч�ʲ��ȴ����أ�����ֵ���ж����Զ���ֵ
static void kmd_get_config_info(uint8_t motorNum);

// ���δʹ��״̬�°����¼�
void kmd_motor_menu_button_event(tKmdButtonEvent tKmdEvent);
// ���ʹ��״̬�°����¼�
void kmd_motor_mode_button_event(tKmdAppEnum tKmdApp);
// �����¼�
void kmd_button_event(uint16_t tNum);

void kmd_app_run(void);

/**
 * @Description  : kmd focӦ�ó������
 * @return        (*)
 */
void kmd_foc_app_loop(void)
{
    uint16_t buttonEvent;
    buttonEvent = 0;
    if (key_t[0].key_info.click_state_num != 0)
        {
            // ʹ�ð��´��������¼�
            buttonEvent = key_t[0].key_info.click_state_num;
            // ʹ�ú���Ҫ���״̬�������ظ�����
            key_t[0].key_info.click_state_num = 0;
            // �����¼�
            kmd_button_event(buttonEvent);
        }
    kmd_app_run();
}

/**
 * @Description  : �����¼�
 * @param         (uint16_t) tNum
 * @return        (*)
 */
void kmd_button_event(uint16_t tNum)
{

    uint8_t check = 0;
    check = 0;
    for (uint8_t i = 0; i < DEMO_MOTOR_NUM; i++)
        {

            if ((kmd_check_motor_state(i) == HAL_FSM_MOTOR_MODE))
                {
                    check++;
                }
        }
    // ����Ƿ����е��ʹ��
    if (check == DEMO_MOTOR_NUM)
        {
            // ʹ��״̬�°����¼�
            kmd_motor_mode_button_event(tNum);
        }
    else
        {
            // δʹ��״̬�°����¼�
            kmd_motor_menu_button_event(tNum);
        }
}

/**
 * @Description  : ���δʹ��״̬�°����¼�
 * @param         (tKmdButtonEvent) tKmdEvent
 * @return        (*)
 */
void kmd_motor_menu_button_event(tKmdButtonEvent tKmdEvent)
{
    switch (tKmdEvent)
        {
        case KMD_BT_EVENT_GET_VERSION: //��ʼ��
        {
            int heartbeat_ch;
            for (uint8_t j = 0; j < 10; j++)
                {

                    for (uint8_t i = 0; i < DEMO_MOTOR_NUM; i++)
                        {
                            tHalKmdUser.hal_kmd_get_version_hook(&mHalFocInfo[i].__tx_frame,
                                                                 mHalFocInfo[i].node_id);
                            kmd_delay_ms(2);
                            heartbeat_ch =
                                (0 << (HAL_KMD_FSM_HEARTBEAT0 - HAL_KMD_FSM_HEARTBEAT0)) |
                                (1 << (HAL_KMD_POS_HEARTBEAT1 - HAL_KMD_FSM_HEARTBEAT0)) |
                                (1 << (HAL_KMD_VEL_HEARTBEAT2 - HAL_KMD_FSM_HEARTBEAT0)) |
                                (1 << (HAL_KMD_CUR_HEARTBEAT3 - HAL_KMD_FSM_HEARTBEAT0)) |
                                (0 << (HAL_KMD_FSM_HEARTBEAT0 - HAL_KMD_FSM_HEARTBEAT0));
                            config_heartbeat_ch(heartbeat_ch, i);
                            kmd_delay_ms(2);
                        }
                }

            for (uint8_t i = 0; i < DEMO_MOTOR_NUM; i++)
                {
                    // ÿ3ms ����һ֡�������ݣ������ĸ���������֡, ��������������Ϊ 3*4 = 12ms
                    config_heartbeat_ms(4, i);
                    kmd_delay_ms(2);
                    config_vel_gain(2.0f,i);
                    kmd_delay_ms(2);
                    config_vel_integrator_gain(0.125f,i);
                    kmd_delay_ms(2);
                }
        }
        break;
        case KMD_BT_EVENT_MOTOR_ENABLE:
        {
            for (uint8_t i = 0; i < DEMO_MOTOR_NUM; i++)
                {
                    // ���0ʹ�ܿ���
                    if (kmd_check_motor_state(i) != HAL_FSM_MOTOR_MODE)
                        {
                            // ����Ƿ��Ѿ�У׼
                            if (kmd_check_motor_calib_valid(i))
                                {
                                    // ʹ�ܵ��0
                                    tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_enable_hook(&mHalFocInfo[i].__tx_frame,
                                            mHalFocInfo[i].node_id);
                                }
                        }
                    else
                        {
                            // ʧ�ܵ��0
                            tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_disable_hook(&mHalFocInfo[i].__tx_frame,
                                    mHalFocInfo[i].node_id);
                        }
                    kmd_delay_ms(2);
                }
        }
        break;
        case KMD_BT_ENVNT_CALI_MOTOR_ENTER:
        {
            for (uint8_t i = 0; i < DEMO_MOTOR_NUM; i++)
                {
                    // У׼�������
                    if (kmd_check_motor_state(i) != HAL_FSM_CALIBRATION_MODE)
                        {
                            tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_cali_start_hook(
                                &mHalFocInfo[i].__tx_frame,
                                mHalFocInfo[i].node_id);
                        }
                    else
                        {
                            tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_cali_stop_hook(
                                &mHalFocInfo[i].__tx_frame,
                                mHalFocInfo[i].node_id);
                        }
                    kmd_delay_ms(2);
                }
        }
        break;
        case KMD_BT_EVENT_GET_CONFIG:
        {
            for (uint8_t i = 0; i < DEMO_MOTOR_NUM; i++)
                {
                    // ��ȡ���в���
                    kmd_get_config_info(i);
                    kmd_delay_ms(2);
                }
        }
        break;
        case KMD_BT_ENVNT_SAVE_CONFIG:
        {
            for (uint8_t i = 0; i < DEMO_MOTOR_NUM; i++)
                {
                    // �������в���
                    tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_configs_updata_hook(
                        &mHalFocInfo[i].__tx_frame, mHalFocInfo[i].node_id);
                    kmd_delay_ms(2);
                }
        }
        break;
        case KMD_BT_ENVNT_RESET_CONFIG:
        {
            // ��λ���в���
            for (uint8_t i = 0; i < DEMO_MOTOR_NUM; i++)
                {
                    tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_configs_reset_hook(
                        &mHalFocInfo[i].__tx_frame, mHalFocInfo[i].node_id);
                    kmd_delay_ms(2);
                }
        };
        default:
            break;
        }
}

/**
 * @Description  : ���ʹ��״̬�°����¼�
 * @param         (tKmdAppEnum) tKmdApp
 * @return        (*)
 */
void kmd_motor_mode_button_event(tKmdAppEnum tKmdApp)
{
    switch (tKmdApp)
        {
        case KMD_APP_EVENT_MOTOR_ENABLE:
        {
            // ʹ�ܵ������
            kmd_motor_menu_button_event(KMD_BT_EVENT_MOTOR_ENABLE);
            tKmdRunMode = KMD_APP_EVENT_MOTOR_ENABLE;
        }
        break;
        case KMD_APP_POS_KNOB:
        {
            tKmdRunMode = KMD_APP_POS_KNOB;
        }
        break;
        case KMD_APP_VEL_KNOB:
        {
            tKmdRunMode = KMD_APP_VEL_KNOB;
        }
        break;
        case KMD_APP_OBSTACLE:
        {
            tKmdRunMode = KMD_APP_OBSTACLE;
        }
        break;
        case KMD_APP_OBSTACLE_BACK:
        {
            tKmdRunMode = KMD_APP_OBSTACLE_BACK;
        }
        break;
        case KMD_APP_SPRING:
        {
            tKmdRunMode = KMD_APP_SPRING;
        }
        break;
        case KMD_APP_FREE_MODE:
        {
            tKmdRunMode = KMD_APP_FREE_MODE;
        }
        break;
        case KMD_APP_OBSTACLE_MUTUAL:
        {
            tKmdRunMode = KMD_APP_OBSTACLE_MUTUAL;
        }
        break;
        case KMD_APP_OBSTACLE_MUTUAL_ONLY:
        {
            tKmdRunMode = KMD_APP_OBSTACLE_MUTUAL_ONLY;
        }
        break;
        case KMD_APP_OBSTACLE_MUTUAL_TURN_ONLY:
        {
            tKmdRunMode = KMD_APP_OBSTACLE_MUTUAL_TURN_ONLY;
        }
        break;
        case KMD_APP_ALL:
        {
            tKmdRunMode = KMD_APP_ALL;
        }
        break;
        }
    kmd_delay_ms(2);
}

/**
 * @Description  : ʹ��״̬�µ������
 * @return        (*)
 */
void kmd_motor_mode_run(void)
{
    switch (tKmdRunMode)
        {
        case KMD_APP_EVENT_MOTOR_ENABLE:
        {
        }
        break;
        case KMD_APP_POS_KNOB: // ��ťЧ��
        {
            float position_move[DEMO_MOTOR_NUM];
            if (tLastKmdRunMode != tKmdRunMode)
                {
                    int heartbeat_ch = 0;
                    // �״ν��룬����ģʽ�Ͳ���
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            // λ�ù켣ģʽ
                            config_kmd_work_mode(HAL_CONTROL_MODE_POSITION, kmd_num);
                            kmd_delay_ms(2);
                            config_velocity_limit(25      , kmd_num);
                            kmd_delay_ms(2);
                            config_current_limit(35, kmd_num);
                            kmd_delay_ms(2);
                            ready_set_motor_angle[kmd_num] = realit_position[kmd_num] * 360;
                        }
                }
            else
                {
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            position_move[kmd_num] = realit_motor_angle[kmd_num] - ready_set_motor_angle[kmd_num];
                            // ��Ӧ�������ظ�����ֵ
                            if ((fabs(position_move[kmd_num]) > KNOB_BUTTON_DIFF) &&
                                    (fabs(position_move[kmd_num]) <= KNOB_BUTTON_ADD))
                                {
                                    if (position_move[kmd_num] > 0)
                                        {
                                            ready_set_motor_angle[kmd_num] += KNOB_BUTTON_ADD;
                                        }
                                    else
                                        {
                                            ready_set_motor_angle[kmd_num] -= KNOB_BUTTON_ADD;
                                        }
                                    tHalKmdUser.hal_kmd_set_pisotion_hook(
                                        &mHalFocInfo[kmd_num].__tx_frame,
                                        mHalFocInfo[kmd_num].node_id, (ready_set_motor_angle[kmd_num] / 360.0f));
                                    kmd_delay_ms(2);
                                }
                        }
                }
        }
        break;
        case KMD_APP_VEL_KNOB: // P����,�����ٶ�->��ťЧ��
        {
            if (tLastKmdRunMode != tKmdRunMode)
                {
                    // �״ν��룬����ģʽ�Ͳ���
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            // ת��ģʽ
                            config_kmd_work_mode(HAL_CONTROL_MODE_VELOCITY, kmd_num);
                            kmd_delay_ms(2);
                            config_velocity_limit(20, kmd_num);
                            kmd_delay_ms(2);
                            config_current_limit(25, kmd_num);
                            kmd_delay_ms(2);
                            ready_set_motor_angle[kmd_num] = realit_position[kmd_num] * 360;
                        }
                }
            else
                {
                    float velocity_knob[DEMO_MOTOR_NUM];
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            // �˷�ʽ������̫�༶��ť
                            velocity_knob[kmd_num] = sin((realit_position[kmd_num] * 3.14) * (KNOB_BUTTON_NUM * 2));
                            // P����,�˷�ʽ�����𵴣�ʹ��λ�÷�ʽʵ����ť�����ɿ�
                            tHalKmdUser.hal_kmd_set_velocity_hook(
                                &mHalFocInfo[kmd_num].__tx_frame,
                                mHalFocInfo[kmd_num].node_id,
                                velocity_knob[kmd_num] * 2);
                            kmd_delay_ms(2);
                        }
                }
        }
        break;
        case KMD_APP_OBSTACLE:
        {
            // λ�ÿ��Ʒ�ʽʵ������
            if (tLastKmdRunMode != tKmdRunMode)
                {
                    // �״ν��룬����ģʽ�Ͳ���
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            config_kmd_work_mode(HAL_CONTROL_MODE_POSITION, kmd_num);
                            kmd_delay_ms(2);
                            config_velocity_limit(30, kmd_num);
                            kmd_delay_ms(2);
                            config_current_limit(30, kmd_num);
                            kmd_delay_ms(2);
                            ready_set_motor_angle[kmd_num] = realit_position[kmd_num] * 360;
                        }
                }
            else
                {
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            if (fabs(filter_motor_curre[kmd_num]) > OBSTACLE_MOMENT)
                                {
                                    // Ԥ��һ��С��λ�ÿ��������Ա�������
                                    if (realit_motor_angle[kmd_num] < ready_set_motor_angle[kmd_num])
                                        {
                                            ready_set_motor_angle[kmd_num] = realit_motor_angle[kmd_num] + holding_torque_angle[kmd_num];
                                        }
                                    else if (realit_motor_angle[kmd_num] > ready_set_motor_angle[kmd_num])
                                        {
                                            ready_set_motor_angle[kmd_num] = realit_motor_angle[kmd_num] - holding_torque_angle[kmd_num];
                                        }
                                }
                            else if ((fabs(filter_motor_curre[kmd_num]) > (OBSTACLE_MOMENT * OBSTACLE_MOMENT_FRACTION)) &
                                     (fabs(filter_motor_curre[kmd_num]) < OBSTACLE_MOMENT))
                                {
                                    // ��¼ (OBSTACLE_MOMENT_FRACTION) ֮һ���ص�λ��ƫ��
                                    holding_torque_angle[kmd_num] = fabs(ready_set_motor_angle[kmd_num] - realit_motor_angle[kmd_num]);
                                }
                            tHalKmdUser.hal_kmd_set_pisotion_hook(
                                &mHalFocInfo[kmd_num].__tx_frame,
                                mHalFocInfo[kmd_num].node_id, ready_set_motor_angle[kmd_num] / 360.0f);
                            kmd_delay_ms(2);
                        }
                }
        };
        break;
        case KMD_APP_OBSTACLE_BACK:
        {
            if (tLastKmdRunMode != KMD_APP_OBSTACLE_BACK)
                {
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            config_kmd_work_mode(HAL_CONTROL_MODE_POSITION, kmd_num);
                            kmd_delay_ms(2);
                            config_velocity_limit(1.2, kmd_num);
                            kmd_delay_ms(2);
                            config_current_limit(15, kmd_num);
                            kmd_delay_ms(2);
                            ready_set_motor_angle[kmd_num] = realit_position[kmd_num] * 360;
                        }
                }
            else
                {
                }
        }
        break;
        case KMD_APP_SPRING:
        {
            static float target_vel[DEMO_MOTOR_NUM] = {0};
            if (tLastKmdRunMode != KMD_APP_SPRING)
                {
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            config_kmd_work_mode(HAL_CONTROL_MODE_VELOCITY, kmd_num);
                            kmd_delay_ms(2);
                            config_velocity_limit(10, kmd_num);
                            kmd_delay_ms(2);
                            config_current_limit(30, kmd_num);
                            kmd_delay_ms(2);
                            ready_set_motor_angle[kmd_num] = realit_position[kmd_num] * 360;
                            target_vel[kmd_num]=0;
                        }
                }
            else
                {
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {

                            target_vel[kmd_num] = -(realit_motor_angle[kmd_num] - ready_set_motor_angle[kmd_num]) * 0.20;
                            if (target_vel[kmd_num] > 2.0)
                                {
                                    target_vel[kmd_num] = 2.0;
                                }
                            else if (target_vel[kmd_num] < -2.0)
                                {
                                    target_vel[kmd_num] = -2.0;
                                }
                            tHalKmdUser.hal_kmd_set_velocity_hook(
                                &mHalFocInfo[kmd_num].__tx_frame,
                                mHalFocInfo[kmd_num].node_id,
                                target_vel[kmd_num]);
                            kmd_delay_ms(2);
                        }
                }
        }
        break;
        case KMD_APP_FREE_MODE:
        {
            static float vel_measure[DEMO_MOTOR_NUM] = {0, 0};
            static uint16_t vel_add_count[DEMO_MOTOR_NUM] = {0, 0};
            static float vel_max[DEMO_MOTOR_NUM] = {0, 0};
            if (tLastKmdRunMode != tKmdRunMode)
                {
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            config_kmd_work_mode(HAL_CONTROL_MODE_VELOCITY, kmd_num);
                            kmd_delay_ms(2);
                            config_velocity_limit(30, kmd_num);
                            kmd_delay_ms(2);
                            config_current_limit(30, kmd_num);
                            kmd_delay_ms(2);
                            ready_set_motor_angle[kmd_num] = realit_position[kmd_num] * 360;
                            vel_measure[kmd_num]=0;
                            vel_add_count[kmd_num]=0;
                            vel_max[kmd_num]=0;
                        }

                }
            else
                {

                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            // �ٶ�����
                            if (fabs(realit_velocity[kmd_num]) > fabs(vel_max[kmd_num]) * 1.1)
                                {
                                    vel_max[kmd_num] = realit_velocity[kmd_num];
                                    if (vel_add_count[kmd_num] < 1024)
                                        {
                                            vel_add_count[kmd_num]++;
                                        }
                                }
                            else if (fabs(realit_velocity[kmd_num]) <= fabs(vel_max[kmd_num]) * 0.9)
                                {
                                    // �˴�����ת����������һ��ת�����ٵ�ʱ��
                                    if (vel_add_count[kmd_num] > 4)
                                        {
                                            tHalKmdUser.hal_kmd_set_velocity_hook(
                                                &mHalFocInfo[kmd_num].__tx_frame,
                                                mHalFocInfo[kmd_num].node_id,
                                                vel_max[kmd_num]);
                                        }
                                    if (vel_add_count[kmd_num] > 0)
                                        {
                                            vel_add_count[kmd_num]--;
                                        }
                                }
                            if (fabs(realit_current_[kmd_num]) > 5)
                                {
                                    vel_max[kmd_num] = vel_max[kmd_num] * 0.2;
                                    tHalKmdUser.hal_kmd_set_velocity_hook(
                                        &mHalFocInfo[kmd_num].__tx_frame,
                                        mHalFocInfo[kmd_num].node_id,
                                        vel_max[kmd_num]);
                                }
                            kmd_delay_ms(2);
                        }
                }
        };
        break;
        case KMD_APP_OBSTACLE_MUTUAL: //˫���ͬ�������طŴ�
        {
            static float position_difference[DEMO_MOTOR_NUM];
            if (tLastKmdRunMode != tKmdRunMode)
                {
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            config_kmd_work_mode(HAL_CONTROL_MODE_VELOCITY, kmd_num);
                            kmd_delay_ms(2);
                            config_velocity_limit(50, kmd_num);
                            kmd_delay_ms(2);
                            config_current_limit(30, kmd_num);
                            kmd_delay_ms(2);
                            ready_set_motor_angle[kmd_num] = realit_position[kmd_num] * 360;
                        }
                    position_difference[0] = realit_position[1] - realit_position[0];
                    position_difference[1] = realit_position[3] - realit_position[2];
                }
            else
                {
                    // ���0�͵��1����
                    tHalKmdUser.hal_kmd_set_velocity_hook(
                        &mHalFocInfo[0].__tx_frame,
                        mHalFocInfo[0].node_id,
                        -(realit_position[0] - realit_position[1] + position_difference[0]) * OBSTACLE_MUTUAL_KP1);
                    kmd_delay_ms(2);
                    tHalKmdUser.hal_kmd_set_velocity_hook(
                        &mHalFocInfo[1].__tx_frame,
                        mHalFocInfo[1].node_id,
                        -(realit_position[1] - realit_position[0] - position_difference[0]) * OBSTACLE_MUTUAL_KP2);
                    kmd_delay_ms(2);

                    // ���2�͵��3����
                    tHalKmdUser.hal_kmd_set_velocity_hook(
                        &mHalFocInfo[2].__tx_frame,
                        mHalFocInfo[2].node_id,
                        -(realit_position[2] - realit_position[3] + position_difference[1]) * OBSTACLE_MUTUAL_KP1);
                    kmd_delay_ms(2);
                    tHalKmdUser.hal_kmd_set_velocity_hook(
                        &mHalFocInfo[3].__tx_frame,
                        mHalFocInfo[3].node_id,
                        -(realit_position[3] - realit_position[2] - position_difference[1]) * OBSTACLE_MUTUAL_KP2);
                    kmd_delay_ms(2);
                }
        }
        break;
        case KMD_APP_OBSTACLE_MUTUAL_ONLY: //���������1,���ظ���
        {
            static float position_difference[DEMO_MOTOR_NUM];
            static float last_position[DEMO_MOTOR_NUM];
            if (tLastKmdRunMode != tKmdRunMode)
                {
                    config_kmd_work_mode(HAL_CONTROL_MODE_VELOCITY, 0);
                    kmd_delay_ms(2);
                    config_velocity_limit(50, 0);
                    kmd_delay_ms(2);
                    config_current_limit(30, 0);
                    kmd_delay_ms(2);

                    config_kmd_work_mode(HAL_CONTROL_MODE_POSITION, 1);
                    kmd_delay_ms(2);
                    config_velocity_limit(10, 1);
                    kmd_delay_ms(2);
                    config_current_limit(15, 1);
                    kmd_delay_ms(2);

                    position_difference[0] = realit_position[1] - realit_position[0];

                    config_kmd_work_mode(HAL_CONTROL_MODE_VELOCITY, 2);
                    kmd_delay_ms(2);
                    config_velocity_limit(50, 2);
                    kmd_delay_ms(2);
                    config_current_limit(30, 2);
                    kmd_delay_ms(2);

                    config_kmd_work_mode(HAL_CONTROL_MODE_POSITION, 3);
                    kmd_delay_ms(2);
                    config_velocity_limit(10, 3);
                    kmd_delay_ms(2);
                    config_current_limit(15, 3);
                    kmd_delay_ms(2);

                    position_difference[1] = realit_position[3] - realit_position[2];
                }
            else
                {
                    if (((realit_position[0] - last_position[0]) >= 0))
                        {
                            tHalKmdUser.hal_kmd_set_pisotion_hook(
                                &mHalFocInfo[1].__tx_frame,
                                mHalFocInfo[1].node_id,
                                position_difference[0] + realit_position[0]);
                            kmd_delay_ms(2);
                        }
                    else
                        {
                            position_difference[0] = realit_position[1] - realit_position[0];
                        }
                    last_position[0] = realit_position[0];
                    last_position[1] = realit_position[1];

                    if (((realit_position[2] - last_position[2]) >= 0))
                        {
                            tHalKmdUser.hal_kmd_set_pisotion_hook(
                                &mHalFocInfo[3].__tx_frame,
                                mHalFocInfo[3].node_id,
                                position_difference[1] + realit_position[2]);
                            kmd_delay_ms(2);
                        }
                    else
                        {
                            position_difference[1] = realit_position[3] - realit_position[2];
                        }
                    last_position[2] = realit_position[2];
                    last_position[3] = realit_position[3];
                }
        }
        break;
        case KMD_APP_OBSTACLE_MUTUAL_TURN_ONLY: //���������2���о����Ч��
        {
            static float position_difference[DEMO_MOTOR_NUM];
            static float last_position[DEMO_MOTOR_NUM];
            if (tLastKmdRunMode != tKmdRunMode)
                {
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            config_kmd_work_mode(HAL_CONTROL_MODE_VELOCITY, kmd_num);
                            kmd_delay_ms(2);
                            config_velocity_limit(50, kmd_num);
                            kmd_delay_ms(2);
                            config_current_limit(30, kmd_num);
                            kmd_delay_ms(2);
                        }
                    position_difference[0] = realit_position[1] - realit_position[0];
                    position_difference[1] = realit_position[3] - realit_position[2];
                }
            else
                {
                    if (((realit_position[0] - last_position[0]) >= 0))
                        {
                            tHalKmdUser.hal_kmd_set_velocity_hook(
                                &mHalFocInfo[1].__tx_frame,
                                mHalFocInfo[1].node_id,
                                -(realit_position[1] - realit_position[0] - position_difference[0]) * OBSTACLE_MUTUAL_KP2);
                            kmd_delay_ms(2);
                        }
                    else
                        {
                            position_difference[0] = realit_position[1] - realit_position[0];
                        }
                    last_position[0] = realit_position[0];
                    last_position[1] = realit_position[1];

                    if (((realit_position[2] - last_position[2]) >= 0))
                        {
                            tHalKmdUser.hal_kmd_set_velocity_hook(
                                &mHalFocInfo[3].__tx_frame,
                                mHalFocInfo[3].node_id,
                                -(realit_position[3] - realit_position[2] - position_difference[1]) * OBSTACLE_MUTUAL_KP2);
                            kmd_delay_ms(2);
                        }
                    else
                        {
                            position_difference[1] = realit_position[3] - realit_position[2];
                        }
                    last_position[2] = realit_position[2];
                    last_position[3] = realit_position[3];
                }
        }
        break;
        case KMD_APP_ALL:
        {

            static float position_difference[DEMO_MOTOR_NUM];
            if (tLastKmdRunMode != tKmdRunMode)
                {
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            config_kmd_work_mode(HAL_CONTROL_MODE_VELOCITY, kmd_num);
                            kmd_delay_ms(2);
                            config_velocity_limit(50, kmd_num);
                            kmd_delay_ms(2);
                            config_current_limit(30, kmd_num);
                            kmd_delay_ms(2);
                            ready_set_motor_angle[kmd_num] = realit_position[kmd_num] * 360;
                        }
                    position_difference[0] = realit_position[1] - realit_position[0];//0-1����
                    position_difference[1] = realit_position[2] - realit_position[1];//1-2����
                    position_difference[2] = realit_position[3] - realit_position[2];//2-3����
                    position_difference[3] = realit_position[0] - realit_position[3];//3-0����
                }
            else
                {
                    tHalKmdUser.hal_kmd_set_velocity_hook(
                        &mHalFocInfo[0].__tx_frame,
                        mHalFocInfo[0].node_id,
                        -((realit_position[0] - realit_position[1] + position_difference[0])+(realit_position[0] - realit_position[3] - position_difference[3])) * OBSTACLE_MUTUAL_KP1);
                    kmd_delay_ms(2);
                    tHalKmdUser.hal_kmd_set_velocity_hook(
                        &mHalFocInfo[1].__tx_frame,
                        mHalFocInfo[1].node_id,
                        -((realit_position[1] - realit_position[0] - position_difference[0]) + (realit_position[1] - realit_position[2] + position_difference[1])) * OBSTACLE_MUTUAL_KP2);
                    kmd_delay_ms(2);

                    tHalKmdUser.hal_kmd_set_velocity_hook(
                        &mHalFocInfo[2].__tx_frame,
                        mHalFocInfo[2].node_id,
                        -((realit_position[2] - realit_position[3] + position_difference[2]) + (realit_position[2] - realit_position[1] - position_difference[1])) * OBSTACLE_MUTUAL_KP1);
                    kmd_delay_ms(2);
                    tHalKmdUser.hal_kmd_set_velocity_hook(
                        &mHalFocInfo[3].__tx_frame,
                        mHalFocInfo[3].node_id,
                        -((realit_position[3] - realit_position[2] - position_difference[2])+(realit_position[3] - realit_position[0] + position_difference[3])) * OBSTACLE_MUTUAL_KP2);
                    kmd_delay_ms(2);
                }
        }
        break;
        default:
            break;
        }
    if (tLastKmdRunMode != tKmdRunMode)
        {
            tLastKmdRunMode = tKmdRunMode;
        }
    kmd_delay_ms(2);
}

/**
 * @Description  : ���ʹ��״̬��Ӧ��
 * @return        (*)
 */
void kmd_app_run(void)
{
    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
        {
            ready_set_position[kmd_num] = mHalFocInfo[kmd_num].__kmd_ctr.__closed_loop.ready_set.position;
            ready_set_velocity[kmd_num] = mHalFocInfo[kmd_num].__kmd_ctr.__closed_loop.ready_set.velocity;
            ready_set_current_[kmd_num] = mHalFocInfo[kmd_num].__kmd_ctr.__closed_loop.ready_set.current_;
            realit_position[kmd_num] = mHalFocInfo[kmd_num].__kmd_ctr.__closed_loop.realit.position;
            realit_velocity[kmd_num] = mHalFocInfo[kmd_num].__kmd_ctr.__closed_loop.realit.velocity;
            realit_current_[kmd_num] = mHalFocInfo[kmd_num].__kmd_ctr.__closed_loop.realit.current_;
            //����һ��仯�ϴ��ʵ����˲��������Ե���ƽ����
            //һ�׵�ͨ�˲�
            filter_motor_curre[kmd_num] = filter_motor_curre[kmd_num] * (CURRENT_FILTER) + realit_current_[kmd_num] * (1 - CURRENT_FILTER);

            realit_motor_angle[kmd_num] = realit_position[kmd_num] * 360;
        }
    uint8_t check = 0;
    check = 0;
    for (uint8_t i = 0; i < DEMO_MOTOR_NUM; i++)
        {

            if ((kmd_check_motor_state(i) == HAL_FSM_MOTOR_MODE))
                {
                    check++;
                }
        }
    // ����Ƿ����е��ʹ��
    if (check >= 1)
        {
            // ʹ��״̬�°����¼�
            kmd_motor_mode_run();
        }
}

/*****************************************************************/

/**
 * @Description  : ���ù���ģʽ
 * @param         (tHalControlMode) work_mode
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static void config_kmd_work_mode(tHalControlMode work_mode, uint8_t motorNum)
{
    config_data tData;

    // ��Ӧ���У���������������ģʽ
    if ((work_mode == HAL_CONTROL_MODE_CURRENT) | (work_mode == HAL_CONTROL_MODE_CURRENT_RAMP))
        {
            return;
        }
    tData.value_int = work_mode;
    // ʧ�ܵ��
    tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_disable_hook(
        &mHalFocInfo[motorNum].__tx_frame,
        mHalFocInfo[motorNum].node_id);
    kmd_delay_ms(2);

    // ���õ��ģʽ
    set_config_data(HAL_USER_CONFIG_CONTROL_MODE, tData, motorNum);
    kmd_delay_ms(2);

    // ʹ�ܵ��
    tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_enable_hook(
        &mHalFocInfo[motorNum].__tx_frame,
        mHalFocInfo[motorNum].node_id);
    kmd_delay_ms(2);
}

/**
 * @Description  : ���÷��������Ի�ȡ���ʵʱ��Ϣ
 * @param         (int) ms
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static void config_heartbeat_ms(int ms, uint8_t motorNum)
{
    config_data tData;
    if (ms < 2)
        {
            return;
        }
    tData.value_int = ms;
    set_config_data(HAL_USER_CONFIG_CAN_HEARTBEAT_MS, tData, motorNum);
}


/**
 * @Description  :
 * @param         (int) ms
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static void config_vel_gain(float vel_gain, uint8_t motorNum)
{
    config_data tData;
    tData.value_float = vel_gain;
    set_config_data(HAL_USER_CONFIG_VEL_GAIN, tData, motorNum);
}

/**
 * @Description  :
 * @param         (int) ms
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static void config_vel_integrator_gain(float vel_integrator_gain, uint8_t motorNum)
{
    config_data tData;
    tData.value_float = vel_integrator_gain;
    set_config_data(HAL_USER_CONFIG_VEL_INTEGRATOR_GAIN, tData, motorNum);
}


/**
 * @Description  : ����������������
 * @param         (int) ms
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static void config_heartbeat_ch(int ms, uint8_t motorNum)
{
    config_data tData;
    if (ms < 2)
        {
            return;
        }
    tData.value_int = ms;
    set_config_data(HAL_USER_CONFIG_CAN_HEARTBEAT_CH, tData, motorNum);
}

/**
 * @Description  : ���õ�������
 * @param         (float) curLimit
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static void config_current_limit(float curLimit, uint8_t motorNum)
{
    config_data tData;
    if (curLimit<CUR_LIMIT_MIN | curLimit> CUR_LIMIT_MAX)
        {
            return;
        }
    tData.value_float = curLimit;
    set_config_data(HAL_USER_CONFIG_CURRENT_LIMIT, tData, motorNum);
}

/**
 * @Description  : ����ת������
 * @param         (float) velLimit
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static void config_velocity_limit(float velLimit, uint8_t motorNum)
{
    config_data tData;
    if (velLimit < VEL_LIMIT_MIN)
        {
            velLimit = VEL_LIMIT_MIN;
        }
    else if (velLimit > VEL_LIMIT_MAX)
        {
            velLimit = VEL_LIMIT_MAX;
        }
    tData.value_float = velLimit;
    set_config_data(HAL_USER_CONFIG_VEL_LIMIT, tData, motorNum);
}

/**
 * @Description  : ���õ��������ʹ����Ӧ��֪�����Ӧ�Ĳ�������
 * @param         (tHalUsrConfigsCmd) tCmd
 * @param         (config_data) tData
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static void set_config_data(tHalUsrConfigsCmd tCmd, config_data tData, uint8_t motorNum)
{
    uint16_t index = 0;
    uint8_t pData[4];
    for (index = 0; index < KMD_CONFIG_MAP_MAX_LENGTH; index++)
        {
            if (kmd_config_map[index][1] == tCmd)
                {
                    switch (kmd_config_map[index][0])
                        {
                        case KMD_TYPE_FLOAT:
                        {
                            float_to_data(tData.value_float, &pData[0]);
                        }
                        break;
                        case KMD_TYPE_INT:
                        {
                            int_to_data(tData.value_int, &pData[0]);
                        }
                        break;
                        }
                    tHalKmdUser.kmd_config.hal_kmd_user_config_set_hook(
                        &mHalFocInfo[motorNum].__tx_frame,
                        mHalFocInfo[motorNum].node_id,
                        kmd_config_map[index][1], pData);
                }
        }
}

/**
 * @Description  : ��ѯ��ȡ�������ò���,ʹ����Ӧ��֪����������
 * @param         (tHalUsrConfigsCmd) tCmd
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static config_data get_config_data(tHalUsrConfigsCmd tCmd, uint8_t motorNum)
{
    uint16_t index = 0;
    for (index = 0; index < KMD_CONFIG_MAP_MAX_LENGTH; index++)
        {
            if (kmd_config_map[index][1] == tCmd)
                {
                    return mHalFocInfo[motorNum].__kmd_user.__config[index];
                }
        }
}

/**
 * @Description  : ��ѯ�����ǰģʽ
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static tHalFsmStat kmd_check_motor_state(uint8_t motorNum)
{
    return mHalFocInfo[motorNum].__kmd_ctr.__motor_states.motor_fsm_mode;
}

/**
 * @Description  : ������Ƿ��Ѿ�У׼
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static bool kmd_check_motor_calib_valid(uint8_t motorNum)
{
    return get_config_data(HAL_USER_CONFIG_CALIB_VALID, motorNum).value_int;
}

/**
 * @Description  : �˴�ֻ�Ƿ��ͻ�ȡ�������Ч�ʲ��ȴ����أ�����ֵ���ж����Զ���ֵ
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static void kmd_get_config_info(uint8_t motorNum)
{
    uint16_t config_index = 0;
    //��ȡ Kmd-Foc�汾���ϵ��״λ������״η��ͻ�ȡ��ȡ�汾�ŵ�ͨ�ŷ�ʽ�����ڿ��ƣ�ֱ�������ϵ�
    tHalKmdUser.hal_kmd_get_version_hook(&mHalFocInfo[0].__tx_frame, mHalFocInfo[0].node_id);
    kmd_delay_ms(2);
    //��ȡ������Ϣ
    for (config_index = 0; config_index < KMD_CONFIG_MAP_MAX_LENGTH; config_index++)
        {
            tHalKmdUser.kmd_config.hal_kmd_user_config_get_hook(
                &mHalFocInfo[motorNum].__tx_frame,
                mHalFocInfo[motorNum].node_id,
                kmd_config_map[config_index][1]);
            kmd_delay_ms(2);
        }
}