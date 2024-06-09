#ifndef _BSP_PROTOCOL_CLASS_H_
#define _BSP_PROTOCOL_CLASS_H_

#include "bsp_protocol.h"

#ifdef CLR_CONFIG
//CLR_CONFIG ���ڻ�࣬C++����C# �ɵ���Dll
#include  <vSerial/vSerialPort.h>
#define PROTOCOL_CLASS_REF  public ref class
#define PROTOCOL_STRUCT_REF public ref struct
#define PROTOCOL_CPP_CLR_DEBUG
void bsp_debug_c(uint8_t level, const char* Format, ...);
#else
#define PROTOCOL_CLASS_REF		class
#define PROTOCOL_STRUCT_REF		struct
#endif// CLR_CONFIG


#ifdef CLR_CONFIG
namespace ComCanProtocol
{
#endif

#ifdef CLR_CONFIG
	//����һ���й����ͣ�������ʾ��Ҫ���µ����ݵ���
	public delegate void  pCanReceiveCppPointer(uint32_t CanId, uint32_t* pData);
	public delegate void  pCanComDebug(char* pStrError);
	public enum ComCanType
	{
		COM_CAN_TYPE_SERIAL = 0,
		COM_CAN_TYPE_TCP = 1,
		COM_CAN_TYPE_BLE = 2,
		COM_CAN_TYPE_NUM
	};
	public enum class COM_LOG_LEVEL
	{
		COM_LOG_LEVEL_DEBUG = (1 << 0),
		COM_LOG_LEVEL_INFO_ = (1 << 1),
		COM_LOG_LEVEL_WARN_ = (1 << 2),
		COM_LOG_LEVEL_ERROR = (1 << 3),
		COM_LOG_LEVEL_FATAL = (1 << 4),
	};
#endif
	/// <summary>
	/// kmdFocComͨ�����
	/// </summary>
	PROTOCOL_CLASS_REF ComCanClass
	{
		public:
			ComCanClass(void);
			~ComCanClass();
			//Э������
			protocol_struct* pTxProtocol = new protocol_struct();
			protocol_struct* pRxProtocol = new protocol_struct();
			bool GetStorageMethodIsSmall(void);
			/******************************************************************************/
			/***********************************Protocol***********************************/
			void ProtocolInit();
			uint16_t ProtocolCalcLen(uint16_t uLen);
			void ProtocolAutoInitTx(uint16_t uLen);
			void ProtocolAutoInitRx(uint16_t uLen);
			void ProtocolTxPointGet(uint32_t* pTxData, uint8_t* pTxBuffer, uint16_t& uMaxLen);
			void ProtocolRxPointGet(uint32_t* pTxData, uint8_t* pTxBuffer, uint16_t& uMaxLen);
			void ProtocolInitTx(uint32_t* pTxData, uint8_t* pTxBuffer, uint16_t uLen);
			void ProtocolInitRx(uint32_t* pRxData, uint8_t* pRxBuffer, uint16_t uLen);
			/***********************************Protocol***********************************/
			/******************************************************************************/

			/******************************************************************************/
			/************************************Serial************************************/
#ifdef CLR_CONFIG
			bool SerialOpen(uint32_t com_num, uint32_t baud_rate, uint32_t parity, uint32_t byte_size, uint32_t stop_bits);
			void SerialClose();
			bool SerialIsOpened();
			void SerialClearBuffer(void);
			uint8_t SerialGetLastError();
			void SerialReceiveTask();
			/************************************Serial************************************/
			/******************************************************************************/
			//CAN��Ϣ����
			void CanDataTransmit(uint32_t CanCmd, uint32_t* pData);
			void SetCanReceivCallbackFun(pCanReceiveCppPointer^ pFun)
			{
				pCanReceiveCallbackFun = pFun;
				pCanReceiveCppPointerIsEnable = true;
				bsp_debug_c(LOG_LEVEL_DEBUG,"SetCanReceivCallbackFun\n");
			};
			void SetCanComDebugCallbackFun(uint8_t debugLevel,pCanComDebug^ pFun);
			void ComCanSetEquipmentType(uint16_t type)
			{
				can_equipment_type = type;
			}
			bool ComCanIsOpen(void);
			void ComDebugCallBack(void);
		private:
			static ComCanType	ComCanTypeIndex = COM_CAN_TYPE_SERIAL;	 //ͨ�ŷ�ʽ����
			static bool		SerialIsOpen = false;						//���ڴ򿪱�־
			static bool		TcpIsOpen = false;							//Tcp�򿪱�־
			static bool     BleIsOpen = false;							//�����򿪱�־
			static bool     pCanReceiveCppPointerIsEnable = false;
			static bool     pCanComDebugIsEnable = false;
			const  uint16_t SerialProtocolLen = 20;						//��������֡����
			static uint16_t can_equipment_type;
			static vSerialPort* vSerialPortClass = new vSerialPort();
			void  CanRxCallBack(void);
			static pCanReceiveCppPointer^ pCanReceiveCallbackFun;
			static pCanComDebug^ pCanComDebugCallbackFun;
			void ProtocolTransmitConfig(uint16_t equipment_type, uint16_t equipment_id, uint16_t data_id, uint16_t data_len);
			void ProtocolTransmit();
			void ComDebugPrintf(char* pStr);
#endif
	};

#ifdef CLR_CONFIG
}
#endif

#endif


