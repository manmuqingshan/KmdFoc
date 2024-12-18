#include "hal_foc_calibration.h"
#include "hal_foc_config.h"
#include "hal_foc_controller.h"
#include "hal_foc_current.h"
#include "hal_foc_encoder.h"
#include "hal_foc_enum.h"
#include "hal_foc_fsm.h"
#include "hal_foc_heap.h"
#include "hal_foc_interact.h"
#include "hal_foc_interface.h"
#include "hal_foc_struct.h"
#include "hal_foc_trapTraj.h"
#include "hal_foc_usr_config.h"
#include "hal_foc_util.h"

#include "bsp_systick.h"
#include "bsp_can.h"
#include "bsp_encoder.h"
#include "bsp_svpwm.h"
#include <math.h>

#include "bsp_can.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_kmd.h"
#include "bsp_hall.h"
#include "bsp_svpwm.h"
#include "bsp_device_id.h"

#ifdef KMD_USB_USER
#include "drv_usb_hw.h"
#include "cdc_acm_core.h"
#include "drv_usbd_int.h"
extern usb_core_driver cdc_acm;
uint8_t usb_send_enable = 0;
bool usb_data_transmit(uint8_t *pData, uint16_t len);
void usb_vcp_auto_receive(void);
void usb_clear_txfifo(void);
uint32_t usb_int_epin(usb_core_driver *udev);
uint32_t usb_emptytxfifo_write(usb_core_driver *udev, uint32_t ep_num);
#endif

void flash_protect(void);

/**
 * GD32内存大小
 *                          ->>131,072  byte    ->>128      kbyte
 * 内存资源占用
 * Stack_Size 0x00000800    ->>2048     byte    ->>2        kbyte
 * Heap_Size  0x00000800    ->>2048     byte    ->>2        kbyte
 * Program Size:
 *            Code          ->>33668    byte    ->>32.88    kbyte
 *            RO-data       ->>2036     byte    ->>1.99     kbyte
 *            RW-data       ->>1480     byte    ->>1.45     kbyte
 *            ZI-data       ->>23552    byte    ->>23       kbyte
 *            All           ->>                 ->>59.32    kbyte
 * 读保护标志位地址           ->>
 */
int main(void)
{
    // APB1: Fmax = 60MHZ
    // APB2: Fmax = 120MHZ
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    rcu_periph_clock_enable(RCU_AF);
    // 初始化协议所需内存
    bsp_kmd_init();
    // 初始化systick
    bsp_systick_init();
    // 初始化FOC和FOC依赖硬件
    hal_foc_init_main();
    // 开启读保护
    flash_protect();
    while (1)
        {
            // LED灯控制函数
            bsp_led_callback();
            // FOC异常检查
            hal_foc_check_loop();
#ifdef KMD_USB_USER
            // USB收发
            usbd_isr(&cdc_acm);
            usb_vcp_auto_receive();
#endif
        }
}

void flash_protect(void)
{
    uint8_t fmc_state;
    if (RESET == ob_security_protection_flag_get())
        {
            fmc_unlock();
            ob_unlock();
            fmc_state = ob_erase();
            ob_security_protection_config(FMC_USPC);
            ob_lock();
            fmc_lock();
            bsp_systick_delay_ms(500);
            __set_PRIMASK(1);
            NVIC_SystemReset();
        }
}

#ifdef KMD_USB_USER

/**
 * @description: USB 接收函数
 * @param {uint8_t} *pRxData
 * @param {uint16_t} this_rx_buff_len
 * @return {*}
 */
bool bsp_kmd_usb_callback(uint8_t *pRxData, uint16_t this_rx_buff_len)
{
    bool ret = false;
    /* usb 接收函数 */
    ret = bsp_kmd_callback(pRxData, this_rx_buff_len, (BSP_KMD_COM_TYPE)KMD_MSG_FRAME_USB_);
    return ret;
}

/**
 * @description: USB消息发送函数
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
                                    // 发送失败，则怀疑串口未打开
                                    usb_send_enable = 0;
                                    usb_clear_txfifo();
                                    cdc->packet_sent = 0;
                                    usb_int_epin(&cdc_acm);
                                    return false;
                                }
                        };
                    // 清除发送完成标志
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
 * @description: USB消息发送函数
 * @param {uint8_t} *pData
 * @param {uint16_t} len
 * @return {*}
 */
bool bsp_kmd_usb_transmit(uint8_t *pData, uint16_t len)
{
    // 消息发送
    if(usb_send_enable!=0)
        {
            return usb_data_transmit(pData, len);
        }
    //由于当前串口未打开，强制设置为发送成功
    return true;
}

/**
 * @description: USB消息接收函数
 * @return {*}
 */
void usb_vcp_auto_receive(void)
{
    usb_cdc_handler *cdc = (usb_cdc_handler *)cdc_acm.dev.class_data[CDC_COM_INTERFACE];
    if (USBD_CONFIGURED == cdc_acm.dev.cur_status)
        {
            // 接收数据已经准备好
            if (cdc->packet_receive == 1)
                {
                    usb_send_enable = 1;
                    usbd_ep_recev(&cdc_acm, CDC_DATA_OUT_EP, (uint8_t *)(cdc->data), USB_CDC_DATA_PACKET_SIZE);
                    // 有效数据长度大于0->>进行数据处理
                    if (cdc->receive_length > 0)
                        {
                            // 有效数据小于 64 进行数据处理，避免内存越界
                            if (cdc->receive_length <= 64)
                                {
                                    bsp_kmd_usb_callback(cdc->data, cdc->receive_length);
                                }
                            // 清除消息接收标志
                            cdc->receive_length = 0;
                        }
                    // 清除消息接收标志
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
 * @Description  : 清空发送缓冲区，如果发送失败，请务必清除缓冲区，否则下次发送，会输出以前发送失败的数据，数据会错位，同时导致最新新发送的数据无法发送出去。
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
void Error_Handler(void)
{
    __disable_irq();
    bsp_svpwm_switch_off();
    bsp_led_set(1);
    hal_report_err_hanlder();
    for(uint64_t i=0; i<5000; i++)
        {
            // 大约 1us
            for(uint64_t j=0; j<120; j++)
                {
                    __NOP();
                }
        }
    __set_PRIMASK(1);
    NVIC_SystemReset();
    while (1)
        {
        }
}
