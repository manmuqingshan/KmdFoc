#include "hal_foc_struct.h"
#include "hal_kmd_foc.h"
#include "hal_kmd_interface.h"
// extern const uint8_t KMD_CONFIG_MAP_MAX_LENGTH;
// extern const char kmd_config_map[][2];
//  [��������,CONFIG_CMD_ID,name]
const uint8_t KMD_CONFIG_MAP_MAX_LENGTH = 33;
const uint8_t kmd_config_map[][2] =
{
    // Name,��λ,�ؼ�����,CMD����ʾ
    {KMD_TYPE_INT, HAL_USER_CONFIG_MOTOR_POLE_PAIRS},           //"motor_pole_pairs", "Auto","��ż���������У׼�������Զ�������Ŀǰ֧��2~30����������������"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_MOTOR_PHASE_RESISTANCE},   //"motor_phase_resistance", "Auto", "�������������Ȧ����ֵ����У׼�������Զ�����,�������á�"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_MOTOR_PHASE_INDUCTANCE},   //"motor_phase_inductance", "Auto","���������Ȧ����ֵ����У׼�������Զ��������������á�"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_INERTIA},                  //"inertia", "A/(turn/s^2)","inertia", "A/(turn/s^2)","����ת�������������ά����1תÿ��ļ��ٶ�����ʱ����Ҫ�ṩ�ĵ���ֵ(����)����Ҫ���ݵ�����������������������е���,HAL_CONTROL_MODE_VELOCITY_RAMP��HAL_CONTROL_MODE_POSITION_TRAP����ģʽ����Ч��"},
    // Encoder
    {KMD_TYPE_INT, HAL_USER_CONFIG_ENCODER_TYPE},               //"encoder_type", "","���ñ��������ͣ����ı�����֮����븴λ����������Ч���ĸ�λ������ǰ��Ӧ���ȱ������á�"},
    {KMD_TYPE_INT, HAL_USER_CONFIG_ENCODER_PLL_BW},             //"encoder_pll_bandwidth", "Auto","���ñ����� PLL ����һ����ڸ߷ֱ��ʱ����� (> 4000������/ת) ��ֵӦ��Խ�ߣ����������ڼ��ٵ���񶯡�����HALL���ֱַ��ʺܵ͵ı��������������ù��ߵĲ�����"},
    {KMD_TYPE_INT, HAL_USER_CONFIG_ENCODER_CPR},                //"encoder_cpr", "Auto","���ñ������ֱ��ʣ�����ֵ�������������ã�����HALL������������У׼ǰ����Ϊ(6*������)��},
    // Calib
    {KMD_TYPE_INT, HAL_USER_CONFIG_CALIB_VALID},                //"calib_valid", "Auto", "��ǰУ׼�����Ƿ���Ч��У׼��ɺ�����ΪTrue��"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_CALIB_CURRENT},            //"calib_current", "A","����У׼ʱ�ĵ���ֵ������̫СУ׼ʱ�޷�˳��ת�������ù�������ܵ��µ���������ء�"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_CALIB_MAX_VOLTAGE},        //"calib_max_voltage", "V","����У׼ʱ������ѹ��"},
    // Control
    {KMD_TYPE_INT, HAL_USER_CONFIG_CONTROL_MODE},               //"control_mode", "","���õ������ģʽ,һ��������ģʽ��"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_CURRENT_RAMP_RATE},        //"current_ramp_rate", "A/sec", "���õ�����������ģʽ�µ��������ٶȣ��������ģʽΪCURRENT_RAMPʱ��Ч��"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_VEL_RAMP_RATE},            //"ramp_rate", "(turn/s)/s", "����ת������ģʽ��ת�������ٶȣ��������ģʽΪVELOCITY_RAMPʱ��Ч��"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_TRAJ_VEL},                 //"traj_vel", "turn/s", "�������ι켣����ģʽ�����ת�٣��������ģʽΪPOSITION_TRAPʱ��Ч��"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_TRAJ_ACCEL},               //"traj_accel", "(turn/s)/s", "�������ι켣����ģʽ�¼��ٶȣ��������ģʽΪPOSITION_TRAPʱ��Ч��"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_TRAJ_DECEL},               //"traj_decel", "(turn/s)/s", "�������ι켣����ģʽ�¼��ٶȣ��������ģʽΪPOSITION_TRAPʱ��Ч��"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_POS_GAIN},                 //"pos_gain", "(turn/s)/turn","����λ�û����档"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_VEL_GAIN},                 //"vel_gain", "A/(turn/s)", "����ת�ٻ����档"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_VEL_INTEGRATOR_GAIN},      //"vel_integrator_gain", "A/((turn/s)*s)", "����ת�ٻ��������档"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_VEL_LIMIT},                //"vel_limit", "turn/s","����ת�ٻ����ơ�"},
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_CURRENT_LIMIT},            //"current_limit", "A","���õ��������ơ�"},                                                       
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_CURRENT_CTRL_P_GAIN},      //"current_ctrl_p_gain", "Auto","���õ��������棬��У׼�������Զ�����õ���Ҳ���������á�"}, 
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_CURRENT_CTRL_I_GAIN},      //"current_ctrl_i_gain", "Auto", "���õ������������棬��У׼�������Զ�����õ���Ҳ���������á�"}, 
    {KMD_TYPE_INT, HAL_USER_CONFIG_CURRENT_CTRL_BW},            //"current_ctrl_bandwidth", "rad/s","���õ������˲��������´���ʱҲ��ͬʱ���µ���������͵������������档[100~2000]"},
    // Protect
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_PROTECT_UNDER_VOLTAGE},    //"protect_under_voltage", "V", "���õ�ѹ������ֵ��"}, 
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_PROTECT_OVER_VOLTAGE},     //"protect_over_voltage", "V", "���ù�ѹ������ֵ��""},   
    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_PROTECT_OVER_CURRENT},     //"protect_over_current", "A", "���ù���������ֵ��"},   

    {KMD_TYPE_FLOAT, HAL_USER_CONFIG_PROTECT_OVER_SPEED},       //"protect_over_speed", "turn/s","���õ�����ٱ�����ֵ��"}, 
    // CAN
    {KMD_TYPE_INT, HAL_USER_CONFIG_CAN_ID},                     //"can_id", "","�����豸ID����Χ1~16��"}, 
    {KMD_TYPE_INT, HAL_USER_CONFIG_CAN_TIMEOUT_MS},             //"can_timeout_ms", "","����ͨ�ų�ʱʱ�䣬������ʱ��δ����ͨ�ţ������ֹͣ���У�����Ϊ0�����á�������ã�����ֵ���������500ms��Ĭ��0��"}, 
    {KMD_TYPE_INT, HAL_USER_CONFIG_CAN_HEARTBEAT_MS},           //"can_report_heart_ms", "","�����豸�������豸���������ϱ�״̬��λ�ã�����Ϊ0�����á�������ã�����ֵ���������2ms���ر�Ĵ��ڷ�ʽ��Ҫ����10ms,Ĭ��0��"},     
    {KMD_TYPE_INT, HAL_USER_CONFIG_CAN_HEARTBEAT_CH},           //"cen_report_heart_choose", "","ѡ��can���������Ը��µ����ݣ�Ĭ��[(1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)]��"},      
    
    // Debug
    {KMD_TYPE_INT, HAL_USER_CONFIG_UART_DEBUG_MS},              //"uart_debug_ms", "",  "���ô��ڿ������ԣ����趨���ڴ�ӡ���ݲ��Ρ�����Ϊ0�����á�������ã�����ֵ���������15ms,Ĭ��0��"} 
};

//(((uint16_t)pFocFsm->state) << 16) | (pFocFsm->motor_err & 0XFFFF);
static void parse_motor_state(uint16_t *pFsm, uint16_t *pErr, uint32_t tData)
{
    *pFsm = tData >> 16;
    *pErr = tData & 0XFFFF;
}

/**
 * @Description  : ������������
 * @param         (tHalFocUserConfig) *pKmdUserConfigInfo
 * @return        (*)
 */
bool hal_kmd_user_config_receive_callback(hal_frame_struct *pFrame, tHalFocUserConfig *pKmdUserConfigInfo)
{
    uint32_t kmdUserCmdId = 0;
    uint32_t kmdUserResult = 0;
    uint32_t kmdUserTxOeder;
    kmdUserTxOeder = data_to_uint32(&pFrame->data[0]);
    hal_parse_order_id(&kmdUserCmdId, &kmdUserResult, kmdUserTxOeder);
    uint16_t config_index;
    for (config_index = 0; config_index < KMD_CONFIG_MAP_MAX_LENGTH; config_index++)
        {
            if (kmd_config_map[config_index][1] == kmdUserCmdId)
                {
                    switch (kmd_config_map[config_index][0])
                        {
                        case KMD_TYPE_FLOAT:
                        {
                            pKmdUserConfigInfo->__config[config_index].value_float = data_to_float(&pFrame->data[4]);
                        }
                        break;
                        case KMD_TYPE_INT:
                        {
                            pKmdUserConfigInfo->__config[config_index].value_int = data_to_int(&pFrame->data[4]);
                        }
                        break;
                        }
                }
        }
}

/**
 * @Description  : HAL_KMD_MOTOR_CTR ��Ӧ��FOC״̬���Ʒ���
 * @param         (tHalFocControl) *pKmdCtr
 * @return        (*)
 */
bool hal_kmd_motor_receive_callback(hal_frame_struct *pFrame, tHalFocControl *pKmdCtr)
{
    uint32_t kmdUserCmdId = 0;
    uint32_t kmdUserResult = 0;
    uint32_t kmdUserTxOeder;
    kmdUserTxOeder = data_to_uint32(&pFrame->data[0]);
    hal_parse_order_id(&kmdUserCmdId, &kmdUserResult, kmdUserTxOeder);

    switch (kmdUserCmdId)
        {

        case HAL_MOTOR_DISABLE: //ʧ�ܵ��              pData[1] = ��ǰλ��
        {
            if (kmdUserResult == HAL_RESULT_OK)
                {
                    pKmdCtr->__motor_states.motor_fsm_mode = HAL_FSM_MENU_MODE;
                }
        }
        break;
HAL_MOTOR_ENABLE: //ʹ�ܵ��              pData[1] = ��ǰλ��
        {
            if (kmdUserResult == HAL_RESULT_OK)
                {
                    pKmdCtr->__closed_loop.target.position = data_to_float(&pFrame->data[4]);
                    pKmdCtr->__closed_loop.target.velocity = 0;
                    pKmdCtr->__closed_loop.target.current_ = 0;
                    pKmdCtr->__motor_states.motor_fsm_mode = HAL_FSM_MOTOR_MODE;
                }
        }
        break;
HAL_MOTOR_ERROR_REPORT: //�������              pData[1] = ������Ϣ���Զ��ϱ�����ȡ��Ч
        {
            if (kmdUserResult == HAL_RESULT_OK)
                {
                    parse_motor_state(
                        &pKmdCtr->__motor_states.motor_fsm_mode,
                        &pKmdCtr->__motor_states.motor_error,
                        data_to_uint32(&pFrame->data[4]));
                }
        }
        break;
HAL_MOTOR_ERROR_RESET: //��λ����              pData[1] = 0
        {
            if (kmdUserResult == HAL_RESULT_OK)
                {
                    pKmdCtr->__motor_states.motor_error = 0;
                }
        }
        break;
HAL_MOTOR_GET_STAT: //��ȡ���״̬          pData[1] = ����״̬
        {
            if (kmdUserResult == HAL_RESULT_OK)
                {
                    parse_motor_state(
                        &pKmdCtr->__motor_states.motor_fsm_mode,
                        &pKmdCtr->__motor_states.motor_error,
                        data_to_uint32(&pFrame->data[4]));
                }
        }
        break;
HAL_MOTOR_CALIBRATION_START: //��ʼУ׼              pData[1] = 0
        {
            if (kmdUserResult == HAL_RESULT_OK)
                {
                    pKmdCtr->__motor_states.motor_fsm_mode = HAL_FSM_CALIBRATION_MODE;
                }
        }
        break;
HAL_MOTOR_CALIBRATION_ABORT: //��λУ׼              pData[1] = 0
        {
            if (kmdUserResult == HAL_RESULT_OK)
                {
                    pKmdCtr->__motor_states.motor_fsm_mode = HAL_FSM_MENU_MODE;
                }
        }
        break;
HAL_MOTOR_CONFIGS_UPDATA: //�������             pData[1] = 0
        {
            if (kmdUserResult == HAL_RESULT_OK)
                {
                    //����ɹ�
                }
        }
        break;
HAL_MOTOR_CONFIGS_RESET_ALL: //��λ���в���         pData[1] = 0
        {
            if (kmdUserResult == HAL_RESULT_OK)
                {
                    //��λ�ɹ�
                    pKmdCtr->__motor_states.motor_fsm_mode = HAL_FSM_MENU_MODE;
                }
        }
        break;
        default:
            ;
            break;
        }
    return true;
}

/**
 * @Description  : FOCУ׼�������ֵ
 * @return        (*)
 */
bool hal_kmd_cali_receive_callback(hal_frame_struct *pFrame, tHalFocControl *pKmdCtr)
{
    uint32_t OrderCmdId = 0;
    uint32_t OrderResult = 0;
    hal_parse_order_id(&OrderCmdId, &OrderResult, data_to_uint32(&pFrame->data[0]));

    switch (OrderCmdId)
        {
        case HAL_CALIBRATION_ERROR:
        {
            pKmdCtr->__cali_info.cali_error = data_to_uint32(&pFrame->data[4]);
        }
        break;
        case HAL_CALIBRATION_RES:
        {
            pKmdCtr->__cali_info.cali_res = data_to_float(&pFrame->data[4]);
        }
        break;
        case HAL_CALIBRATION_IND:
        {
            pKmdCtr->__cali_info.cali_ind = data_to_float(&pFrame->data[4]);
        }
        break;
        case HAL_CALIBRATION_PAIRS:
        {
            pKmdCtr->__cali_info.cali_pairs = data_to_int(&pFrame->data[4]);
        }
        break;
        case HAL_CALIBRATION_DIR:
        {
            pKmdCtr->__cali_info.cali_dir = data_to_int(&pFrame->data[4]);
        }
        break;
        case HAL_CALIBRATION_OFFSET:
        {
            pKmdCtr->__cali_info.cali_offset = data_to_int(&pFrame->data[4]);
        }
        break;
        case HAL_CALIBRATION_POSITION:
        {
            pKmdCtr->__cali_info.cali_position = data_to_float(&pFrame->data[4]);
        }
        break;
        case HAL_CALIBRATION_END:
        {
            pKmdCtr->__cali_info.cali_error = data_to_uint32(&pFrame->data[4]);
        }
        break;
        default:
            break;
        }
    if (OrderCmdId > HAL_CALIBRATION_OFFSET_LUT)
        {
            pKmdCtr->__cali_info.__offset_lut[OrderCmdId - HAL_CALIBRATION_OFFSET_LUT].value_int = data_to_int(&pFrame->data[4]);
        }
    return true;
}

bool hal_kmd_frame_receive_callback(hal_frame_struct *pFrame, tHalFocInfo *pKmdInfo)
{
    uint32_t node_id;
    uint32_t cmd_id;

    hal_parse_frame_id(&node_id, &cmd_id, pFrame->can_id);

    if (node_id != pKmdInfo->node_id)
        {
            return false;
        }
		
    switch (cmd_id)
        {
        case HAL_KMD_MOTOR_DISABLE: //ʧ�ܵ��
        {
        }
        break;
        case HAL_KMD_MOTOR_CTR: //�������״̬������	    ->����ֵ [(result<<16)|(tKmdMotorCmd),float]
        {
            hal_kmd_motor_receive_callback(pFrame, &pKmdInfo->__kmd_ctr);
        }
        break;
        case HAL_KMD_CALIBRATION_REPORT: //����У׼
        {
            hal_kmd_cali_receive_callback(pFrame, &pKmdInfo->__kmd_ctr);
        }
        break;
        case HAL_KMD_SET_TARGET_POSITION: //����Ŀ��λ��			->����ֵ [float,0]
        {
            pKmdInfo->__kmd_ctr.__closed_loop.target.position = data_to_float(&pFrame->data[0]);
        }
        break;
        case HAL_KMD_SET_TARGET_VELOCITY: //����Ŀ��ת��			->����ֵ [float,0]
        {
            pKmdInfo->__kmd_ctr.__closed_loop.target.velocity = data_to_float(&pFrame->data[0]);
        }
        break;
        case HAL_KMD_SET_TARGET_CURRENT: //����Ŀ�����			->����ֵ [float,0]
        {
            pKmdInfo->__kmd_ctr.__closed_loop.target.current_ = data_to_float(&pFrame->data[0]);
        }
        break;
        case HAL_KMD_GET_POSITION: //Ŀ��λ�ã�ʵ��λ��	->����ֵ [float,float]
        {
            pKmdInfo->__kmd_ctr.__closed_loop.target.position = data_to_float(&pFrame->data[0]);
            pKmdInfo->__kmd_ctr.__closed_loop.realit.position = data_to_float(&pFrame->data[4]);
        }
        break;
        case HAL_KMD_GET_VELOCITY: //Ŀ��ת�٣�ʵ��ת��	->����ֵ [float,float]
        {
            pKmdInfo->__kmd_ctr.__closed_loop.target.velocity = data_to_float(&pFrame->data[0]);
            pKmdInfo->__kmd_ctr.__closed_loop.realit.velocity = data_to_float(&pFrame->data[4]);
        }
        break;
        case HAL_KMD_GET_CURRENT: //Ŀ�������ʵ�ʵ���	->����ֵ [float,float]
        {
            pKmdInfo->__kmd_ctr.__closed_loop.target.current_ = data_to_float(&pFrame->data[0]);
            pKmdInfo->__kmd_ctr.__closed_loop.realit.current_ = data_to_float(&pFrame->data[4]);
        }
        break;
        case HAL_KMD_GET_FOCBUS: //���ߵ�ѹ��ʵ�ʵ�ѹ	->����ֵ [float,float]
        {
            pKmdInfo->__kmd_ctr.__bus_info.v_bus_filt = data_to_float(&pFrame->data[0]);
            pKmdInfo->__kmd_ctr.__bus_info.i_bus_filt = data_to_float(&pFrame->data[4]);
        }
        break;
        case HAL_KMD_SET_CONFIG: //���ò���				->����ֵ [(result<<16)|(tHalUsrConfigsCmd),ʵ��ֵ(int/float)]
        {
            hal_kmd_user_config_receive_callback(pFrame, &pKmdInfo->__kmd_user);
        }
        break;
        case HAL_KMD_GET_CONFIG: //��ȡ����				->����ֵ [(result<<16)|(tHalUsrConfigsCmd),ʵ��ֵ(int/float)]
        {
            hal_kmd_user_config_receive_callback(pFrame, &pKmdInfo->__kmd_user);
        }
        break;
        case HAL_KMD_GET_FW_VERSION: //��ȡ����汾��
        {
            pKmdInfo->__kmd_user.__version[0] = pFrame->data[0];
            pKmdInfo->__kmd_user.__version[1] = pFrame->data[1];
            pKmdInfo->__kmd_user.__version[2] = pFrame->data[2];
            pKmdInfo->__kmd_user.__version[3] = pFrame->data[3];
        }
        break;
        //�������������ú��������ں��������ݺ󣬻��������Ƶ���Զ��������ݣ��Լ��ٿ���ʱ�������������ͨ�Ÿ���
        case HAL_KMD_FSM_HEARTBEAT0: //����0
        {
            //���µ��״̬��λ��
            parse_motor_state(
                &pKmdInfo->__kmd_ctr.__motor_states.motor_fsm_mode,
                &pKmdInfo->__kmd_ctr.__motor_states.motor_error,
                data_to_uint32(&pFrame->data[4]));
            pKmdInfo->__kmd_ctr.__closed_loop.realit.position = data_to_float(&pFrame->data[4]);
        }
        break;
        case HAL_KMD_POS_HEARTBEAT1: //����1
        {
            pKmdInfo->__kmd_ctr.__closed_loop.target.position = data_to_float(&pFrame->data[0]);
            pKmdInfo->__kmd_ctr.__closed_loop.realit.position = data_to_float(&pFrame->data[4]);
        }
        break;
        case HAL_KMD_VEL_HEARTBEAT2: //����2
        {
            pKmdInfo->__kmd_ctr.__closed_loop.target.velocity = data_to_float(&pFrame->data[0]);
            pKmdInfo->__kmd_ctr.__closed_loop.realit.velocity = data_to_float(&pFrame->data[4]);
        }
        break;
        case HAL_KMD_CUR_HEARTBEAT3: //����3
        {
            //���µ��״̬��λ��
            pKmdInfo->__kmd_ctr.__closed_loop.target.current_ = data_to_float(&pFrame->data[0]);
            pKmdInfo->__kmd_ctr.__closed_loop.realit.current_ = data_to_float(&pFrame->data[4]);
        }
        break;
        case HAL_KMD_BUS_HEARTBEAT4: //����4
        {
            //���µ��״̬��λ��
            pKmdInfo->__kmd_ctr.__bus_info.v_bus_filt = data_to_float(&pFrame->data[0]);
            pKmdInfo->__kmd_ctr.__bus_info.i_bus_filt = data_to_float(&pFrame->data[4]);
        }
        break;
        default:
            break;
        }
    return true;
}


/** 
 * @Description  : ʧ�ܵ��
 * @param         (uint16_t) node_id
 * @return        (*)
 */
bool hal_kmd_disable_motor(hal_frame_struct *pFrame, uint16_t node_id)
{
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_MOTOR_DISABLE, &pFrame->can_id);
    //ֻ��Ҫ����
    memset(pFrame->data, 0, 8);
    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_KMD_SET_TARGET_POSITION,//����Ŀ��λ��			->��Ϣ����ֵ [float,0]  ���ҽ���λ�ÿ���ģʽ��Ч
 * @param         (uint16_t) node_id
 * @param         (float) tData
 * @return        (*)
 */
bool hal_kmd_set_pisotion(hal_frame_struct *pFrame, uint16_t node_id, float tData)
{
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_SET_TARGET_POSITION, &pFrame->can_id);
    float_to_data(tData, pFrame->data);
    memset(&pFrame->data[4], 0, 4);
    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_KMD_SET_TARGET_VELOCITY,//����Ŀ��ת��			->��Ϣ����ֵ [float,0] ���ҽ���ת�ٿ���ģʽ��Ч
 * @param         (uint16_t) node_id
 * @param         (float) tData
 * @return        (*)
 */
bool hal_kmd_set_velocity(hal_frame_struct *pFrame, uint16_t node_id, float tData)
{
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_SET_TARGET_VELOCITY, &pFrame->can_id);
    float_to_data(tData, pFrame->data);
    memset(&pFrame->data[4], 0, 4);
    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_KMD_SET_TARGET_CURRENT, //����Ŀ�����			->��Ϣ����ֵ [float,0] ���ҽ�����������ģʽ��Ч
 * @param         (uint16_t) node_id
 * @param         (float) tData
 * @return        (*)
 */
bool hal_kmd_set_current(hal_frame_struct *pFrame, uint16_t node_id, float tData)
{
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_SET_TARGET_CURRENT, &pFrame->can_id);
    float_to_data(tData, pFrame->data);
    memset(&pFrame->data[4], 0, 4);
    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_KMD_GET_POSITION,		//Ŀ��λ�ã�ʵ��λ����Ϣ����	->��Ϣ����ֵ [float,float]
 * @param         (uint16_t) node_id
 * @return        (*)
 */
bool hal_kmd_get_position(hal_frame_struct *pFrame, uint16_t node_id)
{
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_GET_POSITION, &pFrame->can_id);
    //ֻ��Ҫ����
    memset(pFrame->data, 0, 8);

    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_KMD_GET_VELOCITY,		//Ŀ��ת�٣�ʵ��ת����Ϣ����	->��Ϣ����ֵ [float,float]
 * @param         (uint16_t) node_id
 * @return        (*)
 */
bool hal_kmd_get_velocity(hal_frame_struct *pFrame, uint16_t node_id)
{
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_GET_VELOCITY, &pFrame->can_id);
    //ֻ��Ҫ����
    memset(pFrame->data, 0, 8);

    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  :  HAL_KMD_GET_CURRENT,		//Ŀ�������ʵ�ʵ�����Ϣ����	->��Ϣ����ֵ [float,float]
 * @param         (uint16_t) node_id
 * @return        (*)
 */
bool hal_kmd_get_current(hal_frame_struct *pFrame, uint16_t node_id)
{
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_GET_CURRENT, &pFrame->can_id);
    //ֻ��Ҫ����
    memset(pFrame->data, 0, 8);
    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_KMD_GET_FOCBUS,			//���ߵ�ѹ��ʵ�ʵ�ѹ��Ϣ����	->��Ϣ����ֵ [float,float]
 * @param         (uint16_t) node_id
 * @return        (*)
 */
bool hal_kmd_get_bus(hal_frame_struct *pFrame, uint16_t node_id)
{
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_GET_FOCBUS, &pFrame->can_id);
    //ֻ��Ҫ����
    memset(pFrame->data, 0, 8);

    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_KMD_GET_FW_VERSION,		//��ȡ����汾����Ϣ����
 * @param         (uint16_t) node_id
 * @return        (*)
 */
bool hal_kmd_get_version(hal_frame_struct *pFrame, uint16_t node_id)
{
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_GET_FW_VERSION, &pFrame->can_id);
    //ֻ��Ҫ����
    memset(pFrame->data, 0, 8);

    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_MOTOR_DISABLE = 0,          //ʧ�ܵ��		pData[1] = ��ǰλ��
 * @param         (uint16_t) node_id
 * @return        (*)
 */
static bool hal_kmd_fsm_motor_disable(hal_frame_struct *pFrame, uint16_t node_id)
{

    uint32_t tOrderId;
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_MOTOR_CTR, &pFrame->can_id);
    hal_make_order_id((uint32_t)HAL_MOTOR_DISABLE, 0, &tOrderId);
    uint32_to_data(tOrderId, &pFrame->data[0]);
    memset(&pFrame->data[4], 0, 4);
    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_MOTOR_ENABLE,               //ʹ�ܵ��		pData[1] = ��ǰλ��
 * @param         (uint16_t) node_id
 * @return        (*)
 */
static bool hal_kmd_fsm_motor_enable(hal_frame_struct *pFrame, uint16_t node_id)
{
    uint32_t tOrderId;
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_MOTOR_CTR, &pFrame->can_id);
    hal_make_order_id((uint32_t)HAL_MOTOR_ENABLE, 0, &tOrderId);
    uint32_to_data(tOrderId, &pFrame->data[0]);
    memset(&pFrame->data[4], 0, 4);
    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_MOTOR_ERROR_REPORT,         //�������
 * @param         (uint16_t) node_id
 * @return        (*)
 */
bool hal_kmd_fsm_motor_report_error(hal_frame_struct *pFrame,uint16_t node_id)
{
	uint32_t tOrderId;
	hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_MOTOR_CTR, &pFrame->can_id);
	hal_make_order_id((uint32_t)HAL_MOTOR_ERROR_REPORT,0,&tOrderId);
	uint32_to_data(tOrderId,&pFrame->data[0]);
	memset(&pFrame->data[4], 0, 4);
	return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_MOTOR_ERROR_RESET,          //��λ����
 * @param         (uint16_t) node_id
 * @return        (*)
 */
static bool hal_kmd_fsm_motor_reset_error(hal_frame_struct *pFrame, uint16_t node_id)
{
    uint32_t tOrderId;
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_MOTOR_CTR, &pFrame->can_id);
    hal_make_order_id((uint32_t)HAL_MOTOR_ERROR_RESET, 0, &tOrderId);
    uint32_to_data(tOrderId, &pFrame->data[0]);
    memset(&pFrame->data[4], 0, 4);
    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_MOTOR_GET_STAT,             //��ȡ���״̬��Ϣ����
 * @param         (uint16_t) node_id
 * @return        (*)
 */
static bool hal_kmd_fsm_motor_get_state(hal_frame_struct *pFrame, uint16_t node_id)
{
    uint32_t tOrderId;
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_MOTOR_CTR, &pFrame->can_id);
    hal_make_order_id((uint32_t)HAL_MOTOR_GET_STAT, 0, &tOrderId);
    uint32_to_data(tOrderId, &pFrame->data[0]);
    memset(&pFrame->data[4], 0, 4);
    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_MOTOR_CALIBRATION_START,    //��ʼУ׼
 * @param         (uint16_t) node_id
 * @return        (*)
 */
static bool hal_kmd_fsm_motor_cali_start(hal_frame_struct *pFrame, uint16_t node_id)
{
    uint32_t tOrderId;
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_MOTOR_CTR, &pFrame->can_id);
    hal_make_order_id((uint32_t)HAL_MOTOR_CALIBRATION_START, 0, &tOrderId);
    uint32_to_data(tOrderId, &pFrame->data[0]);
    memset(&pFrame->data[4], 0, 4);
    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_MOTOR_CALIBRATION_ABORT,    //ֹͣУ׼
 * @param         (uint16_t) node_id
 * @return        (*)
 */
static bool hal_kmd_fsm_motor_cali_stop(hal_frame_struct *pFrame, uint16_t node_id)
{
    uint32_t tOrderId;
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_MOTOR_CTR, &pFrame->can_id);
    hal_make_order_id((uint32_t)HAL_MOTOR_CALIBRATION_ABORT, 0, &tOrderId);
    uint32_to_data(tOrderId, &pFrame->data[0]);
    memset(&pFrame->data[4], 0, 4);
    return kmd_interface_transmit(pFrame);
}


/** 
 * @Description  : HAL_MOTOR_CONFIGS_UPDATA,        //�������
 * @param         (uint16_t) node_id
 * @return        (*)
 */
static bool hal_kmd_fsm_motor_configs_updata(hal_frame_struct *pFrame, uint16_t node_id)
{
    uint32_t tOrderId;
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_MOTOR_CTR, &pFrame->can_id);
    hal_make_order_id((uint32_t)HAL_MOTOR_CONFIGS_UPDATA, 0, &tOrderId);
    uint32_to_data(tOrderId, &pFrame->data[0]);
    memset(&pFrame->data[4], 0, 4);
    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : HAL_MOTOR_CONFIGS_RESET_ALL,	 //��λ���в���
 * @param         (uint16_t) node_id
 * @return        (*)
 */
static bool hal_kmd_fsm_motor_configs_reset(hal_frame_struct *pFrame, uint16_t node_id)
{
    uint32_t tOrderId;
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_MOTOR_CTR, &pFrame->can_id);
    hal_make_order_id((uint32_t)HAL_MOTOR_CONFIGS_RESET_ALL, 0, &tOrderId);
    uint32_to_data(tOrderId, &pFrame->data[0]);
    memset(&pFrame->data[4], 0, 4);
    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : ���ò���,ʹ��ǰ��ذ�װ��ȷ�����ݸ�ʽ���в�������
 * @param         (uint16_t) node_id
 * @param         (uint16_t) tOrderCmd
 * @param         (uint8_t) *pData
 * @return        (*)
 */
static bool hal_kmd_user_config_set(hal_frame_struct *pFrame, uint16_t node_id, uint16_t tOrderCmd, uint8_t *pData)
{
    uint32_t tOrderId;
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_SET_CONFIG, &pFrame->can_id);
    hal_make_order_id((uint32_t)tOrderCmd, 0, &tOrderId);
    uint32_to_data(tOrderId, &pFrame->data[0]);
    memcpy(&pFrame->data[4], pData, 4);
    return kmd_interface_transmit(pFrame);
}

/** 
 * @Description  : ��ȡ������Ϣ����
 * @param         (uint16_t) node_id
 * @param         (uint16_t) tOrderCmd
 * @return        (*)
 */
static bool hal_kmd_user_config_get(hal_frame_struct *pFrame, uint16_t node_id, uint16_t tOrderCmd)
{
    uint32_t tOrderId;
    hal_make_frame_id((uint32_t)node_id, (uint32_t)HAL_KMD_GET_CONFIG, &pFrame->can_id);
    hal_make_order_id((uint32_t)tOrderCmd, 0, &tOrderId);
    uint32_to_data(tOrderId, &pFrame->data[0]);
    memset(&pFrame->data[4], 0, 4);
    return kmd_interface_transmit(pFrame);
}

hal_kmd_user_struct tHalKmdUser =
    {
        .hal_kmd_disable_motor_hook = &hal_kmd_disable_motor,//ʧ�ܵ��
        .hal_kmd_set_pisotion_hook = &hal_kmd_set_pisotion,//����Ŀ��λ��
        .hal_kmd_set_velocity_hook = &hal_kmd_set_velocity,//����Ŀ��ת��
        .hal_kmd_set_current_hook = &hal_kmd_set_current,//����Ŀ�����
        .hal_kmd_get_position_hook = &hal_kmd_get_position,//Ŀ��λ�ã�ʵ��λ����Ϣ����
        .hal_kmd_get_velocity_hook = &hal_kmd_get_velocity,//Ŀ��ת�٣�ʵ��ת����Ϣ����
        .hal_kmd_get_current_hook = &hal_kmd_get_current,//Ŀ�������ʵ�ʵ�����Ϣ����
        .hal_kmd_get_bus_hook = &hal_kmd_get_bus,//���ߵ�ѹ��ʵ�ʵ�ѹ��Ϣ����
        .hal_kmd_get_version_hook = &hal_kmd_get_version,//��ȡ����汾����Ϣ����
        .kmd_fsm.hal_kmd_fsm_motor_disable_hook = &hal_kmd_fsm_motor_disable,//ʧ�ܵ��
        .kmd_fsm.hal_kmd_fsm_motor_enable_hook = &hal_kmd_fsm_motor_enable,//ʹ�ܵ��
        .kmd_fsm.hal_kmd_fsm_motor_reset_error_hook = &hal_kmd_fsm_motor_reset_error,//��λ����
        .kmd_fsm.hal_kmd_fsm_motor_get_state_hook = &hal_kmd_fsm_motor_get_state,//��ȡ���״̬��Ϣ����
        .kmd_fsm.hal_kmd_fsm_motor_cali_start_hook = &hal_kmd_fsm_motor_cali_start,//��ʼУ׼
        .kmd_fsm.hal_kmd_fsm_motor_cali_stop_hook = &hal_kmd_fsm_motor_cali_stop,//ֹͣУ׼
        .kmd_fsm.hal_kmd_fsm_motor_configs_updata_hook = &hal_kmd_fsm_motor_configs_updata,//�������
        .kmd_fsm.hal_kmd_fsm_motor_configs_reset_hook = &hal_kmd_fsm_motor_configs_reset,//��λ���в���
        .kmd_config.hal_kmd_user_config_set_hook = &hal_kmd_user_config_set,//���ò���,ʹ��ǰ��ذ�װ��ȷ�����ݸ�ʽ���в�������
        .kmd_config.hal_kmd_user_config_get_hook = &hal_kmd_user_config_get};//��ȡ������Ϣ����
