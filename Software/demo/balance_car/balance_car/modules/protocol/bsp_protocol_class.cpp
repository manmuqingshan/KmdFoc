#include  <stdlib.h>           
#include  <string.h> 
#include  <iostream>
#include  <thread>
#include  "bsp_protocol_class.h"

#ifdef CLR_CONFIG
uint8_t debug_level_config;
string debug_level_str[5] =
{
	"[ComCanClass.DEBUG]:",
	"[ComCanClass.INFO_]:",
	"[ComCanClass.WARN_]:"
	"[ComCanClass.ERROR]:",
	"[ComCanClass.FATAL]:"
};
string debug_info;
#define TX_RX_BUFFER_SIZE 1024
#endif

#ifdef CLR_CONFIG
/// <summary>
/// ͨ�����ַ�ʽʵ��CAN��Ϣ�Ĵ���
/// </summary>
namespace ComCanProtocol
{
#endif
	/// <summary>
	/// ���û�������
	/// </summary>
	ComCanClass::ComCanClass()
	{
#ifdef CLR_CONFIG
		vSerialPortClass->setBufferSize(TX_RX_BUFFER_SIZE, TX_RX_BUFFER_SIZE);
#endif
	}
	ComCanClass::~ComCanClass()
	{
#ifdef CLR_CONFIG
		vSerialPortClass->closeSerial();
		SerialIsOpen = false;
#endif
	}



	/// <summary>
	/// ���������ݵĴ�С�˲���
	/// </summary>
	/// <param name="uLen"></param>
	bool ComCanClass::GetStorageMethodIsSmall(void)
	{
		int ret = -1;
		data_union data_union_t[2];
		data_union_t[0].data_u32 = 1;
		data_union_t[1].data_u32 = 2;
		if (data_union_t[0].data_u8[0] == 1)
		{
			//            bsp_debug_c(LOG_LEVEL_INFO_,"little endian storage!\n");
			return true;
		}
		else
		{
			//            bsp_debug_c(LOG_LEVEL_INFO_,"big endian storage!\n");
			return false;
		}
	}

	/******************************************************************************/
	/***********************************Protocol***********************************/
	void ComCanClass::ProtocolInit()
	{
		ProtocolAutoInitTx(12);
		ProtocolAutoInitRx(12);
	}

	/// <summary>
	/// ͨ����̬ Data(uint32_t) ���ȼ��� ������Buffer(uint8_t) �ĳ���
	/// </summary>
	/// <param name="uLen"></param>
	/// <returns></returns>
	uint16_t ComCanClass::ProtocolCalcLen(uint16_t uLen)
	{
		return get_protocol_size(uLen);
	}

	/// <summary>
	/// �Զ���ʼ��Tx���趯̬�ڴ棬uLen��С��128���ڲ������ڴ棬���ⲿ�й��޷�����
	/// </summary>
	/// <param name="uLen">pTxData�����ݳ���</param>
	void ComCanClass::ProtocolAutoInitTx(uint16_t uLen)
	{
		pTxProtocol->frame_st.frame_user.cmd_data.pData = NULL;
		pTxProtocol->message_st.pData = NULL;
		init_protocol(pTxProtocol, uLen);
	}

	/// <summary>
	/// �Զ���ʼ��Rx���趯̬�ڴ棬uLen��С��128���ڲ������ڴ棬���ⲿ�й��޷�����
	/// </summary>
	/// <param name="uLen">pRxData���ݳ���</param>
	void ComCanClass::ProtocolAutoInitRx(uint16_t uLen)
	{
		pRxProtocol->frame_st.frame_user.cmd_data.pData = NULL;
		pRxProtocol->message_st.pData = NULL;
		init_protocol(pRxProtocol, uLen);
	}

	/// <summary>
	/// ��ȡTx��̬�ڴ��ַ�ͳ��ȣ����س���Ϊ pRxData���ȣ�pRxBuffer����Ϊ uMaxLen*4+12
	/// </summary>
	/// <param name="pTxData"></param>
	/// <param name="pTxBuffer"></param>
	/// <param name="uMaxLen">pTxData�����ݳ���</param>
	void ComCanClass::ProtocolTxPointGet(uint32_t* pTxData, uint8_t* pTxBuffer, uint16_t& uMaxLen)
	{
		pTxData = &pTxProtocol->frame_st.frame_user.cmd_data.pData[0];
		pTxBuffer = &pTxProtocol->message_st.pData[0];
		uMaxLen = pTxProtocol->frame_st.frame_user.cmd_data.max_data_len;
	}

	/// <summary>
	/// ��ȡRx��̬�ڴ��ַ�ͳ��ȣ����س���Ϊ pRxData���ȣ�pRxBuffer����Ϊ uMaxLen*4+12
	/// </summary>
	/// <param name="pRxData"></param>
	/// <param name="pRxBuffer"></param>
	/// <param name="uMaxLen">pRxData�����ݳ���</param>
	void ComCanClass::ProtocolRxPointGet(uint32_t* pRxData, uint8_t* pRxBuffer, uint16_t& uMaxLen)
	{
		pRxData = &pRxProtocol->frame_st.frame_user.cmd_data.pData[0];
		pRxBuffer = &pRxProtocol->message_st.pData[0];
		uMaxLen = pRxProtocol->frame_st.frame_user.cmd_data.max_data_len;
	}

	/// <summary>
	/// ��ʼ��Э��Tx�����ڴ棬�ⲿ���䣬uLen��С��128
	/// </summary>
	/// <param name="pTxData"></param>
	/// <param name="pTxBuffer"></param>
	/// <param name="uLen">pTxData�����ݳ���</param>
	void ComCanClass::ProtocolInitTx(uint32_t* pTxData, uint8_t* pTxBuffer, uint16_t uLen)
	{
		pTxProtocol->frame_st.frame_user.cmd_data.pData = NULL;
		pTxProtocol->message_st.pData = NULL;
		init_protocol_pointer(pTxProtocol, pTxData, pTxBuffer, uLen);
	}

	/// <summary>
	/// ��ʼ��Э��Rx�����ڴ�,�ⲿ���䣬uLen��С��128
	/// </summary>
	/// <param name="pRxData"></param>
	/// <param name="pRxBuffer"></param>
	/// <param name="uLen">pRxData�����ݳ���</param>
	void ComCanClass::ProtocolInitRx(uint32_t* pRxData, uint8_t* pRxBuffer, uint16_t uLen)
	{
		pRxProtocol->frame_st.frame_user.cmd_data.pData = NULL;
		pRxProtocol->message_st.pData = NULL;
		init_protocol_pointer(pRxProtocol, pRxData, pRxBuffer, uLen);
	}
	/***********************************Protocol***********************************/
	/******************************************************************************/

	/******************************************************************************/
	/************************************Serial************************************/
#ifdef CLR_CONFIG
	bool ComCanClass::ComCanIsOpen()
	{
		switch (ComCanTypeIndex)
		{
		case COM_CAN_TYPE_SERIAL: {
			return SerialIsOpened();
		}break;
		case COM_CAN_TYPE_TCP: {

		}break;
		case COM_CAN_TYPE_BLE: {

		}break;
		}
	}
	/// <summary>
	/// ͬ����ʽ�򿪴���
	/// </summary>
	/// <param name="com_num">���ں�</param>
	/// <param name="baud_rate">������</param>
	/// <param name="parity">��żУ��λ</param>
	/// <param name="byte_size">����λ</param>
	/// <param name="stop_bits">ֹͣλ</param>
	/// <returns></returns>
	bool ComCanClass::SerialOpen(uint32_t com_num, uint32_t baud_rate, uint32_t parity, uint32_t byte_size, uint32_t stop_bits)
	{
		//�йܲ�֧��string
		string serial_name = "";
		//Win Api�޷�ֱ��֧�ִ���10��COM��
		if (com_num < 10)
		{
			serial_name += "COM";
		}
		else
		{
			serial_name += "\\\\.\\COM";
		}
		serial_name += std::to_string(com_num);
		SerialIsOpen = vSerialPortClass->openSyn(serial_name, baud_rate, parity, byte_size, stop_bits);
		SerialIsOpen = vSerialPortClass->isOpened();
		return SerialIsOpen;
	}

	/// <summary>
	/// �رմ���
	/// </summary>
	void ComCanClass::SerialClose()
	{
		SerialIsOpen = false;
		vSerialPortClass->closeSerial();
	}

	/// <summary>
	/// �жϴ����Ƿ��
	/// </summary>
	/// <returns>��ȷ����Ϊture�����󷵻�Ϊfalse</returns>
	bool ComCanClass::SerialIsOpened()
	{
		SerialIsOpen = vSerialPortClass->isOpened();
		return SerialIsOpen;
	}

	/// <summary>
	/// ���������
	/// </summary>
	void ComCanClass::SerialClearBuffer()
	{
		vSerialPortClass->purgeBuff(PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT);
	}

	/// <summary>
	/// �õ����һ��ʧ�ܵĴ�����Ϣ
	/// </summary>
	/// <returns></returns>
	uint8_t ComCanClass::SerialGetLastError()
	{
		return vSerialPortClass->getSerialLastError();
	}

	/// <summary>
	/// ���ô��������ݹ̶�������Ϣ
	/// </summary>
	/// <param name="equipment_type">�豸����</param>
	/// <param name="equipment_id">�豸ID</param>
	/// <param name="data_id">����ID</param>
	/// <param name="data_len">��̬���ݶγ���</param>
	void ComCanClass::ProtocolTransmitConfig(uint16_t equipment_type, uint16_t equipment_id, uint16_t data_id, uint16_t data_len)
	{
		user_data_struct* pTxUser = &pTxProtocol->frame_st.frame_user;
		pTxUser->equipment_type = equipment_type;
		pTxUser->equipment_id = equipment_id;
		pTxUser->data_id = data_id;
		pTxUser->cmd_data.data_len = data_len;
	}
	/// <summary>
	/// ���ú�������һ���������߳��У���ʵ�ִ�����Ϣ�Ľ��մ���
	/// </summary>
	void ComCanClass::SerialReceiveTask()
	{
		int Ret = -1;
		uint8_t  tRxBuffer[TX_RX_BUFFER_SIZE] = { 0 };
		static uint32_t curr_read_len = 0;  //��ǰ��������
		static uint32_t last_len = 0;       //��һ�δ���ȡ����
		static uint32_t total_read_len;     //�ܶ�ȡ����
		static uint32_t reality_read_len;   //ʵ�ʶ�ȡ����
		static uint32_t parse_pos;          //���ݽ���ָ��
		uint32_t ready_read_len = 0;
		static double start_time = GetTickCount();
		static double read_time = GetTickCount();
		static double end_time = GetTickCount();
		while (SerialIsOpen == true)
		{
			if (vSerialPortClass->isOpened())
			{
				total_read_len = 0;
				//��ȡ��ǰ���ݣ������Ǳ��ضϣ�ֻ���Ǳ����
				total_read_len = vSerialPortClass->getSerialReceiveLen();
				curr_read_len = total_read_len - last_len;
				//curr_read_len = vSerialPortClass->readData(&((char*)tRxBuffer)[total_read_len], TX_RX_BUFFER_SIZE-total_read_len);
				end_time = GetTickCount();
				//������һ�ζ������ݵ�ʱ��
				if (((end_time - start_time) > 16))
				{
					//�����Ƿ������ݶ���Ҫ���� readData ������������־
					ready_read_len = SerialProtocolLen * (total_read_len / SerialProtocolLen);
					reality_read_len = vSerialPortClass->readData(&((char*)tRxBuffer)[0], SerialProtocolLen * (total_read_len / SerialProtocolLen));
					//���ڴ���ȡ����
					if (reality_read_len > 0)
					{
						parse_pos = 0;
						//�ȴ���������������
						while ((parse_pos < reality_read_len))
						{
							memcpy(&pRxProtocol->message_st.pData[0], &tRxBuffer[parse_pos], reality_read_len - parse_pos);
							Ret = parse_protocol(pRxProtocol, reality_read_len - parse_pos);
							if (Ret != PROTOCOL_RESULT_OK)
							{
								total_read_len = vSerialPortClass->getSerialReceiveLen();
								vSerialPortClass->readData(&((char*)tRxBuffer)[0], total_read_len);
								//ֱ����Ϊ�������
								bsp_debug_c(LOG_LEVEL_ERROR, "ReceiveTask Parse Err!\n");
								bsp_debug_c(LOG_LEVEL_ERROR, "Clear RaedBuffer Len [%d]!\n", total_read_len);
								parse_pos = total_read_len;
								break;
							}
							else
							{
								bsp_debug_c(LOG_LEVEL_DEBUG, "CanRxCallBack [%d]!\n", reality_read_len);
								CanRxCallBack();
							}
							//�ƶ�����һ��δ��������
							parse_pos += pRxProtocol->message_st.data_len;
							read_time = GetTickCount();
							//�������ݳ�ʱ
							if ((read_time - end_time) > 32)
							{
								//ֱ����Ϊ�������
								bsp_debug_c(LOG_LEVEL_ERROR, "Parse Data Time Out [%d]!\n", read_time - end_time);
								parse_pos = total_read_len;
								break;
							}
						}
						total_read_len = 0;
					}
				}
				//���ȷ����ı䣬���µȴ�ʱ����ʼֵ
				if (curr_read_len > 0)
				{
					curr_read_len = 0;
					start_time = GetTickCount();
				}
				//������һ���ܳ���
				last_len = total_read_len;
			}
		}
	}

	/// <summary>
	/// Э����㣬����������
	/// </summary>
	void ComCanClass::ProtocolTransmit()
	{
		int ret = -1;
		/*�޸Ĵ����͵���Ϣ*/
		ret = make_protocol(pTxProtocol);
		switch (ComCanTypeIndex)
		{
		case COM_CAN_TYPE_SERIAL:
		{
			if (SerialIsOpened())
			{
				vSerialPortClass->writeData((char*)pTxProtocol->message_st.pData, pTxProtocol->message_st.data_len);
			}
		}break;
		case COM_CAN_TYPE_TCP:; break;
		case COM_CAN_TYPE_BLE:; break;
		default:
			break;
		}
	}

	/// <summary>
	/// ��װ����Э�飬��ȷ�װΪCanͨ��Э��
	/// </summary>
	/// <param name="KmdCmd">Kmd����ID</param>
	/// <param name="pData">data_u32[2]</param>
	void ComCanClass::CanDataTransmit(uint32_t CanCmd, uint32_t* pData)
	{
		ProtocolTransmitConfig(can_equipment_type, (CanCmd & 0xffff), ((CanCmd >> 16) & 0xffff), 2);
		pTxProtocol->frame_st.frame_user.cmd_data.pData[0] = pData[0];
		pTxProtocol->frame_st.frame_user.cmd_data.pData[1] = pData[1];
		ProtocolTransmit();
	}

	/// <summary>
	/// CanRxCallBack
	/// </summary>
	void ComCanClass::CanRxCallBack()
	{
		static uint16_t CanCmdId = 0;
		static uint32_t pRxData[2];
		user_data_struct* pRxUser = &pRxProtocol->frame_st.frame_user;
		if (pRxUser->equipment_type == can_equipment_type)
		{
			CanCmdId = (pRxUser->equipment_id) | (pRxUser->data_id << 16);
			pRxData[0] = pRxUser->cmd_data.pData[0];
			pRxData[1] = pRxUser->cmd_data.pData[1];
			if (pCanReceiveCppPointerIsEnable)
			{
				pCanReceiveCallbackFun(CanCmdId, pRxData);
			}
			else
			{
				ComDebugPrintf("Needs to be  SetCanReceivCallbackFun \n");
			}
		}
	}


	void ComCanClass::ComDebugPrintf(char* pStr)
	{
		if (pCanComDebugIsEnable)
		{
			pCanComDebugCallbackFun(pStr);
		}
	}

	/// <summary>
	/// ʹ�ö�ʱ����ѯ�������������Ϣ
	/// </summary>
	void ComCanClass::ComDebugCallBack()
	{
		char pStr[4096];
		if (debug_info.size() > 0)
		{
			strncpy(pStr, debug_info.data(), 4096);
			ComDebugPrintf(pStr);
			debug_info.clear();
		}
	}

	void ComCanClass::SetCanComDebugCallbackFun(uint8_t debugLevel, pCanComDebug^ pFun)
	{
		debug_level_config = debugLevel;
		pCanComDebugCallbackFun = pFun;
		pCanComDebugIsEnable = true;
		debug_info.clear();
	}
#endif
	/************************************Serial************************************/
	/******************************************************************************/

#ifdef CLR_CONFIG
}
#endif


#ifdef CLR_CONFIG
void bsp_debug_c(uint8_t level, const char* Format, ...)
{
	string str;
	int ret;
	char debugBuffer[4096];
	if ((level & debug_level_config) != 0)
	{
		va_list aptr;
		va_start(aptr, Format);
		ret = vsprintf(debugBuffer, Format, aptr);
		va_end(aptr);
		switch (level)
		{
		case LOG_LEVEL_DEBUG:debug_info.append(debug_level_str[0]); break;
		case LOG_LEVEL_INFO_:debug_info.append(debug_level_str[1]); break;
		case LOG_LEVEL_WARN_:debug_info.append(debug_level_str[2]); break;
		case LOG_LEVEL_ERROR:debug_info.append(debug_level_str[3]); break;
		case LOG_LEVEL_FATAL:debug_info.append(debug_level_str[4]); break;
		}
		debug_info.append(debugBuffer);
	}
}
#endif
