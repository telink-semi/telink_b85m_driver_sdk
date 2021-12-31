/********************************************************************************************************
 * @file	qdec.h
 *
 * @brief	This is the header file for b80
 *
 * @author	Driver Group
 * @date	2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#pragma once

#include "clock.h"
#include "lib/include/pm.h"
#include "analog.h"
#include "register.h"

/**
 * qedc input channel type of A
 */
typedef enum
{
	PA2A,
	PA3A,
	PB6A,
	PB7A,
	PC2A,
	PC3A,
	PD6A,
	PD7A,

}QDEC_InputAchTypeDef;

/**
 * qedc input channel type of A
 */
typedef enum
{
	PA2B,
	PA3B,
	PB6B,
	PB7B,
	PC2B,
	PC3B,
	PD6B,
	PD7B,
}QDEC_InputBchTypeDef;


/**
 * qedc mode
 */
typedef enum
{
	COMMON_MODE,
	DOUBLE_ACCURACY_MODE,
}qdec_mode_e;


/**
 *the hardware debouncing threshold
 */
typedef enum{
	QDEC_THRSH_P187p5us =0,
	QDEC_THRSH_P375p0us,
    QDEC_THRSH_P750p0us,
	QDEC_THRSH_P1500p0us,
	QDEC_THRSH_P3000p0us,
	QDEC_THRSH_P6000p0us,
	QDEC_THRSH_P12000p0us,
	QDEC_THRSH_P24000p0us,
}qdec_thrsh_e;

/**
 * @brief      This function servers to set input port.
 * @param[in]  chn_a - input types of a channel.
 * @param[in]  chn_b - input types of b channel.
 * @return     none.
 */
void qdec_set_pin(QDEC_InputAchTypeDef chn_a,QDEC_InputBchTypeDef chn_b);

/**
 * @brief      This function servers to set qdec mode,qdec mode:common mode and double accuracy mode.
 *             common mode:the qdec counter value is increased/decreased by 1 only when the same rising/falling edges are detected from the two phase signals.
 *             double accuracy mode:the qdec counter value is increased/decreased by 1 on each rising/falling edge of the two phase signals;
 *             the counter value will be increased/decreased by 2 for one wheel rolling.
 * @param[in]  mode - mode type to select.
 * @return     none.
 */
void qdec_set_mode(qdec_mode_e mode);

/**
 * @brief      This function servers to initials qedc source clock.
 * @param[in]  none.
 * @return     none.
 */
void qdec_clk_en(void);

/**
 * @brief      This function servers to read hardware counting value,
 *             After reading the value, if there is no input to channels A and B, then reading the value is 0 through this function,
 *             If the count is not read, the qdec read real time counting value increases or decreases with the direction of the wheel according to the qdec mode.
 * @param[in]  none.
 * @return     hardware counting value.
 */
signed char qdec_get_count_value(void);

/**
 * @brief      This function servers to reset qdec and the qdec counter value is cleared zero.
 * @param[in]  none.
 * @return     none.
 */
void qdec_reset(void);

/**
 * @brief      This function servers to set hardware debouncing.
 * @param[in]  thrsh - any signal with width lower than the threshold will be regarded as jitter,
 *             effective signals input from Channel A and B should contain high/low level with width more than the threshold.
 * @return     none.
 */
void qdec_set_debouncing(qdec_thrsh_e thrsh);
