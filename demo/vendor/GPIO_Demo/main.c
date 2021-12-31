/********************************************************************************************************
 * @file	main.c
 *
 * @brief	This is the source file for b85m
 *
 * @author	Driver Group
 * @date	2018
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
#include "calibration.h"


extern void user_init();
extern void main_loop (void);
volatile unsigned int gpio_irq_cnt;
volatile unsigned int gpio_set_irq_cnt;
/**
 * @brief		This function serves to handle the interrupt of MCU
 * @param[in] 	none
 * @return 		none
 */
_attribute_ram_code_sec_noinline_ void irq_handler(void)
{
#if (GPIO_MODE == GPIO_IRQ )

	if((reg_irq_src & FLD_IRQ_GPIO_EN)==FLD_IRQ_GPIO_EN){
		reg_irq_src |= FLD_IRQ_GPIO_EN; // clear the relevant irq
			gpio_irq_cnt++;
			gpio_toggle(LED1);
	}

#elif(GPIO_MODE == GPIO_IRQ_RSIC0)

	if((reg_irq_src & FLD_IRQ_GPIO_RISC0_EN)==FLD_IRQ_GPIO_RISC0_EN){
		reg_irq_src |= FLD_IRQ_GPIO_RISC0_EN; // clear the relevant irq
			gpio_irq_cnt++;
			gpio_toggle(LED1);
	}

#elif(GPIO_MODE == GPIO_IRQ_RSIC1)

	if((reg_irq_src & FLD_IRQ_GPIO_RISC1_EN)==FLD_IRQ_GPIO_RISC1_EN){
		reg_irq_src |= FLD_IRQ_GPIO_RISC1_EN; // clear the relevant irq

			gpio_irq_cnt++;
			gpio_toggle(LED1);
	}
#elif((GPIO_MODE == GPIO_SEL_IRQ_SRC)&&(MCU_CORE_B80))
	static unsigned char gpio_irqsrc;
	gpio_irqsrc =(reg_gpio_irq_from_pad&SET_GROUP_GPIO);
	if(gpio_irqsrc)
	{
		reg_gpio_irq_from_pad|=SET_GROUP_GPIO;

			gpio_irq_cnt++;
			gpio_toggle(LED1);
	}
#endif

}

/**
 * @brief		This is main function
 * @param[in]	none
 * @return      none
 */
int main (void)   //must on ramcode
{

#if(MCU_CORE_B80||MCU_CORE_B89)
	cpu_wakeup_init(EXTERNAL_XTAL_24M);
#elif (MCU_CORE_B85)
	cpu_wakeup_init();
#elif (MCU_CORE_B87)
	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

#endif

#if (MCU_CORE_B85) || (MCU_CORE_B87)
	//Note: This function must be called, otherwise an abnormal situation may occur.
	//Called immediately after cpu_wakeup_init, set in other positions, some calibration values may not take effect.
	user_read_flash_value_calib();
#elif (MCU_CORE_B89)
	//Note: This function must be called, otherwise an abnormal situation may occur.
	//Called immediately after cpu_wakeup_init, set in other positions, some calibration values may not take effect.
	user_read_otp_value_calib();
#endif

	clock_init(SYS_CLK);

	gpio_init(0);

	user_init();

	while (1) {
		main_loop ();
	}
	return 0;
}
