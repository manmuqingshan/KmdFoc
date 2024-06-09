#ifndef _HAL_FOC_ENUM_H_
#define _HAL_FOC_ENUM_H_

typedef enum eHalCalibrationReport
{
	HAL_CALIBRATION_ERROR = 0,
	HAL_CALIBRATION_RES = 1,        //����
	HAL_CALIBRATION_IND = 2,        //���
	HAL_CALIBRATION_PAIRS = 3,      //������
	HAL_CALIBRATION_DIR = 4,        //����
	HAL_CALIBRATION_OFFSET = 5,     //ƫ��
	HAL_CALIBRATION_POSITION = 6,   //���λ��
	HAL_CALIBRATION_END = 7,		//У׼����

	HAL_CALIBRATION_OFFSET_LUT = 10,//У׼����ͷ
}tHalCalibrationReport;

typedef enum eCalibrateStep
{	
	HAL_CS_NULL = 0,
	
	HAL_CS_MOTOT_CALI_READY_START,
	HAL_CS_MOTOT_CALI_READY_RESET,
	HAL_CS_MOTOT_CALI_READY_END,

	HAL_CS_MOTOR_R_START,
	HAL_CS_MOTOR_R_LOOP,
	HAL_CS_MOTOR_R_END,
	
	HAL_CS_MOTOR_L_START,
	HAL_CS_MOTOR_L_LOOP,
	HAL_CS_MOTOR_L_END,
	
	HAL_CS_ENCODER_DIR_PP_START,
	HAL_CS_ENCODER_DIR_PP_LOCK,
	HAL_CS_ENCODER_DIR_PP_LOOP,
	HAL_CS_ENCODER_DIR_PP_END,
	
	HAL_CS_ENCODER_OFFSET_START,
	HAL_CS_ENCODER_OFFSET_LOCK,
	HAL_CS_ENCODER_OFFSET_CW_LOOP,
	HAL_CS_ENCODER_OFFSET_CCW_LOOP,
	HAL_CS_ENCODER_OFFSET_END,
	
	HAL_CS_ERROR,
}tHalCalibrationStep;

typedef enum eCalibrationError{
	HAL_CE_NULL = 0,
	HAL_CE_PHASE_RESISTANCE_OUT_OF_RANGE,
	HAL_CE_MOTOR_POLE_PAIRS_OUT_OF_RANGE
}tHalCalibrationError;

typedef enum eHalFocFsmCmd
{
	HAL_CMD_FSM_MENU = 0,
	HAL_CMD_FSM_MOTOR,
	HAL_CMD_FSM_MOTOR_ADD,
	HAL_CMD_FSM_MOTOR_DEL,
	HAL_CMD_FSM_CALIBRATION,
	HAL_CMD_FSM_UPDATE_CONFIGS,
	HAL_CMD_FSM_RESET_ERROR,
	HAL_CMD_FSM_UART_SETUP
}tHalFocFsmCmd;

typedef enum eHalFsmStat {
	HAL_FSM_STARTUP = 0,
	HAL_FSM_MENU_MODE,
	HAL_FSM_MOTOR_MODE,
	HAL_FSM_CALIBRATION_MODE,

	HAL_FSM_UART_SETUP,
} tHalFsmStat;

typedef enum eHalMotorError
{
	MOTOR_ERR_OVER_VOLTAGE = 0,
	MOTOR_ERR_UNDER_VOLTAGE = 1,
	MOTOR_ERR_OVER_SPEED = 2,
	MOTOR_ERR_OVER_CURRENT = 3,
    MOTOR_ERR_ENCODER = 4,
	MOTOR_ERR_CAN_CONFLICT = 5,
    
    MOTOR_ERR_HANLDER = 15,
	MOTOR_ERR_NUM = 16,//��λ��Ч
}tHalMotorError;

//128
typedef enum eHalCanKmdCmd
{
    HAL_KMD_MOTOR_DISABLE  = 0,//ʧ�ܵ��
    HAL_KMD_MOTOR_CTR,     	   //�������״̬������	    ->����ֵ [(result<<16)|(tHalMotorCmd),float]

    HAL_KMD_CALIBRATION_REPORT,	//����У׼

    HAL_KMD_READY_SET_POSITION,//����Ŀ��λ��			->����ֵ [float,0]
    HAL_KMD_READY_SET_VELOCITY,//����Ŀ��ת��			->����ֵ [float,0]
    HAL_KMD_READY_SET_CURRENT, //����Ŀ�����			->����ֵ [float,0]

    HAL_KMD_GET_POSITION,		//Ŀ��λ�ã�ʵ��λ��	->����ֵ [float,float]
    HAL_KMD_GET_VELOCITY,		//Ŀ��ת�٣�ʵ��ת��	->����ֵ [float,float]
    HAL_KMD_GET_CURRENT,		//Ŀ�������ʵ�ʵ���	->����ֵ [float,float]
    HAL_KMD_GET_FOCBUS,			//���ߵ�ѹ��ʵ�ʵ�ѹ	->����ֵ [float,float]

    HAL_KMD_SET_CONFIG,			//���ò���				->����ֵ [(result<<16)|(tHalUsrConfigsCmd),ʵ��ֵ(int/float)]
    HAL_KMD_GET_CONFIG,			//��ȡ����				->����ֵ [(result<<16)|(tHalUsrConfigsCmd),ʵ��ֵ(int/float)]

    HAL_KMD_GET_FW_VERSION,		//��ȡ����汾��
	
    HAL_KMD_FSM_HEARTBEAT0,		//������״̬ 0
	HAL_KMD_POS_HEARTBEAT1,		//����λ����Ϣ 1
	HAL_KMD_VEL_HEARTBEAT2,		//�����ٶ���Ϣ 2
	HAL_KMD_CUR_HEARTBEAT3,		//���������Ϣ 3
	HAL_KMD_BUS_HEARTBEAT4,		//����������Ϣ 4
	
	HAL_KMD_GET_DEVIEC_ID_00_63, //��ȡоƬID
	HAL_KMD_GET_DEVIEC_ID_64_95, //��ȡоƬID

    HAL_KMD_SYSTEM_RESET,       //��λϵͳ
    
    HAL_KMD_MAX_NUM,
} tHalCanKmdCmd;

typedef enum eHalMotorCmd
{
    HAL_MOTOR_DISABLE = 0,          //ʧ�ܵ��              pData[1] = ��ǰλ��
    HAL_MOTOR_ENABLE,               //ʹ�ܵ��              pData[1] = ��ǰλ��

    HAL_MOTOR_ERROR_REPORT,         //�������              pData[1] = ������Ϣ���Զ��ϱ�����ȡ��Ч
    HAL_MOTOR_ERROR_RESET,          //��λ����              pData[1] = 0

    HAL_MOTOR_GET_STAT,             //��ȡ���״̬          pData[1] = ����״̬

    HAL_MOTOR_CALIBRATION_START,    //��ʼУ׼              pData[1] = 0
    HAL_MOTOR_CALIBRATION_ABORT,    //��λУ׼              pData[1] = 0

    HAL_MOTOR_CONFIGS_UPDATA,        //�������             pData[1] = 0
    HAL_MOTOR_CONFIGS_RESET_ALL,	 //��λ���в���         pData[1] = 0
} tHalMotorCmd;


//128
typedef enum eHalUsrConfigsCmd
{
    HAL_USER_CONFIG_NULL = 0,
    // Motor
    HAL_USER_CONFIG_MOTOR_POLE_PAIRS,
    HAL_USER_CONFIG_MOTOR_PHASE_RESISTANCE,
    HAL_USER_CONFIG_MOTOR_PHASE_INDUCTANCE,
    HAL_USER_CONFIG_INERTIA,
    // Encoder
    HAL_USER_CONFIG_ENCODER_TYPE,
    HAL_USER_CONFIG_ENCODER_PLL_BW,
    HAL_USER_CONFIG_ENCODER_CPR,

    // Calib
    HAL_USER_CONFIG_CALIB_VALID,
    HAL_USER_CONFIG_CALIB_CURRENT,
    HAL_USER_CONFIG_CALIB_MAX_VOLTAGE,
    // Control
    HAL_USER_CONFIG_CONTROL_MODE,
    HAL_USER_CONFIG_CURRENT_RAMP_RATE,
    HAL_USER_CONFIG_VEL_RAMP_RATE,
    HAL_USER_CONFIG_TRAJ_VEL,
    HAL_USER_CONFIG_TRAJ_ACCEL,
    HAL_USER_CONFIG_TRAJ_DECEL,
    HAL_USER_CONFIG_POS_GAIN,
    HAL_USER_CONFIG_VEL_GAIN,
    HAL_USER_CONFIG_VEL_INTEGRATOR_GAIN,
    HAL_USER_CONFIG_VEL_LIMIT,
    HAL_USER_CONFIG_CURRENT_LIMIT,
    HAL_USER_CONFIG_CURRENT_CTRL_P_GAIN,
    HAL_USER_CONFIG_CURRENT_CTRL_I_GAIN,
    HAL_USER_CONFIG_CURRENT_CTRL_BW,
    // Protect
    HAL_USER_CONFIG_PROTECT_UNDER_VOLTAGE,
    HAL_USER_CONFIG_PROTECT_OVER_VOLTAGE,
	HAL_USER_CONFIG_PROTECT_OVER_CURRENT,
    HAL_USER_CONFIG_PROTECT_OVER_SPEED,
    // CAN
    HAL_USER_CONFIG_CAN_ID,
    HAL_USER_CONFIG_CAN_TIMEOUT_MS,
    HAL_USER_CONFIG_CAN_HEARTBEAT_MS,
	HAL_USER_CONFIG_CAN_HEARTBEAT_CH,

    HAL_USER_CONFIG_UART_DEBUG_MS,
    HAL_USER_CONFIG_NUM,//MAX=127
} tHalUsrConfigsCmd;

typedef enum eHalControlMode{
	HAL_CONTROL_MODE_CURRENT		= 0,//����ģʽ
	HAL_CONTROL_MODE_CURRENT_RAMP	= 1,//��������ģʽ
	HAL_CONTROL_MODE_VELOCITY		= 2,//ת��ģʽ
	HAL_CONTROL_MODE_VELOCITY_RAMP	= 3,//ת������ģʽ
	HAL_CONTROL_MODE_POSITION		= 4,//λ��ģʽ
	HAL_CONTROL_MODE_POSITION_TRAP	= 5,//λ������ģʽ
} tHalControlMode;

typedef enum eHalEncoderType
{
	HAL_ENCODER_MT6825 = 0,	//�Ѿ�֧��
	HAL_ENCODER_MA730,		//�Ѿ�֧��
	HAL_ENCODER_AS5047P,	//�Ѿ�֧��
	HAL_ENCODER_HALL,
	HAL_ENCODER_ABI,		//�ݲ�֧��
} tHalEncoderType;

typedef enum eHalOrderResult
{
	HAL_RESULT_OK,
	HAL_RESULT_FAIL,
	HAL_RESULT_ERROR,
} tHalOrderResult;

#endif /* __HAL_FOC_CONFIG_H */
