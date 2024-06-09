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
 * @	You should have received a copy of the GNU General Public License
 * @	along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * @
 * @Author       : Copyright (c) 2022, vSeasky.Liu liuwei_seasky@163.com.
 * @Github       : https://github.com/SEASKY-Master
 * @Date         : 2022-05-27 14:01:11
 * @FilePath     : \MDK-ARMe:\KmdFoc\Tools\UsbCanBus\software\CanUsb-V4\Main\main.c
 * @Description  :
 *                  �˰�����ʾ�� Seasky Э���ʹ�ý̳̣��Լ� KmdFoc ��������������������CAN0�������USB VCPʵ�����⴮��תCAN�Ĺ��ܣ�
 *                  �����ͨ����ӵ�и��ߵĴ������ʡ�CAN1���� ����ʵ�� KmdFoc ��������
 *                  ע�⣺USB VCP���η������ݲ�Ҫ����64
 */

#include "gd32c10x.h"
#include "bsp_systick.h"
#include "bsp_can.h"
#include "bsp_can.h"
#include "bsp_timer.h"
#include "bsp_kmd.h"
#include "hal_foc_struct.h"

#ifdef KMD_USB_USER
#include "drv_usb_hw.h"
#include "cdc_acm_core.h"
#include "drv_usbd_int.h"
usb_core_driver cdc_acm;
uint8_t usb_send_enable = 0;
bool usb_data_transmit(uint8_t *pData, uint16_t len);
void usb_vcp_auto_receive(void);
void usb_clear_txfifo(void);
uint32_t usb_int_epin(usb_core_driver *udev);
uint32_t usb_emptytxfifo_write(usb_core_driver *udev, uint32_t ep_num);
#endif

int main(void)
{
    // APB1: Fmax = 60MHZ
    // APB2: Fmax = 120MHZ
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    rcu_periph_clock_enable(RCU_AF);
    // ��ʼ��systick
    bsp_systick_init();
    // ��ʼ��kmd-foc��Ҫ��Э��
    bsp_protocol_init();
    // USB��ʼ��
    {
#ifdef KMD_USB_USER
        usb_rcu_config();
        usb_timer_init();
        usbd_init(&cdc_acm, USB_CORE_ENUM_FS, &cdc_desc, &cdc_class);
        usb_intr_config();
#ifdef USE_IRC48M
        rcu_periph_clock_enable(RCU_CTC);
        ctc_config();
        while (ctc_flag_get(CTC_FLAG_CKOK) == RESET)
            {
            }
#endif
#endif
    }
    // ��ʼ��can
    bsp_can0_init();
    bsp_can1_init();
    // ��ʼ��led
    bsp_led_init();
    // ��ʼ����ʱ��5
    bsp_timer5_init();
    // ��ʼ������
    bsp_key_init();
    while (1)
        {
            kmd_foc_app_loop();
        }
}

#ifdef KMD_USB_USER
/**
 * @Description  : usb��Ϣ����
 * @param         (uint8_t) *pRxData
 * @param         (uint16_t) this_rx_buff_len
 * @return        (*)
 */
bool bsp_kmd_usb_callback(uint8_t *pRxData, uint16_t this_rx_buff_len)
{
    bool ret = false;
    /* usb ���պ��� */
    ret = bsp_usb_can_callback(pRxData, this_rx_buff_len, KMD_MSG_FRAME_USB_);
    return ret;
}

uint64_t hpcs_plst_count = 0;
/**
 * @description: USB��Ϣ���ͺ���
 * @param {uint8_t} *pData
 * @param {uint16_t} len
 * @return {*}
 */
bool usb_data_transmit(uint8_t *pData, uint16_t len)
{
    uint64_t timeTickOut;
    usb_cdc_handler *cdc = (usb_cdc_handler *)cdc_acm.dev.class_data[CDC_COM_INTERFACE];
    if (USBD_CONFIGURED == cdc_acm.dev.cur_status)
        {
            if (cdc->packet_sent == 0)
                {
                    usbd_ep_send(&cdc_acm, CDC_DATA_IN_EP, pData, len);
                    timeTickOut = 0;
                    while (cdc->packet_sent == 0)
                        {
                            usb_int_epin(&cdc_acm);
                            cdc = (usb_cdc_handler *)cdc_acm.dev.class_data[CDC_COM_INTERFACE];
                            timeTickOut++;
                            if (timeTickOut > 256)
                                {
                                    // ����ʧ�ܣ����ɴ���δ��
                                    usb_send_enable = 0;
                                    usb_clear_txfifo();
                                    cdc->packet_sent = 0;
                                    usb_int_epin(&cdc_acm);
                                    return false;
                                }
                        };
                    // ���������ɱ�־
                    cdc->packet_sent = 0;
                    usb_int_epin(&cdc_acm);
                    return true;
                }
            else
                {
                    usb_clear_txfifo();
                    cdc->packet_sent = 0;
                    usb_int_epin(&cdc_acm);
                    return false;
                }
        }
}

/**
 * @Description  : USB VCP��Ϣ���ʹ���
 * @param         (uint8_t) *pData
 * @param         (uint16_t) len
 * @return        (*)
 */
bool bsp_usb_transmit(uint8_t *pData, uint16_t len)
{
    if(usb_send_enable==1)
        {
            usb_data_transmit(pData, len);
        }
    return true;
}

/**
 * @description: USB��Ϣ���պ���
 * @return {*}
 */
void usb_vcp_auto_receive(void)
{
    usb_cdc_handler *cdc = (usb_cdc_handler *)cdc_acm.dev.class_data[CDC_COM_INTERFACE];
    if (USBD_CONFIGURED == cdc_acm.dev.cur_status)
        {
            // ���������Ѿ�׼����
            if (cdc->packet_receive == 1)
                {
                    usb_send_enable = 1;
                    usbd_ep_recev(&cdc_acm, CDC_DATA_OUT_EP, (uint8_t *)(cdc->data), USB_CDC_DATA_PACKET_SIZE);
                    // ��Ч���ݳ��ȴ���0->>�������ݴ���
                    if (cdc->receive_length > 0)
                        {
                            // ��Ч����С�� 64 �������ݴ��������ڴ�Խ��
                            if (cdc->receive_length <= 64)
                                {
                                    bsp_kmd_usb_callback(cdc->data, cdc->receive_length);
                                }
                            // �����Ϣ���ձ�־
                            cdc->receive_length = 0;
                        }
                    // �����Ϣ���ձ�־
                    cdc->packet_receive = 0;
                }
            if(cdc->packet_sent == 1)
                {
                    usb_clear_txfifo();
                    cdc->packet_sent = 0;
                    usb_int_epin(&cdc_acm);
                }
        }
}

/**
 * @Description  : ��շ��ͻ��������������ʧ�ܣ����������������������´η��ͣ��������ǰ����ʧ�ܵ����ݣ����ݻ��λ��ͬʱ���������·��͵������޷����ͳ�ȥ��
 * @return        (*)
 */
void usb_clear_txfifo(void)
{
    uint8_t i = 0;
    usb_core_driver *udev = &cdc_acm;
    usb_transc *transc = &udev->dev.transc_in[EP_ID(CDC_DATA_IN_EP)];
    uint8_t ep_num = transc->ep_addr.num;
    (void)usb_txfifo_flush(&udev->regs, 0x10U);
    udev->regs.er_in[ep_num]->DIEPLEN = 0U;
    udev->regs.er_in[ep_num]->DIEPINTF = 0xFFU;
    if (udev->regs.er_out[ep_num]->DOEPCTL & DEPCTL_EPEN)
        {
            udev->regs.er_out[ep_num]->DOEPCTL |= DEPCTL_EPD | DEPCTL_SNAK;
        }
    else
        {
            udev->regs.er_out[ep_num]->DOEPCTL = 0U;
        }
    udev->regs.er_out[ep_num]->DOEPLEN = 0U;
    udev->regs.er_out[ep_num]->DOEPINTF = 0xFFU;
    udev->regs.gr->GINTF = GINTF_ISOINCIF;
}

/**
 * @Description  :
 * @param         (usb_core_driver) *udev
 * @return        (*)
 */
uint32_t usb_int_epin(usb_core_driver *udev)
{
    uint32_t epintnum = 0U;
    uint8_t ep_num = 0U;
    for (epintnum = usb_iepintnum_read(udev); epintnum; epintnum >>= 1, ep_num++)
        {
            if (epintnum & 0x1U)
                {
                    __IO uint32_t iepintr = usb_iepintr_read(udev, ep_num);
                    if (iepintr & DIEPINTF_TF)
                        {
                            udev->regs.er_in[ep_num]->DIEPINTF = DIEPINTF_TF;
                            /* data transmission is completed */
                            (void)usbd_in_transc(udev, ep_num);
                            if ((uint8_t)USB_USE_DMA == udev->bp.transfer_mode)
                                {
                                    if ((0U == ep_num) && ((uint8_t)USB_CTL_STATUS_IN == udev->dev.control.ctl_state))
                                        {
                                            usb_ctlep_startout(udev);
                                        }
                                }
                        }
                    if (iepintr & DIEPINTF_TXFE)
                        {
                            usb_emptytxfifo_write(udev, (uint32_t)ep_num);
                            udev->regs.er_in[ep_num]->DIEPINTF = DIEPINTF_TXFE;
                        }
                }
        }
    return 1U;
}

/**
 * @Description  :
 * @param         (usb_core_driver) *udev
 * @param         (uint32_t) ep_num
 * @return        (*)
 */
uint32_t usb_emptytxfifo_write(usb_core_driver *udev, uint32_t ep_num)
{
    uint32_t len;
    uint32_t word_count;

    usb_transc *transc = &udev->dev.transc_in[ep_num];

    len = transc->xfer_len - transc->xfer_count;

    /* get the data length to write */
    if (len > transc->max_len)
        {
            len = transc->max_len;
        }

    word_count = (len + 3U) / 4U;

    while (((udev->regs.er_in[ep_num]->DIEPTFSTAT & DIEPTFSTAT_IEPTFS) >= word_count) &&
            (transc->xfer_count < transc->xfer_len))
        {
            len = transc->xfer_len - transc->xfer_count;

            if (len > transc->max_len)
                {
                    len = transc->max_len;
                }

            /* write FIFO in word(4bytes) */
            word_count = (len + 3U) / 4U;

            /* write the FIFO */
            (void)usb_txfifo_write(&udev->regs, transc->xfer_buf, (uint8_t)ep_num, (uint16_t)len);

            transc->xfer_buf += len;
            transc->xfer_count += len;

            if (transc->xfer_count == transc->xfer_len)
                {
                    /* disable the device endpoint FIFO empty interrupt */
                    udev->regs.dr->DIEPFEINTEN &= ~(0x01U << ep_num);
                }
        }

    return 1U;
}

#endif
// �����쳣 ��λ
void Error_Handler(void)
{
    __disable_irq();
    __set_PRIMASK(1);
    NVIC_SystemReset();
    while (1)
        {
        }
}
