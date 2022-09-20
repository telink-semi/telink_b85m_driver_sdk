/********************************************************************************************************
 * @file	calibration.h
 *
 * @brief	This is the header file for B85m
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
#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

#include "driver.h"


#ifndef OTP_ADC_VREF_CALIB_ADDR
#define OTP_ADC_VREF_CALIB_ADDR			0x7f18
#endif

#ifndef OTP_CAP_VALUE_ADDR
#define OTP_CAP_VALUE_ADDR				0x7f10
#endif

#ifndef OTP_VDDF_CALIB_ADDR
#define OTP_VDDF_CALIB_ADDR				0x7f11
#endif

#ifndef FLASH_ADC_VREF_CALIB_ADDR_64K
#define FLASH_ADC_VREF_CALIB_ADDR_64K	0xe0c0
#endif

#ifndef FLASH_ADC_VREF_CALIB_ADDR_128K
#define FLASH_ADC_VREF_CALIB_ADDR_128K	0x1e0c0
#endif

#ifndef FLASH_ADC_VREF_CALIB_ADDR_512K
#define FLASH_ADC_VREF_CALIB_ADDR_512K	0x770c0
#endif

#ifndef FLASH_ADC_VREF_CALIB_ADDR_1M
#define FLASH_ADC_VREF_CALIB_ADDR_1M	0xfe0c0
#endif

#ifndef FLASH_ADC_VREF_CALIB_ADDR_2M
#define FLASH_ADC_VREF_CALIB_ADDR_2M	0x1fe0c0
#endif

#ifndef FLASH_CAP_VALUE_ADDR_64K
#define FLASH_CAP_VALUE_ADDR_64K		0xe000
#endif

#ifndef FLASH_CAP_VALUE_ADDR_128K
#define FLASH_CAP_VALUE_ADDR_128K		0x1e000
#endif

#ifndef FLASH_CAP_VALUE_ADDR_512K
#define FLASH_CAP_VALUE_ADDR_512K		0x77000
#endif

#ifndef FLASH_CAP_VALUE_ADDR_1M
#define FLASH_CAP_VALUE_ADDR_1M			0xfe000
#endif

#ifndef FLASH_CAP_VALUE_ADDR_2M
#define FLASH_CAP_VALUE_ADDR_2M			0x1fe000
#endif

#ifndef FLASH_VDDF_CALIB_ADDR_64K
#define FLASH_VDDF_CALIB_ADDR_64K		0xe1c0
#endif

#ifndef FLASH_VDDF_CALIB_ADDR_128K
#define FLASH_VDDF_CALIB_ADDR_128K		0x1e1c0
#endif

#ifndef FLASH_VDDF_CALIB_ADDR_512K
#define FLASH_VDDF_CALIB_ADDR_512K		0x771c0
#endif

#ifndef FLASH_VDDF_CALIB_ADDR_1M
#define FLASH_VDDF_CALIB_ADDR_1M		0xfe1c0
#endif

#ifndef FLASH_VDDF_CALIB_ADDR_2M
#define FLASH_VDDF_CALIB_ADDR_2M		0x1fe1c0
#endif

/**
 * @brief	flash voltage definition
 */
typedef enum {
    USER_CALIB_FROM_FLASH	= 0x01,
    USER_CALIB_FROM_OTP		= 0x02,
}user_calib_from_e;


/**
 * @brief		This function is used to calibrate the user's parameters.
 * 				This function is to read the calibration value stored in otp,
 * 				and use the calibration value to configure the chip to improve chip performance.
 * 				(reduce adc measurement error, reduce frequency offset, etc.)
 * @return		none.
 */
void user_read_otp_value_calib(void);


#endif

