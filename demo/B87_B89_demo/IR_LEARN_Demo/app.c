/********************************************************************************************************
 * @file	app.c
 *
 * @brief	This is the source file for b85m
 *
 * @author	Driver Group
 * @date	2020
 *
 * @par     Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#include "app_config.h"

#if (MCU_CORE_B87)
#define GPIO_Type		AS_PWM0
#elif (MCU_CORE_B89)
#define GPIO_Type		PC2_PWM0
#endif

#define PWM_PIN			GPIO_PC2
#define IRLEARN_RX_PIN	GPIO_PB5

#define IR_LEARN_MODE					1
#define IR_LEARN_FOR_EDGE_DETECT_MODE	2
#define MODE_SELECT						IR_LEARN_MODE

#if (MODE_SELECT == IR_LEARN_MODE)

#define RISING_EDGE_CNT_MODE		0		//RISING_EDGE_START_CNT
#define HIGH_STATE_CNT_MODE			1		//IL_EN_HIGH_STATUS_START_CNT
#define FALLING_EDGE_CNT_MODE		2		//FALLING_EDGE_START_CNT
#define LOW_STATE_CNT_MODE			3		//IL_EN_LOW_STATUS_START_CNT
#define IRLEARN_CNT_MODE			RISING_EDGE_CNT_MODE

volatile unsigned short ir_dma_send_buff[16]={0};
volatile unsigned short il_wave_send_buff[16]={0};
volatile unsigned short il_wave_receive_buff[18]={0};

volatile unsigned int il_index = 0;
volatile unsigned int ir_irq_cnt = 0;

#elif (MODE_SELECT == IR_LEARN_FOR_EDGE_DETECT_MODE)

#define RISING_EDGE			1
#define FALLING_EDGE		2
#define GPIO_IRQ_TYPE		FALLING_EDGE

volatile unsigned int ir_rising_cnt = 0;
volatile unsigned int ir_falling_cnt = 0;

#endif


void user_init()
{
	gpio_set_func(LED1|LED2|LED3|LED4,AS_GPIO);
	gpio_set_output_en(LED1|LED2|LED3|LED4,1);
	gpio_set_input_en(LED1|LED2|LED3|LED4,0);

	ir_learn_set_pin(IRLEARN_RX_PIN);
	ir_learn_set_timeout(TICK_VALUE_65535);
	irq_set_mask(FLD_IRQ_IR_LEARN_EN);

#if (MODE_SELECT == IR_LEARN_MODE)
	ir_learn_set_mode(IRLEARN_CNT_MODE);
	ir_learn_set_irq_mask(RISING_EDGE_IRQ|TIMEOUT_IRQ);

	if(IRLEARN_CNT_MODE == LOW_STATE_CNT_MODE || IRLEARN_CNT_MODE == RISING_EDGE_CNT_MODE)
	{
		gpio_setup_up_down_resistor(IRLEARN_RX_PIN, PM_PIN_PULLDOWN_100K);
	}
	else
	{
		gpio_setup_up_down_resistor(IRLEARN_RX_PIN, PM_PIN_PULLUP_10K);
	}
	ir_learn_en();	//ir module must be enabled here, otherwise the first edge will not enter the interrupt

/*****************************pwm ir dma fifo send waveform******************************************************************/
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);

	gpio_set_func(PWM_PIN, GPIO_Type);
	pwm_set_mode(PWM0_ID, PWM_IR_DMA_FIFO_MODE);
	pwm_set_cycle_and_duty(PWM0_ID, 100 * CLOCK_SYS_CLOCK_1US, 50 * CLOCK_SYS_CLOCK_1US);
	pwm_set_pwm0_shadow_cycle_and_duty(120 * CLOCK_SYS_CLOCK_1US, 80 * CLOCK_SYS_CLOCK_1US);

	unsigned char index=2;
	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 1);
	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 1);
	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 1);
	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 2);
	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 1);
	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 3);
	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 1);

	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_SHADOW, 1);
	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(0, PWM0_PULSE_SHADOW, 1);
	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_SHADOW, 1);
	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(0, PWM0_PULSE_SHADOW, 2);
	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_SHADOW, 1);
	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(0, PWM0_PULSE_SHADOW, 3);
	ir_dma_send_buff[index++]= pwm_config_dma_fifo_waveform(1, PWM0_PULSE_SHADOW, 2);

	//The array has 16 elements, two elements make up a cycle
	il_wave_send_buff[0] = 50 * CLOCK_SYS_CLOCK_1US;			//Tick value of high level duration
	il_wave_send_buff[1] = 100 * CLOCK_SYS_CLOCK_1US * 2;		//Tick value of cycle time
	il_wave_send_buff[2] = 50 * CLOCK_SYS_CLOCK_1US;
	il_wave_send_buff[3] = 100 * CLOCK_SYS_CLOCK_1US * 3;
	il_wave_send_buff[4] = 50 * CLOCK_SYS_CLOCK_1US;
	il_wave_send_buff[5] = 100 * CLOCK_SYS_CLOCK_1US * 4;
	il_wave_send_buff[6] = 50 * CLOCK_SYS_CLOCK_1US;
	il_wave_send_buff[7] = 100 * CLOCK_SYS_CLOCK_1US;

	il_wave_send_buff[8] = 80 * CLOCK_SYS_CLOCK_1US;
	il_wave_send_buff[9] = 120 * CLOCK_SYS_CLOCK_1US * 2;
	il_wave_send_buff[10] = 80 * CLOCK_SYS_CLOCK_1US;
	il_wave_send_buff[11] = 120 * CLOCK_SYS_CLOCK_1US * 3;
	il_wave_send_buff[12] = 80 * CLOCK_SYS_CLOCK_1US;
	il_wave_send_buff[13] = 120 * CLOCK_SYS_CLOCK_1US * 4;
	il_wave_send_buff[14] = 80 * CLOCK_SYS_CLOCK_1US;
	il_wave_send_buff[15] = 120 * CLOCK_SYS_CLOCK_1US;

	unsigned int length = index*2 - 4;//The first four bytes are data length bytes, not included in the actual length to be sent
	unsigned char* buff = &ir_dma_send_buff[0];
	buff[0]= length&0xff;
	buff[1]= (length>>8)&0xff;
	buff[2]= (length>>16)&0xff;
	buff[3]= (length>>24)&0xff;
	pwm_set_dma_address(&ir_dma_send_buff);

	pwm_set_interrupt_enable(PWM_IRQ_PWM0_IR_DMA_FIFO_DONE);
	irq_set_mask(FLD_IRQ_SW_PWM_EN);
	pwm_start_dma_ir_sending();

/*****************************************************************************************************************************/
#elif (MODE_SELECT == IR_LEARN_FOR_EDGE_DETECT_MODE)
#if(GPIO_IRQ_TYPE == RISING_EDGE)
	gpio_setup_up_down_resistor(IRLEARN_RX_PIN,PM_PIN_PULLDOWN_100K);
	ir_learn_set_mode(IL_EN_LOW_STATUS_START_CNT);
	ir_learn_set_irq_mask(RISING_EDGE_IRQ);
#elif (GPIO_IRQ_TYPE == FALLING_EDGE)
	gpio_setup_up_down_resistor(IRLEARN_RX_PIN,PM_PIN_PULLUP_10K);
	ir_learn_set_mode(IL_EN_HIGH_STATUS_START_CNT);
	ir_learn_set_irq_mask(FALLING_EDGE_IRQ);
#endif
	ir_learn_en();
#endif
	irq_enable();
}


/**
 * @brief		This function serves to handle the interrupt of MCU
 * @param[in] 	none
 * @return 		none
 */
_attribute_ram_code_sec_noinline_ void irq_handler(void)
{
#if (MODE_SELECT == IR_LEARN_MODE)

	if( (reg_irq_src & FLD_IRQ_IR_LEARN_EN) != 0)
	{
		if( ir_learn_get_irq_status(TIMEOUT_IRQ))
		{
			ir_learn_clr_irq_status(TIMEOUT_IRQ);
			gpio_toggle(LED4);
		}
		if( ir_learn_get_irq_status(RISING_EDGE_IRQ) )
		{
			ir_learn_clr_irq_status(RISING_EDGE_IRQ);
			ir_irq_cnt++;
			gpio_toggle(LED3);
			il_wave_receive_buff[il_index] = ir_learn_get_high();
			il_wave_receive_buff[il_index+1] = ir_learn_get_cycle();
			il_index += 2;
		}
	}

#elif (MODE_SELECT == IR_LEARN_FOR_EDGE_DETECT_MODE)
#if(GPIO_IRQ_TYPE == RISING_EDGE)
	if( (reg_irq_src & FLD_IRQ_IR_LEARN_EN) != 0){
		if( ir_learn_get_irq_status(RISING_EDGE_IRQ) )
		{
			ir_learn_clr_irq_status(RISING_EDGE_IRQ);
			gpio_toggle(LED1);
			ir_rising_cnt +=1;
		}
	}
#elif (GPIO_IRQ_TYPE == FALLING_EDGE)
	if( (reg_irq_src & FLD_IRQ_IR_LEARN_EN) != 0){
		if( ir_learn_get_irq_status(FALLING_EDGE_IRQ) )
		{
			ir_learn_clr_irq_status(FALLING_EDGE_IRQ);
			gpio_toggle(LED1);
			ir_falling_cnt +=1;
		}
	}
#endif

#endif
}


/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////

void main_loop(void)
{

}
