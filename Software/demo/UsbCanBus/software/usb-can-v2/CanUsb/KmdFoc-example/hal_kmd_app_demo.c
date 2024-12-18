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
 * @Description  : 此案例展示了 KmdFoc 的一些具体使用方法，以便于您更快速的开发自生的产品
 */
#include "hal_kmd_app_demo.h"
#include "hal_kmd_interface.h"
#include "hal_kmd_foc.h"
#include "bsp_key.h"
#include "bsp_systick.h"

//电机数量，互动模式，需要两个电机
#define DEMO_MOTOR_NUM 4

#define kmd_delay_ms(t_ms) bsp_systick_delay_ms(t_ms)

#define KNOB_BUTTON_NUM (3.0f)                             //整个圆周模拟成3级旋钮,可以适当增大，在此3个作为展示更明显
#define KNOB_BUTTON_ADD ((360.0f) / KNOB_BUTTON_NUM)       //转动增量
#define KNOB_BUTTON_DIFF (KNOB_BUTTON_ADD /2) //判断阈值，

//由于偏移和电机本身等诸多问题，电机正反转力矩对应的电流并不完全一致，但是同一方向力矩值，可以简单用电流表示
#define OBSTACLE_MOMENT (15.0f)              //保持力矩，可以转动
#define OBSTACLE_MOMENT_FRACTION (2.0 / 3.0) //保持力矩

#define CURRENT_FILTER (0.65f) //电流滤波系数

//互控KP值，两个值的比例为力矩差
#define OBSTACLE_MUTUAL_KP1 (30.0f)
#define OBSTACLE_MUTUAL_KP2 (5.0f)

//设置控制相关数据，建议限制范围，随意设置，可能导致异常
#define VEL_LIMIT_MIN 0.5f
#define VEL_LIMIT_MAX 90.0f
#define CUR_LIMIT_MIN 2.0f
#define CUR_LIMIT_MAX 40.0f

typedef enum eKmdButtonEvent
{
    KMD_BT_EVENT_NULL = 0,
    KMD_BT_EVENT_GET_VERSION,      //获取版本号，以固定通信方式
    KMD_BT_EVENT_MOTOR_ENABLE,     //使能电机或失能电机
    KMD_BT_ENVNT_CALI_MOTOR_ENTER, //进行电机校准或停止校准
    KMD_BT_EVENT_GET_CONFIG,       //获取所有参数
    KMD_BT_ENVNT_SAVE_CONFIG,      //保存参数
    KMD_BT_ENVNT_RESET_CONFIG,     //复位参数两个电机参数
} tKmdButtonEvent;

typedef enum eKmdAppEnum
{
    KMD_APP_EVENT_NULL = 0,
    KMD_APP_EVENT_MOTOR_ENABLE,        //使能电机或失能电机
    KMD_APP_POS_KNOB,                  //位置控制方式拨轮旋钮
    KMD_APP_VEL_KNOB,                  //转速方式实现拨轮旋钮
    KMD_APP_OBSTACLE,                  //带阻力的旋钮
    KMD_APP_OBSTACLE_BACK,             //带阻尼回正,就是限制速度而已，
    KMD_APP_SPRING,                    //就是利用P值震荡和速度限制而已，玩玩可以，实际用处不大
    KMD_APP_FREE_MODE,                 //手动旋转，保持转速,自由模式
    KMD_APP_OBSTACLE_MUTUAL,           //双电机同步，力矩放大
    KMD_APP_OBSTACLE_MUTUAL_ONLY,      //单方向控制1,力矩更大
    KMD_APP_OBSTACLE_MUTUAL_TURN_ONLY, //单方向控制2，感觉玩具效果
    KMD_APP_ALL,                       //四个电机互控
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

// 设置返回心跳以获取电机实时信息
static void config_heartbeat_ms(int ms, uint8_t motorNum);
// 设置位置环 kp
static void config_vel_gain(float vel_gain, uint8_t motorNum);
// 设置位置环 ki
static void config_vel_integrator_gain(float vel_integrator_gain, uint8_t motorNum);
// 设置心跳数据内容
static void config_heartbeat_ch(int ms, uint8_t motorNum);
// 配置工作模式
static void config_kmd_work_mode(tHalControlMode work_mode, uint8_t motorNum);
// 设置电流限制
static void config_current_limit(float curLimit, uint8_t motorNum);
// 设置转速限制
static void config_velocity_limit(float velLimit, uint8_t motorNum);
// 查询电机当前模式
static tHalFsmStat kmd_check_motor_state(uint8_t motorNum);
// 检查电机是否已经校准
static bool kmd_check_motor_calib_valid(uint8_t motorNum);
// 查询获取到的配置参数,使用者应当知道参数类型
static config_data get_config_data(tHalUsrConfigsCmd tCmd, uint8_t motorNum);
// 设置电机参数，使用者应当知道其对应的参数类型
static void set_config_data(tHalUsrConfigsCmd tCmd, config_data tData, uint8_t motorNum);
// 此处只是发送获取命令，考虑效率不等待返回，返回值在中断中自动赋值
static void kmd_get_config_info(uint8_t motorNum);

// 电机未使能状态下按键事件
void kmd_motor_menu_button_event(tKmdButtonEvent tKmdEvent);
// 电机使能状态下按键事件
void kmd_motor_mode_button_event(tKmdAppEnum tKmdApp);
// 按键事件
void kmd_button_event(uint16_t tNum);

void kmd_app_run(void);

/**
 * @Description  : kmd foc应用程序入口
 * @return        (*)
 */
void kmd_foc_app_loop(void)
{
    uint16_t buttonEvent;
    buttonEvent = 0;
    if (key_t[0].key_info.click_state_num != 0)
        {
            // 使用按下次数触发事件
            buttonEvent = key_t[0].key_info.click_state_num;
            // 使用后需要清除状态，避免重复调用
            key_t[0].key_info.click_state_num = 0;
            // 按键事件
            kmd_button_event(buttonEvent);
        }
    kmd_app_run();
}

/**
 * @Description  : 按键事件
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
    // 检测是否所有电机使能
    if (check == DEMO_MOTOR_NUM)
        {
            // 使能状态下按键事件
            kmd_motor_mode_button_event(tNum);
        }
    else
        {
            // 未使能状态下按键事件
            kmd_motor_menu_button_event(tNum);
        }
}

/**
 * @Description  : 电机未使能状态下按键事件
 * @param         (tKmdButtonEvent) tKmdEvent
 * @return        (*)
 */
void kmd_motor_menu_button_event(tKmdButtonEvent tKmdEvent)
{
    switch (tKmdEvent)
        {
        case KMD_BT_EVENT_GET_VERSION: //初始化
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
                    // 每3ms 返回一帧心跳数据，开启四个心跳数据帧, 接收完整的数据为 3*4 = 12ms
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
                    // 电机0使能控制
                    if (kmd_check_motor_state(i) != HAL_FSM_MOTOR_MODE)
                        {
                            // 检查是否已经校准
                            if (kmd_check_motor_calib_valid(i))
                                {
                                    // 使能电机0
                                    tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_enable_hook(&mHalFocInfo[i].__tx_frame,
                                            mHalFocInfo[i].node_id);
                                }
                        }
                    else
                        {
                            // 失能电机0
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
                    // 校准电机参数
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
                    // 获取所有参数
                    kmd_get_config_info(i);
                    kmd_delay_ms(2);
                }
        }
        break;
        case KMD_BT_ENVNT_SAVE_CONFIG:
        {
            for (uint8_t i = 0; i < DEMO_MOTOR_NUM; i++)
                {
                    // 保存所有参数
                    tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_configs_updata_hook(
                        &mHalFocInfo[i].__tx_frame, mHalFocInfo[i].node_id);
                    kmd_delay_ms(2);
                }
        }
        break;
        case KMD_BT_ENVNT_RESET_CONFIG:
        {
            // 复位所有参数
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
 * @Description  : 电机使能状态下按键事件
 * @param         (tKmdAppEnum) tKmdApp
 * @return        (*)
 */
void kmd_motor_mode_button_event(tKmdAppEnum tKmdApp)
{
    switch (tKmdApp)
        {
        case KMD_APP_EVENT_MOTOR_ENABLE:
        {
            // 使能电机控制
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
 * @Description  : 使能状态下电机控制
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
        case KMD_APP_POS_KNOB: // 旋钮效果
        {
            float position_move[DEMO_MOTOR_NUM];
            if (tLastKmdRunMode != tKmdRunMode)
                {
                    int heartbeat_ch = 0;
                    // 首次进入，设置模式和参数
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            // 位置轨迹模式
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
                            // 还应当避免重复的增值
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
        case KMD_APP_VEL_KNOB: // P控制,控制速度->旋钮效果
        {
            if (tLastKmdRunMode != tKmdRunMode)
                {
                    // 首次进入，设置模式和参数
                    for (uint8_t kmd_num = 0; kmd_num < DEMO_MOTOR_NUM; kmd_num++)
                        {
                            // 转速模式
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
                            // 此方式不建议太多级旋钮
                            velocity_knob[kmd_num] = sin((realit_position[kmd_num] * 3.14) * (KNOB_BUTTON_NUM * 2));
                            // P控制,此方式容易震荡，使用位置方式实现旋钮将更可靠
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
            // 位置控制方式实现力矩
            if (tLastKmdRunMode != tKmdRunMode)
                {
                    // 首次进入，设置模式和参数
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
                                    // 预留一个小的位置控制量，以保持力矩
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
                                    // 记录 (OBSTACLE_MOMENT_FRACTION) 之一力矩的位置偏差
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
                            // 速度增加
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
                                    // 此处用手转动电机，大概一个转动加速的时间
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
        case KMD_APP_OBSTACLE_MUTUAL: //双电机同步，力矩放大
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
                    // 电机0和电机1互控
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

                    // 电机2和电机3互控
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
        case KMD_APP_OBSTACLE_MUTUAL_ONLY: //单方向控制1,力矩更大
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
        case KMD_APP_OBSTACLE_MUTUAL_TURN_ONLY: //单方向控制2，感觉玩具效果
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
                    position_difference[0] = realit_position[1] - realit_position[0];//0-1互控
                    position_difference[1] = realit_position[2] - realit_position[1];//1-2互控
                    position_difference[2] = realit_position[3] - realit_position[2];//2-3互控
                    position_difference[3] = realit_position[0] - realit_position[3];//3-0互控
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
 * @Description  : 电机使能状态下应用
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
            //电流一般变化较大，适当的滤波参数，以调整平滑度
            //一阶低通滤波
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
    // 检测是否所有电机使能
    if (check >= 1)
        {
            // 使能状态下按键事件
            kmd_motor_mode_run();
        }
}

/*****************************************************************/

/**
 * @Description  : 配置工作模式
 * @param         (tHalControlMode) work_mode
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static void config_kmd_work_mode(tHalControlMode work_mode, uint8_t motorNum)
{
    config_data tData;

    // 此应用中，不包含电流控制模式
    if ((work_mode == HAL_CONTROL_MODE_CURRENT) | (work_mode == HAL_CONTROL_MODE_CURRENT_RAMP))
        {
            return;
        }
    tData.value_int = work_mode;
    // 失能电机
    tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_disable_hook(
        &mHalFocInfo[motorNum].__tx_frame,
        mHalFocInfo[motorNum].node_id);
    kmd_delay_ms(2);

    // 配置电机模式
    set_config_data(HAL_USER_CONFIG_CONTROL_MODE, tData, motorNum);
    kmd_delay_ms(2);

    // 使能电机
    tHalKmdUser.kmd_fsm.hal_kmd_fsm_motor_enable_hook(
        &mHalFocInfo[motorNum].__tx_frame,
        mHalFocInfo[motorNum].node_id);
    kmd_delay_ms(2);
}

/**
 * @Description  : 设置返回心跳以获取电机实时信息
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
 * @Description  : 设置心跳数据内容
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
 * @Description  : 设置电流限制
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
 * @Description  : 设置转速限制
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
 * @Description  : 设置电机参数，使用者应当知道其对应的参数类型
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
 * @Description  : 查询获取到的配置参数,使用者应当知道参数类型
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
 * @Description  : 查询电机当前模式
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static tHalFsmStat kmd_check_motor_state(uint8_t motorNum)
{
    return mHalFocInfo[motorNum].__kmd_ctr.__motor_states.motor_fsm_mode;
}

/**
 * @Description  : 检查电机是否已经校准
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static bool kmd_check_motor_calib_valid(uint8_t motorNum)
{
    return get_config_data(HAL_USER_CONFIG_CALIB_VALID, motorNum).value_int;
}

/**
 * @Description  : 此处只是发送获取命令，考虑效率不等待返回，返回值在中断中自动赋值
 * @param         (uint8_t) motorNum
 * @return        (*)
 */
static void kmd_get_config_info(uint8_t motorNum)
{
    uint16_t config_index = 0;
    //获取 Kmd-Foc版本，上电首次会锁定首次发送获取获取版本号的通信方式，用于控制，直到重新上电
    tHalKmdUser.hal_kmd_get_version_hook(&mHalFocInfo[0].__tx_frame, mHalFocInfo[0].node_id);
    kmd_delay_ms(2);
    //获取参数信息
    for (config_index = 0; config_index < KMD_CONFIG_MAP_MAX_LENGTH; config_index++)
        {
            tHalKmdUser.kmd_config.hal_kmd_user_config_get_hook(
                &mHalFocInfo[motorNum].__tx_frame,
                mHalFocInfo[motorNum].node_id,
                kmd_config_map[config_index][1]);
            kmd_delay_ms(2);
        }
}