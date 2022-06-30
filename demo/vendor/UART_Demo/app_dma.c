/********************************************************************************************************
 * @file	app_dma.c
 *
 * @brief	This is the source file for B85m
 *
 * @author	Driver Group
 * @date	2018
 *
 * @par     Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "app_config.h"

#if (UART_MODE==UART_DMA)

/**
 * rec_buff_Len Set up rules:(Both conditions have to be met)
 * 1.The rec_buff_Len value is a multiple of 16 	(16 * n)
 * 2.The length of the receive data is less than (16 * n -4)
 */
#define rec_buff_Len    32
#define trans_buff_Len  16


#define TX     1
#define RX     2
#define UART_MODE_TRX RX

__attribute__((aligned(4))) unsigned char rec_buff[rec_buff_Len]={0};
__attribute__((aligned(4))) unsigned char trans_buff[trans_buff_Len] = {0x0c,0x00,0x00,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc};
volatile unsigned char uart_dmairq_tx_cnt;
volatile unsigned char uart_dmairq_rx_cnt;
extern volatile unsigned char rts_count;
volatile unsigned char uart_dmairq_err_cnt;
volatile unsigned char uart_dma_send_flag = 1;

void user_init()
{
	sleep_ms(2000);  //leave enough time for SWS_reset when power on

	//note: dma addr must be set first before any other uart initialization! (confirmed by sihui)
	uart_recbuff_init( (unsigned char *)rec_buff, sizeof(rec_buff));

#if( UART_WIRE_MODE == UART_1WIRE_MODE)
	uart_set_rtx_pin(UART_RTX_PIN);// the status of rtx line will be rx by default,if there is a send-action,the status of rtx-line will changed to tx,and changed to rx immediately if send over.
	uart_rtx_en();
#elif(( UART_WIRE_MODE == UART_2WIRE_MODE))
	uart_gpio_set(UART_TX_PIN, UART_RX_PIN);// uart tx/rx pin set
#endif

	uart_reset();  //uart module power-on again.

	//baud rate: 115200
	uart_init_baudrate(115200,CLOCK_SYS_CLOCK_HZ,PARITY_NONE, STOP_BIT_ONE);
	uart_dma_enable(1, 1); 	//uart data in hardware buffer moved by dma, so we need enable them first

#if(UART_DMA_INT_TYPE == UART_RXDMA_IRQ)
	irq_enable_type(FLD_IRQ_DMA_EN);// uart_rx use dma_rx irq

	dma_chn_irq_enable(FLD_DMA_CHN_UART_RX, 1);   	//uart Rx dma irq enable
#elif((UART_DMA_INT_TYPE == UART_RXDONE_IRQ)&&MCU_CORE_B80)
	uart_rxdone_irq_en();  //mask rx_done irq
#endif
	uart_mask_tx_done_irq_enable();

	uart_mask_error_irq_enable();// open uart_error_mask,when stop bit error or parity error,it will enter error_interrupt.
	irq_enable_type(FLD_IRQ_UART_EN);// uart_tx use uart_txdone irq
	irq_enable();

#if( FLOW_CTR ==  USE_CTS )
	uart_set_cts(1, STOP_VOLT, UART_CTS_PIN);
#elif( FLOW_CTR ==   USE_RTS )
	uart_set_rts(1, RTS_MODE, RTS_THRESH, RTS_INVERT,UART_RTS_PIN);
#endif
}

void main_loop (void)
{
#if( (FLOW_CTR == NONE)||(FLOW_CTR ==  USE_CTS))
#if(UART_MODE_TRX == TX)
	if(uart_dma_send_flag == 1)
	{
		uart_send_dma((unsigned char *)trans_buff);
		trans_buff[4] ++;
		uart_dma_send_flag = 0;
	}
#elif(UART_MODE_TRX == RX)
	gpio_toggle(LED1);
	sleep_ms(500);
#endif
#elif( FLOW_CTR ==  USE_RTS )
	while(rts_count==1) //to make the rts obvious,design the program to trigger rts one time.
	{
		uart_dma_enable(0,1); //dis rhe rx,then the data will store in the rx buff until the amount of data trigger rts pin.
		if((reg_uart_buf_cnt&FLD_UART_RX_BUF_CNT)>(RTS_THRESH-1))  //trigger rts pin
		{
			sleep_ms(1000);
			uart_dma_enable(1,1);//enable rx,then the data sending and receiving will proceed normally,
			rts_count=0;//trigger only once.
			break;
		}
	}
#endif
}
#endif














