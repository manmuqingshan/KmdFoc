#ifndef _BSP_PROTOCOL_H_
#define _BSP_PROTOCOL_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define PROTOCOL_HEAD_ID        0XA5
#define PROTOCOL_HEAD_LEN       4
#define PROTOCOL_DATA_OFFSET    10
#define MAX_BUFFER_SIZE         128


#define LOG_LEVEL_DEBUG (1<<0)
#define LOG_LEVEL_INFO_ (1<<1)
#define LOG_LEVEL_WARN_ (1<<2)
#define LOG_LEVEL_ERROR (1<<3)
#define LOG_LEVEL_FATAL (1<<4)

    typedef enum
    {
        PROTOCOL_RESULT_OK = 0,
        PROTOCOL_RESULT_ERR = 1,
        PROTOCOL_RESULT_CHECK_HEAD_ERR = 2,
        PROTOCOL_RESULT_CHECK_FRAME_ERR = 3,
        PROTOCOL_RESULT_OUT_OF_LEN = 4,
    }protocol_result;
    //32λ
    typedef union
    {
        char        data_char[4];
        uint8_t     data_u8[4];
        int16_t     data_int16[2];
        uint16_t    data_u16[2];
        int32_t     data_int32;
        uint32_t    data_u32;
        float       data_float;
    }data_union;
    typedef  struct
    {
        /*�ֽ�ƫ�� 4 -->> �ֽڴ�С 2*/
        uint16_t    equipment_type; //�豸����
        /*�ֽ�ƫ�� 6 -->> �ֽڴ�С 2*/
        uint16_t    equipment_id;	//�豸ID
        /*�ֽ�ƫ�� 8 -->> �ֽڴ�С data_len+2 */
        uint16_t    data_id;        //����ID
        struct
        {
            uint16_t    data_len;     //data_union���ݳ���
            uint16_t    max_data_len; //���������
            uint32_t*   pData;        //����(data_union),�Ƽ�ʹ�� data_union * pDataUnion = &pData[];
        }cmd_data;
    } user_data_struct;//��������

    typedef struct
    {
        uint8_t*    pData;        //����
        uint16_t    data_len;     //�����ܳ���
        uint16_t    max_data_len; //���������
    } message_struct;//��Ϣ����

    typedef struct
    {
        /*�ֽ�ƫ�� 0 -->> �ֽڴ�С 4*/
        struct
        {
            uint8_t  sof;           //�̶�֡ͷ
            uint16_t data_length;   //���ݳ���
            uint8_t  crc_check;     //֡ͷCRCУ��
        } header;					//����֡ͷ
        user_data_struct frame_user;//�û�����
        uint16_t         frame_tail;//֡βCRCУ��
    } frame_struct;

    typedef struct
    {
        frame_struct       frame_st;    //ԭʼ���ݻ�����������
        message_struct     message_st;  //�д����������ɵ�����
    } protocol_struct;



    /// <summary>
    /// ����data_union���ȼ�������֡���ȣ����ڷ����ڴ�
    /// </summary>
    /// <param name="uLen"></param>
    /// <returns></returns>
    uint16_t get_protocol_size(uint16_t uLen);

    /// <summary>
    /// ��ʽһ ��ʼ��,����ʼ���ڴ�
    /// </summary>
    /// <param name="pProtocol"></param>
    /// <param name="uLen"></param>
    void init_protocol(protocol_struct* pProtocol, uint16_t uLen);

    /// <summary>
    /// ��ʽ�� �ⲿԤ�ȷ�����ڴ�ռ�
    /// </summary>
    /// <param name="pProtocol"></param>
    /// <param name="pFrameSt"></param>
    /// <param name="pMessageSt"></param>
    /// <param name="uLen"></param>
    /// <returns></returns>
    int init_protocol_pointer(protocol_struct* pProtocol, void* pFrameSt, void* pMessageSt, uint16_t uLen);

    /// <summary>
    /// ���ɴ����͵���������
    /// </summary>
    /// <param name="pProtocol"></param>
    /// <returns></returns>
    int make_protocol(protocol_struct* pProtocol);

    /// <summary>
    /// �������յ�������
    /// </summary>
    /// <param name="pProtocol"></param>
    /// <returns></returns>
    int parse_protocol(protocol_struct* pProtocol,uint16_t parseDataLen);

    /// <summary>
    /// ����ʼ�����ͷ��ڴ�,����Ƿ�ʽ������Ȼ�󷴳�ʼ������ע����Ұָ��
    /// </summary>
    /// <param name="pProtocol"></param>
    void deinit_protocol(protocol_struct* pProtocol);



    /// <summary>
    /// �ṩ���û���ֱ�Ӳ������ݣ�������ע�ⲻҪ�������ݳ��ȣ������ڴ�Խ��
    /// </summary>
    /// <param name="pProtocol"></param>
    /// <returns></returns>
    const user_data_struct* get_user_data_point(protocol_struct* pProtocol);


    /// <summary>
    /// �ṩ���û���ֱ�Ӳ������ݣ�������ע�ⲻҪ�������ݳ��ȣ������ڴ�Խ��
    /// </summary>
    /// <param name="pProtocol"></param>
    /// <returns></returns>
    const message_struct* get_message_point(protocol_struct* pProtocol);

#ifdef __cplusplus
}
#endif

#endif
