/********************************************************************************************************
 * @file	clock.h
 *
 * @brief	This is the header file for B89
 *
 * @author	Driver Group
 * @date	2020
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#pragma once


#include "compiler.h"
#include "register.h"


#define 	_ASM_NOP_			asm("tnop")

/**
 * @brief  instruction delay.
 */
#define		CLOCK_DLY_1_CYC    _ASM_NOP_
#define		CLOCK_DLY_2_CYC    _ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_3_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_4_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_5_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_6_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_7_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_8_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_9_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_10_CYC   _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_

/**
 * @brief select RC as system clock
 */
#ifndef SYSCLK_RC_CLOCK_EN
#define SYSCLK_RC_CLOCK_EN			0
#endif

extern unsigned int system_clk_type;


/**
 * @brief 	external XTAL type,
 * 			0- 24M , 1-48M
 */
#define  CLK_48M_XTAL_EN   			0

#define  VULTURE_A0   				0

/**
 * @brief 	crystal for different application
 */
typedef enum{
	EXTERNAL_XTAL_24M	= 0,
	EXTERNAL_XTAL_48M	= 1,
}XTAL_TypeDef;
/**
 * @brief system clock type, to optimize the code and reduce the code size, add bit8-bit10,
 * |                  |               |
 * |:---------------  | :------------ |
 * |     <10:8>       |    <7:0>      |
 * |reg_otp_ctrl2<7:5>|      clk      |
 */
typedef enum{
	SYS_CLK_4M_Crystal  =  0x01<<8|0xc6,
	SYS_CLK_6M_Crystal  =  0x01<<8|0xc4,
	SYS_CLK_8M_Crystal  =  0x01<<8|0xc3,
	SYS_CLK_12M_Crystal =  0x02<<8|0xc2,
	SYS_CLK_16M_Crystal =  0x02<<8|0xe0,
	SYS_CLK_24M_Crystal =  0x04<<8|0xa0,

//	SYS_CLK_RC_THRES = 0x10,

//	SYS_CLK_24M_RC 	 = 0x00,
//	SYS_CLK_32M_RC 	 = 0x01,
//	SYS_CLK_48M_RC 	 = 0x02,
}SYS_CLK_TypeDef;

/**
 * @brief 32K clock type.
 */

typedef enum{
	CLK_32K_RC   =0,
	CLK_32K_XTAL =1,
}CLK_32K_TypeDef;

/**
 * @brief       This function to select the system clock source.
 * @param[in]   SYS_CLK - the clock source of the system clock.
 * @return      none
 * @note		1. Do not switch the clock during the DMA sending and receiving process because during the clock switching process, 
 * 					the system clock will be suspended for a period of time, which may cause data loss.
 * 				2. When this function called after power on or deep sleep wakeup, it will perform 24m rc calibration. 
 * 					The usage rules of 24m rc, please refer to the rc_24m_cal() for details.
 * 					If do not want this logic, you can check the usage and precautions of clock_init_calib_24m_rc_cfg().
 */
void clock_init(SYS_CLK_TypeDef SYS_CLK);

/**
 * @brief       This function to get the system clock source.
 * @param[in]   none
 * @return      system clock source
 */

static inline int clock_get_system_clk(void)
{
	return system_clk_type;
}

/**
 * @brief   This function serves to set 32k clock source.
 * @param   variable of 32k type.
 * @return  none.
 */
void clock_32k_init (CLK_32K_TypeDef src);

/**
 * @brief     This function performs to select 24M as the system clock source.
 * @param[in] none.
 * @return    none.
 * @note	  During the first power-on, after the xtal is stable (cpu_wakeup_init()), it is necessary to calibrate the 24m rc as soon as possible 
 * 				to prevent some unknown problems caused by a large frequency deviation of the RC clock.
 *            1. If the sleep function is not used and the accuracy of 24m rc is not high, then there is no need for regular calibration.
 *            2. If the sleep wake-up function is required, it is necessary to calibrate the 24m rc before the first sleep, otherwise it may cause the 
 * 					oscillator to fail to start after waking up.The recommended interval for regular calibration is 10 seconds. 
 *            3. If the 24m rc is more accurate, the oscillator will start up faster after waking up. If it is not accurate, the oscillator may not start
 * 					up after waking up.Therefore, regular calibration is needed to prevent the impact of temperature changes.
 */
void rc_24m_cal (void);

/**
 * @brief     This function performs to select 48M RC as the system clock source.
 * @param[in] none.
 * @return    none.
 */
void rc_48m_cal (void);

/**
 * @brief     This function performs to select 32K as the system clock source.
 * @param[in] none.
 * @return    none.
 * @note	  1. If a more accurate 32K RC timing is required, then to prevent temperature effects, calibration can be performed regularly.
 * 			  2. If it is to ensure accurate sleep time, then the 32K RC calibration is not necessary. Although sleep time is measured by 32K RC, 
 * 				    sleep time is obtained through tracking way and will not affected by 32K RC deviation. So in this scenario, it is necessary to 
 * 				    calibrate once when power-on (to prevent significant frequency deviation caused by 32K RC), and regular calibration is not necessary.
 */
void rc_32k_cal (void);

/**
 * @brief     This function performs to select 24M/2 RC as source of DMIC.
 * @param[in] source clock to provide DMIC.
 * @return    none.
 */
void dmic_prob_48M_rc(void);

/**
 * @brief 	  This function performs to configure whether to calibrate the 24m rc in the clock_init() when power-on or wakeup from deep sleep mode.
 * 				If wakeup from deep retention sleep mode will not calibrate.
 * @param[in] calib_flag - Choose whether to calibrate the 24m rc or not.
 * 						1 - calibrate; 0 - not calibrate
 * @return	  none
 * @note	  This function will not take effect until it is called before clock_init(). 
 */
void clock_init_calib_24m_rc_cfg(char calib_flag);
