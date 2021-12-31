/********************************************************************************************************
 * @file	app_ecdh.c
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
/********* pke version:1.1 *********/
#include "app_config.h"

#if(PKE_TEST_MODE == PKE_ECDH)
#define ECDH_SIGN_VERIFY_ROUND1  (2)



unsigned int ecdh_test_internal(eccp_curve_t *curve, char *curve_name, unsigned char *local_prikey, unsigned char *local_pubkey,
		unsigned char *peer_prikey, unsigned char *peer_pubkey, unsigned char *std_key, unsigned int keyByteLen, KDF_FUNC kdf)
{
	unsigned char local_key[200];
	unsigned char peer_key[200];
	unsigned char tmp[200];
	unsigned int ret;

//	printf("\r\n\r\n ------------- %s ECDH standard data test ------------- ", curve_name);


	//standard inputs
	ret = ecdh_compute_key(curve, local_prikey, peer_pubkey, local_key,  keyByteLen, kdf);
	if(ECDH_SUCCESS != ret)
	{
		printf("\r\n %s ECDH std test error 1, ret = %02x", curve_name, ret);
		return 1;
	}

	ret = ecdh_compute_key(curve, peer_prikey, local_pubkey, peer_key,  keyByteLen, kdf);
	if(ECDH_SUCCESS != ret || memcmp(local_key,std_key,keyByteLen) || memcmp(local_key,peer_key,keyByteLen))
	{
		printf("\r\n %s ECDH std test error 2, ret = %02x", curve_name, ret);
		return 1;
	}


	//corner case: peer pubkey all zero
	memset(tmp, 0, 200);
	ret = ecdh_compute_key(curve, local_prikey, tmp, local_key,  keyByteLen, kdf);
	if(PKE_SUCCESS == ret)
	{
		printf("\r\n %s ECDH corner case test error 1, ret = %02x", curve_name, ret);
		return 1;
	}

	//corner case: peer pubkey corrupted
	memcpy(tmp, peer_pubkey, 2*GET_BYTE_LEN(curve->eccp_p_bitLen));
	tmp[0]+=1;
	ret = ecdh_compute_key(curve, local_prikey, tmp, local_key,  keyByteLen, kdf);
	if(PKE_SUCCESS == ret)
	{
		printf("\r\n %s ECDH corner case test error 2, ret = %02x", curve_name, ret);
		return 1;
	}

	//corner case: local_prikey is 0
	memset(tmp, 0, 200);
	ret = ecdh_compute_key(curve, tmp, peer_pubkey, local_key,  keyByteLen, kdf);
	if(PKE_SUCCESS == ret)
	{
		printf("\r\n %s ECDH corner case test error 3, ret = %02x", curve_name, ret);
		return 1;
	}

	//corner case: local_prikey is n
	reverse_byte_array((unsigned char *)curve->eccp_n, (unsigned char *)tmp, GET_BYTE_LEN(curve->eccp_n_bitLen));
	ret = ecdh_compute_key(curve, tmp, peer_pubkey, local_key,  keyByteLen, kdf);
	if(PKE_SUCCESS == ret)
	{
		printf("\r\n %s ECDH corner case test error 4, ret = %02x", curve_name, ret);
		return 1;
	}

	//corner case: local_prikey is n+1
	reverse_byte_array((unsigned char *)curve->eccp_n, (unsigned char *)tmp, GET_BYTE_LEN(curve->eccp_n_bitLen));
	tmp[GET_BYTE_LEN(curve->eccp_n_bitLen)-1]+=1;
	ret = ecdh_compute_key(curve, tmp, peer_pubkey, local_key,  keyByteLen, kdf);
	if(PKE_SUCCESS == ret)
	{
		printf("\r\n %s ECDH corner case test error 5, ret = %02x", curve_name, ret);
		return 1;
	}

	printf("\r\n %s ECDH std & corner_case test success", curve_name);

	return 0;
}



#if (ECCP_MAX_BIT_LEN >= 160)
unsigned int ecdh_std_test_brainpoolp160r1(void)
{
	unsigned char local_prikey[20]={
		0x62,0x31,0xB2,0xC9,0xF5,0x85,0x18,0x56,0x01,0xF5,0x9F,0x27,0xC0,0xC0,0x09,0x2C,
		0x18,0xA8,0xC5,0x86,};
	unsigned char local_pubkey[40]={
		0x7E,0x47,0x71,0x0A,0x1D,0xF2,0xAD,0xCB,0xD9,0x87,0x05,0xD3,0xDD,0x8E,0x51,0x42,
		0x3F,0xA6,0xDC,0x77,0x5A,0x12,0x42,0x67,0xFC,0xC8,0x30,0x05,0xAF,0xB7,0x0B,0x9D,
		0x14,0x88,0xD6,0x1A,0x55,0xB2,0xB6,0xC1,};
	unsigned char peer_prikey[20]={
		0xCA,0x2A,0x62,0x0F,0x4D,0x41,0xA6,0xC3,0x54,0x36,0xB5,0x01,0x53,0xA7,0x38,0x81,
		0xAB,0xB2,0xDD,0x56,};
	unsigned char peer_pubkey[40]={
		0xA6,0xD4,0x50,0x1B,0xCF,0x85,0xED,0xA4,0xDA,0x3C,0xAD,0x55,0x52,0x18,0x26,0x58,
		0x57,0x0B,0xDA,0x09,0xAD,0x02,0xE3,0xE8,0xDF,0xC7,0x0F,0x1D,0xA3,0x50,0x2F,0x36,
		0x9D,0xD8,0xDB,0xC4,0x9E,0x3F,0xAE,0x2F,};
	unsigned char std_key[20]={
		0xD6,0x26,0xA5,0xC5,0x30,0xBA,0x97,0x27,0xB5,0xBB,0x57,0x19,0x92,0x04,0xE0,0x02,
		0xE7,0xA1,0x33,0xBD};
	unsigned int keyByteLen = 20;
	unsigned int ret;

	ret = ecdh_test_internal(brainpoolp160r1, "brainpoolp160r1", local_prikey, local_pubkey,
				  peer_prikey, peer_pubkey, std_key, keyByteLen, NULL);
	if(ret)
		return 1;

	return 0;
}
#endif


#if (ECCP_MAX_BIT_LEN >= 192)
unsigned int ecdh_std_test_secp192r1(void)
{
	unsigned char local_prikey[24]={
		0x66,0x7F,0x88,0xD1,0xB6,0x78,0x47,0x6E,0x86,0xCA,0x5D,0x8F,0xDB,0x8C,0xD9,0xDB,
		0xDF,0x90,0xCF,0xF4,0x31,0x55,0x27,0x38,};
	unsigned char local_pubkey[48]={
		0xBB,0xED,0xE4,0xD0,0x9B,0x6D,0xBD,0xB3,0x43,0xCE,0x04,0x0F,0x5D,0x83,0x1C,0xE3,
		0xC4,0x1F,0x1E,0x76,0xD7,0xE9,0x32,0xB8,0x90,0xBC,0x04,0x29,0x3E,0xF5,0x48,0x4E,
		0xC0,0x12,0x74,0xBA,0xD8,0xD3,0x34,0x56,0x36,0x1A,0xD1,0x37,0xCF,0xA8,0xD3,0x21,};
	unsigned char peer_prikey[24]={
		0x9E,0x44,0x8E,0xB5,0x88,0x86,0xA0,0x23,0x93,0x7E,0x25,0x6A,0x84,0x65,0x10,0x82,
		0x6F,0xE4,0x29,0x12,0xD9,0x2B,0x4C,0xB3,};
	unsigned char peer_pubkey[48]={
		0xF7,0x98,0x0A,0xF3,0x75,0x26,0xD5,0x96,0x88,0xE1,0x0E,0xF7,0x20,0xEB,0x1A,0xAF,
		0xBD,0x9C,0xAE,0x6E,0x0A,0xD8,0x30,0x6E,0xC7,0xCB,0x0A,0x51,0xF5,0xB9,0x4D,0x7E,
		0x78,0x53,0x71,0x1F,0x62,0xC3,0x3A,0x2B,0xCB,0xD7,0x96,0x77,0x1F,0x56,0x88,0x3F,};
	unsigned char std_key[24]={
		0x63,0x78,0x19,0xC8,0xCE,0x76,0x0E,0xC3,0x73,0x6E,0xC5,0x53,0x46,0xF8,0x02,0x30,
		0x07,0x64,0xFA,0x15,0x68,0x9B,0xA2,0x2F,};
	unsigned int keyByteLen = 24;
	unsigned int ret;

	ret = ecdh_test_internal(secp192r1, "secp192r1", local_prikey, local_pubkey,
				  peer_prikey, peer_pubkey, std_key, keyByteLen, NULL);
	if(ret)
		return 1;

	return 0;
}
#endif


#if (ECCP_MAX_BIT_LEN >= 224)
unsigned int ecdh_std_test_secp224r1(void)
{
	unsigned char local_prikey[28]={
			0x01,0x6B,0xBE,0x37,0xA1,0x54,0xC6,0xB8,0x65,0x66,0x86,0x80,0xDA,0xAF,0x6D,0x7F,
			0x33,0x89,0xFC,0x85,0xB2,0x6B,0x5D,0x62,0x2C,0x8F,0x5D,0x8B,};
	unsigned char local_pubkey[56]={
			0xD2,0xCB,0x42,0xE9,0xE4,0xD4,0x5D,0x7A,0x66,0xCC,0x3B,0xA2,0x06,0x2A,0x0E,0x71,
			0x62,0xA7,0x6D,0x7F,0x54,0x68,0x18,0xB4,0x78,0xEF,0xEE,0xA4,0x34,0x23,0x82,0x73,
			0xEB,0xD7,0x01,0x43,0x02,0xED,0xF8,0x69,0x37,0x61,0xB5,0x58,0x0A,0xBE,0xD7,0x26,
			0x1B,0x42,0x5A,0xEA,0x54,0x92,0x6A,0xFF,};
	unsigned char peer_prikey[28]={
			0xBE,0xD9,0x7E,0x69,0x72,0xD7,0xD0,0x71,0xF8,0x6C,0xC8,0x06,0x25,0x8E,0x2A,0x03,
			0x5C,0xAC,0x82,0x44,0x3B,0x46,0xCE,0x6D,0xA5,0x2D,0xB0,0xA6,};
	unsigned char peer_pubkey[56]={
			0x97,0x3C,0xDD,0x74,0x51,0x9C,0xD6,0x9E,0x12,0x46,0x7C,0x32,0x11,0x71,0x95,0x33,
			0x16,0x10,0xFB,0x2D,0xE5,0x7B,0x42,0x5D,0x51,0x6A,0x88,0x1C,0x72,0xCD,0x47,0x52,
			0x2F,0x47,0x82,0xA2,0x03,0xCE,0x97,0xEA,0x27,0x6D,0x7F,0x83,0x2B,0x14,0x31,0xEE,
			0x38,0xAB,0xDC,0xC1,0xC8,0x91,0xD7,0xB9,};
	unsigned char std_key[28]={
			0x9A,0x34,0x27,0x63,0x84,0xD7,0x88,0x9B,0x91,0xF6,0xC2,0x31,0x9B,0x35,0x42,0xD8,
			0x83,0xE5,0x4C,0x5A,0x7F,0xF3,0x87,0x81,0x15,0xEC,0x8D,0xBC,};
	unsigned int keyByteLen = 28;
	unsigned int ret;

	ret = ecdh_test_internal(secp224r1, "secp224r1", local_prikey, local_pubkey,
				  peer_prikey, peer_pubkey, std_key, keyByteLen, NULL);
	if(ret)
		return 1;

	return 0;
}
#endif


#if (ECCP_MAX_BIT_LEN >= 256)
unsigned int ecdh_std_test_secp256r1(void)
{
	unsigned char local_prikey[32]={
			0xF4,0x3B,0xAF,0xC3,0xC7,0xB2,0xFB,0x10,0xAF,0xEE,0xD6,0x56,0x68,0xD3,0x63,0x4E,
			0x32,0x39,0xC1,0x08,0xBB,0x6A,0xCF,0x34,0x33,0x31,0x49,0x02,0x55,0x84,0xB3,0x0E,
	};
	unsigned char local_pubkey[64]={
			0xBD,0x2F,0xEF,0x24,0xAC,0x63,0x6F,0x55,0xDB,0xF1,0xF5,0x20,0x38,0x85,0x09,0x7E,
			0x18,0x62,0x9E,0xDF,0xFB,0x36,0xF4,0x03,0x89,0x61,0x27,0x44,0xA1,0x0F,0x02,0xD0,
			0x6B,0x51,0x9E,0x0F,0x82,0xBB,0x90,0xD1,0x63,0x3D,0x07,0xE8,0x70,0x8F,0x65,0x95,
			0xCE,0xBF,0xF3,0x86,0x6C,0xFE,0x33,0xF3,0xCB,0xE8,0xA3,0x68,0x1E,0xB8,0x20,0xEA,
	};
	unsigned char peer_prikey[32]={
			0x45,0x97,0x46,0x63,0xBE,0x4A,0xEE,0x88,0xE1,0xFF,0xD5,0x8B,0xC9,0x28,0xB0,0xFE,
			0xF9,0x18,0x16,0x3A,0xD5,0x39,0x9C,0x99,0x99,0x7A,0x62,0x09,0xAB,0xB4,0xA6,0x82,
	};
	unsigned char peer_pubkey[64]={
			0x03,0xE3,0x11,0x49,0xDA,0x81,0x57,0x00,0x19,0x19,0xC5,0xFB,0x64,0xEF,0xD5,0x2F,
			0x2A,0x62,0xEF,0xA9,0xB3,0x10,0xB8,0xB2,0xEA,0xA5,0x71,0x6B,0x2B,0x83,0xAD,0xA5,
			0x8B,0x73,0x43,0xCC,0x6E,0xF6,0x53,0xC2,0x91,0x03,0x57,0xE0,0xDC,0x7A,0x8C,0x0E,
			0xD5,0x64,0xCD,0x20,0x87,0x30,0x84,0x72,0xE5,0xC5,0x23,0x6C,0x4C,0xEB,0xF6,0x27,
	};

	unsigned char std_key[32]={
			0x09,0xB9,0x29,0x4B,0xA4,0x68,0x6A,0xDC,0xE2,0x72,0x77,0xE6,0x3D,0x87,0x59,0x8E,
			0x94,0xBB,0x1F,0x9C,0x44,0xF7,0xB9,0xBB,0x27,0x96,0x3D,0x0F,0x4E,0xAC,0x86,0x1D};
	unsigned int keyByteLen = 32;
	unsigned int ret;

	ret = ecdh_test_internal(secp256r1, "secp256r1", local_prikey, local_pubkey,
				  peer_prikey, peer_pubkey, std_key, keyByteLen, NULL);
	if(ret)
		return 1;

	return 0;
}
#endif


#if (ECCP_MAX_BIT_LEN >= 384)
unsigned int ecdh_std_test_secp384r1(void)
{
	unsigned char local_prikey[48]={
			0x29,0xAE,0x2E,0xE6,0xF9,0x83,0x86,0xD9,0x3E,0x9A,0x12,0x0D,0x40,0xD0,0x1D,0x57,
			0x4D,0x6A,0x0B,0x30,0x69,0xF3,0xF0,0xF1,0x1C,0xC1,0xF1,0x5A,0xE2,0xEB,0x54,0xD4,
			0x04,0x42,0x9E,0xAF,0xFF,0x6D,0xEA,0x18,0x62,0xC6,0x2D,0x6E,0x32,0x22,0x22,0x86,
	};
	unsigned char local_pubkey[96]={
			0x96,0xE5,0x48,0x36,0x01,0x1F,0x13,0x1E,0xEA,0x26,0x7A,0xB4,0xD7,0x29,0x36,0x46,
			0xBC,0x09,0x0D,0x67,0x58,0x63,0x9D,0xB2,0x57,0xA7,0x70,0x5F,0x4F,0xB8,0xEC,0x18,
			0x37,0x11,0x4E,0x4F,0xED,0x5A,0xA6,0x01,0x1E,0xF2,0x6E,0x7F,0x96,0x74,0xE5,0x49,
			0x51,0xD2,0x41,0x43,0x0F,0xB7,0x92,0x0C,0x3A,0x7A,0x64,0x0E,0xB4,0xD1,0x84,0xC9,
			0xA4,0x7A,0xD3,0x9F,0x34,0xC8,0x68,0x62,0xF0,0x21,0xE1,0x4C,0xCF,0xB1,0x4D,0x82,
			0xD2,0xB9,0xD3,0x90,0x9B,0x91,0x84,0x0A,0x29,0xCB,0xD6,0xF1,0xFA,0xF9,0x73,0xC1,
	};
	unsigned char peer_prikey[48]={
			0x39,0x96,0x4A,0x0A,0x71,0x4F,0x65,0xD8,0x47,0x1F,0xF6,0x0F,0x53,0xD6,0xE5,0xA7,
			0x75,0x36,0x92,0x41,0x38,0x83,0xEF,0x73,0xB9,0x78,0x19,0xAC,0xD6,0x46,0x48,0xFD,
			0xC6,0xE7,0x03,0x21,0xBC,0x47,0x9E,0x4E,0x43,0xEE,0x37,0x41,0x24,0x11,0x46,0x0C,
	};
	unsigned char peer_pubkey[96]={
			0xF9,0x3F,0xA9,0x79,0x58,0xA4,0xC3,0x57,0x5A,0x2B,0x61,0x5E,0x39,0x74,0x11,0xBF,
			0x24,0xCB,0xC3,0x2D,0x44,0x44,0xDD,0x72,0x17,0x0A,0x2C,0x29,0x3B,0x14,0x25,0xB6,
			0x75,0xFA,0x59,0x8F,0x5B,0x24,0x5F,0xD8,0x8F,0x89,0xCA,0xF9,0x83,0xF6,0x2E,0x1F,
			0x69,0xE1,0x90,0x34,0xDC,0x9C,0x33,0x29,0x60,0x66,0x00,0x97,0x63,0x90,0xA5,0x55,
			0x18,0xF0,0x18,0x56,0x32,0x37,0x70,0xFB,0x0C,0xED,0x8D,0x4D,0x90,0x86,0x40,0x09,
			0x82,0x0A,0x1A,0xD1,0x9F,0x82,0x6D,0x47,0xC4,0xDC,0x19,0x30,0x8A,0xB1,0x0C,0xE8,
	};

	unsigned char std_key[48]={
			0xD2,0x9D,0xB0,0x44,0x17,0xA5,0xA5,0x8C,0xA2,0x68,0x28,0xCA,0xE1,0x7C,0x00,0xF3,
			0xE2,0xE2,0x31,0x44,0x1E,0xF2,0xC4,0x53,0xDF,0x38,0xB4,0x33,0xAD,0x93,0x9C,0xF9,
			0x71,0x38,0xD2,0xAF,0xE5,0x3D,0x70,0x3D,0x3C,0x6E,0x64,0x1F,0xAA,0x6D,0xA7,0xD7,};
	unsigned int keyByteLen = 48;
	unsigned int ret;

	ret = ecdh_test_internal(secp384r1, "secp384r1", local_prikey, local_pubkey,
				  peer_prikey, peer_pubkey, std_key, keyByteLen, NULL);
	if(ret)
		return 1;

	return 0;
}
#endif


#if (ECCP_MAX_BIT_LEN >= 512)
unsigned int ecdh_std_test_brainpoolp512r1(void)
{
	unsigned char local_prikey[64]={
		0x15,0xFE,0xF6,0xF9,0xD6,0x9D,0xD2,0x6D,0xD3,0xF7,0xD9,0xD4,0xD8,0xBB,0x92,0x33,
		0x6B,0xF9,0x37,0x05,0xD0,0x23,0xB7,0xCB,0xA6,0xCB,0xE4,0x2A,0x09,0x56,0x84,0xBB,
		0x8C,0x05,0xD3,0x35,0xAF,0x58,0x70,0x07,0x27,0x3C,0x55,0x7B,0xC0,0x1B,0xA5,0x01,
		0x36,0xC0,0xB5,0x1B,0x4E,0xA5,0x40,0xFA,0xEC,0x8A,0xB1,0x8F,0xB0,0x05,0x1F,0x42,
	};
	unsigned char local_pubkey[128]={
		0x10,0x27,0xAA,0x4B,0x6B,0x18,0x0D,0x7C,0xC6,0xA4,0x4D,0x76,0x95,0x52,0x1C,0x69,
		0x26,0xF9,0x47,0x66,0x48,0x35,0x3E,0x6C,0xEB,0x41,0x44,0xB8,0x0F,0x67,0x7D,0xFB,
		0xEF,0x54,0x92,0x01,0x34,0x77,0x0E,0xEF,0xCF,0x55,0x93,0x8D,0x78,0xB8,0xBA,0x09,
		0x8C,0x8C,0x4D,0x9F,0xC3,0x91,0xA5,0x85,0x51,0x62,0xD9,0x9D,0x1C,0x11,0x66,0x81,
		0x40,0xA3,0xD1,0x5D,0xDD,0xE0,0x12,0xF3,0x77,0xD0,0xE8,0x02,0xE2,0x6C,0x64,0x5F,
		0xB4,0xB6,0xE8,0x93,0x51,0xE3,0xC8,0xE1,0x29,0x7A,0x6C,0xB0,0x19,0x05,0x13,0x6F,
		0x9A,0x91,0x1A,0x9D,0x38,0x44,0xF5,0xC8,0x2D,0x8B,0x1E,0x2C,0x27,0x3C,0xD0,0x15,
		0xE7,0x29,0xC1,0x95,0xE0,0x77,0x3D,0xA8,0xF6,0x5F,0x88,0xBD,0xCF,0x49,0xA2,0xE8,
	};
	unsigned char peer_prikey[64]={
		0x2F,0x84,0x11,0xED,0xC1,0xF6,0xEB,0xE3,0xCB,0x39,0xBB,0xCE,0xDC,0x95,0x8D,0x9E,
		0x58,0x73,0x42,0xF1,0xAE,0x49,0xFC,0xC7,0x14,0x87,0x94,0xB4,0x5E,0xED,0xAD,0xDA,
		0x9E,0x80,0xA1,0x4F,0xD3,0xAD,0x69,0xC3,0x4D,0x26,0xF9,0xE5,0xD3,0x85,0x6C,0xC5,
		0x7E,0x53,0x66,0x66,0xCE,0x05,0x47,0xD7,0x2E,0x37,0x1B,0xF3,0x0F,0xCD,0x3F,0xE2,
	};
	unsigned char peer_pubkey[128]={
		0x8B,0x58,0xE5,0xBD,0x88,0xC4,0x1D,0x29,0xC8,0x5D,0x92,0x0B,0x64,0xD2,0x14,0xF0,
		0x45,0x26,0x25,0x2A,0x16,0x70,0x6C,0x41,0x23,0xD3,0xAD,0xD4,0x08,0xC4,0x62,0xFB,
		0xB4,0xCB,0xB9,0xE1,0x9F,0xF4,0x34,0x61,0x76,0x06,0x55,0x43,0x5C,0x2C,0x9E,0xD9,
		0x81,0x3A,0x98,0xCA,0x6E,0x06,0xAE,0x5A,0x08,0x6C,0x28,0xC7,0x55,0x0F,0xEF,0xDC,
		0x6C,0x10,0x29,0xC0,0x67,0xEB,0x73,0x27,0x36,0x69,0x7B,0x2E,0x27,0xA3,0x4D,0xE0,
		0xEC,0x46,0xA9,0x1E,0xDF,0xAC,0x4A,0x38,0xC2,0x9A,0x0A,0x19,0x09,0x57,0x46,0xD9,
		0xDD,0x20,0xE6,0x34,0xDB,0x89,0x41,0x64,0xC3,0x4E,0x69,0xED,0x97,0x23,0xE1,0x1A,
		0x2D,0x34,0x48,0x86,0x0D,0x98,0xDB,0x67,0xFF,0x37,0x7A,0xCD,0x55,0x9D,0x93,0x32,
	};

	unsigned char std_key[64]={
		0x3B,0x87,0x3B,0xFB,0x4A,0xE0,0xCA,0x1C,0xB7,0xF0,0xDB,0xEF,0x86,0x9C,0x96,0xF3,
		0xB8,0x5C,0xD5,0x6D,0x29,0xCF,0x1F,0xBF,0xF9,0xF8,0xE9,0x83,0xE3,0x42,0x93,0x27,
		0xD2,0xB0,0x17,0x4E,0x9B,0x76,0x3B,0x48,0xAB,0x2A,0x6D,0x87,0x8A,0xA3,0xF8,0xE3,
		0x6E,0x5C,0xBD,0x29,0x4B,0xCA,0x36,0x5E,0x41,0x12,0x3A,0x6E,0x00,0xEF,0x7E,0x3A,};
	unsigned int keyByteLen = 64;
	unsigned int ret;

	ret = ecdh_test_internal(brainpoolp512r1, "brainpoolp512r1", local_prikey, local_pubkey,
				  peer_prikey, peer_pubkey, std_key, keyByteLen, NULL);
	if(ret)
		return 1;

	return 0;
}
#endif


#if (ECCP_MAX_BIT_LEN >= 521)
unsigned int ecdh_std_test_secp521r1(void)
{
	unsigned char local_prikey[66]={
			0x00,0x3B,0x1A,0x32,0x0D,0xAA,0x7B,0xA5,0x77,0xA7,0x19,0x32,0x23,0xA4,0x9A,0x4A,
			0x1B,0xE4,0xC6,0x60,0x68,0x56,0xE5,0x87,0x7E,0x63,0x58,0x63,0xC0,0x1A,0xC8,0xBD,
			0x21,0x7C,0x51,0x0F,0xB1,0x86,0xC7,0x12,0xF5,0x2D,0x49,0x1B,0x01,0x16,0x93,0x50,
			0x81,0xEF,0x1F,0xF7,0xC4,0x36,0x5D,0xBC,0x5F,0x7C,0xFC,0x42,0xE0,0x65,0xE9,0x6B,
			0x0F,0x22,
	};
	unsigned char local_pubkey[132]={
			0x01,0x84,0xB6,0xE1,0x7A,0x48,0x3B,0xCC,0x5E,0x3E,0x7B,0x41,0x74,0xFC,0xD8,0x17,
			0x43,0xF7,0x08,0xE1,0xB0,0xD9,0x43,0x4E,0x6D,0x99,0x31,0x68,0x94,0x33,0xBA,0x6A,
			0xE5,0xE6,0xD8,0xA8,0xE9,0x99,0x7F,0xE7,0x20,0x18,0xCA,0x80,0x34,0x19,0x00,0x4C,
			0xA4,0xF4,0x3A,0xD4,0x65,0xF9,0x2D,0x47,0x42,0x08,0x96,0xF8,0x3F,0x92,0xC6,0xDF,
			0xDD,0x01,0x01,0x7F,0x3E,0x09,0x6B,0x00,0xE0,0xA0,0x38,0xEF,0x7F,0x84,0xC1,0x92,
			0x90,0x91,0x2D,0x8C,0x68,0x1E,0x6A,0x24,0xBB,0xC1,0x37,0xBE,0x5A,0x6E,0x85,0x43,
			0xFB,0xC3,0x89,0x3E,0x6E,0xB0,0x4D,0x89,0x25,0xE7,0xA3,0xD4,0x09,0x4D,0xD1,0x41,
			0x81,0x61,0xD9,0x4B,0x67,0xB0,0x58,0xD4,0x36,0xEF,0xD1,0x3F,0xE5,0xFE,0x37,0xAF,
			0x58,0x1B,0x5F,0xCD,
	};
	unsigned char peer_prikey[66]={
			0x00,0xB4,0x21,0xC4,0x6E,0xD1,0x32,0x45,0x3A,0xD1,0xCC,0xDA,0x00,0xB9,0xA1,0xCB,
			0x78,0x83,0xAF,0xDE,0x71,0x99,0xA0,0xE6,0x6D,0x5A,0xF9,0x31,0xB6,0x69,0xA5,0x61,
			0x92,0xAD,0x40,0xF6,0x99,0xA9,0x66,0xB4,0x62,0xDC,0x4E,0x7D,0xF5,0x3F,0x95,0x18,
			0xE1,0x1E,0xA5,0x39,0xCE,0x4B,0xD2,0x53,0xE6,0xAA,0xB3,0xD9,0xB4,0xDF,0xD8,0x58,
			0x2C,0xB3,
	};
	unsigned char peer_pubkey[132]={
			0x01,0xE1,0x11,0x48,0x93,0xBE,0x8C,0x37,0xAC,0x29,0x26,0x9D,0x26,0x47,0x07,0xBE,
			0xFF,0x9E,0xD2,0x6F,0xBF,0x77,0x38,0xBE,0x46,0x34,0x45,0xCB,0x1A,0xAF,0x4A,0xDB,
			0xC8,0xE0,0xE2,0xEA,0xCA,0xAA,0x6C,0x00,0x80,0xC1,0x64,0xBE,0x67,0x9F,0x43,0x02,
			0x13,0x80,0x0F,0x50,0xA4,0x4F,0xFF,0x62,0x95,0xF1,0xDE,0xA9,0x9E,0x75,0xD0,0x0E,
			0x66,0xB0,0x01,0xC1,0x8F,0x72,0x62,0x8E,0x30,0x37,0xC8,0x74,0x48,0x4A,0xDF,0x66,
			0xDA,0x9C,0x22,0xFA,0xB3,0x6D,0x73,0x9D,0x86,0xF1,0x4F,0x58,0x31,0xA2,0x46,0xD1,
			0x9C,0x2A,0x21,0xEA,0xB3,0x3E,0x6D,0xF2,0x01,0xF3,0x58,0x84,0x35,0x22,0xAB,0x6F,
			0xAA,0xDA,0x3F,0xDF,0x7F,0x8E,0x82,0x0C,0x63,0x2C,0xA9,0x7C,0x53,0x1E,0x13,0x78,
			0x61,0x0F,0x09,0xBE,
	};

	unsigned char std_key[66]={
			0x01,0x50,0x31,0x02,0x40,0x64,0xDD,0x30,0x36,0xD2,0x42,0x03,0x27,0x95,0x77,0xE0,
			0x17,0x67,0x66,0xD7,0x9E,0x53,0xF1,0x9F,0x1C,0x1C,0x4A,0x6B,0xD0,0x43,0xD4,0xE7,
			0x51,0x07,0x96,0x64,0xF5,0x18,0xE9,0xD6,0x6C,0x70,0x10,0xC2,0x75,0x17,0x52,0xEE,
			0x2E,0xA8,0xA7,0x4A,0xE9,0x9E,0xE5,0x80,0x7B,0xD6,0xC7,0x42,0xBA,0xDE,0xDC,0x58,
			0xD4,0x65};
	unsigned int keyByteLen = 66;
	unsigned int ret;

	ret = ecdh_test_internal(secp521r1, "secp521r1", local_prikey, local_pubkey,
				  peer_prikey, peer_pubkey, std_key, keyByteLen, NULL);
	if(ret)
		return 1;

	return 0;
}
#endif

/**
 * @brief		ecdh rand test.
 * @return		ECDH_SUCCESS(success); other(error).
 */
unsigned int ecdh_rand_test(eccp_curve_t *curve, char *curve_name)
{
	unsigned char local_prikey[66];
	unsigned char local_pubkey[132];
	unsigned char peer_prikey[66];
	unsigned char peer_pubkey[132];
	unsigned char local_key[200];
	unsigned char peer_key[200];
	unsigned int i,j;
	unsigned int keyByteLen, trueKeyByteLen;
	KDF_FUNC kdf=NULL;
	unsigned int ret;

	printf("\r\n\r\n ------------- %s ECDH rand data test ------------- ", curve_name);

	for(i=0;i<ECDH_SIGN_VERIFY_ROUND1;i++)
	{
		ret = eccp_getkey(curve, local_prikey, local_pubkey);
		if(PKE_SUCCESS != ret)
		{
			printf("\r\n %s ECDH A get key error, ret=%d", curve_name, ret);
			return 1;
		}

		ret = eccp_getkey(curve, peer_prikey, peer_pubkey);
		if(PKE_SUCCESS != ret)
		{
			printf("\r\n %s ECDH B get key error, ret=%d", curve_name, ret);
			return 1;
		}
		printf("\r\n ECDH get key OK!");
		for(j=0;j<50;j++)
		{
			printf("\r\n %s ECDH round1=%d, round2=%d", curve_name, i+1, j+1);

			keyByteLen = local_prikey[0]+local_prikey[1];
			keyByteLen %= 200;
			if(0 == keyByteLen)
			{
				keyByteLen+=1;
			}
			trueKeyByteLen = keyByteLen;
			if(NULL == kdf)
			{
				if(keyByteLen > GET_BYTE_LEN(curve->eccp_p_bitLen))
				{
					trueKeyByteLen = GET_BYTE_LEN(curve->eccp_p_bitLen);
				}
			}

			ret = ecdh_compute_key(curve, local_prikey, peer_pubkey, local_key,  keyByteLen, kdf);
			if(ECDH_SUCCESS != ret)
			{
				printf("\r\n %s ECDH test error 1, ret = %02x", curve_name, ret);
				return 1;
			}

			ret = ecdh_compute_key(curve, peer_prikey, local_pubkey, peer_key,  keyByteLen, kdf);
			if(ECDH_SUCCESS != ret || memcmp(local_key, peer_key, trueKeyByteLen))
			{
				printf("\r\n %s ECDH test error 2, ret = %02x", curve_name, ret);
				return 1;
			}
		}
	}

	return 0;
}

/**
 * @brief		ecdh speed test.
 * @return		ECDH_SUCCESS(success); other(error).
 */
unsigned int ecdh_speed_test(eccp_curve_t *curve)
{
	unsigned char local_prikey[66];
	unsigned char local_pubkey[132];
	unsigned char peer_prikey[66];
	unsigned char peer_pubkey[132];
	unsigned char local_key[200];
	unsigned char peer_key[200];
	unsigned int i, keyByteLen=16;
	KDF_FUNC kdf=NULL;
	unsigned int ret;

printf("\r\n begin");
for(i=0;i<80;i++)
	ret = eccp_getkey(curve, local_prikey, local_pubkey);
printf("\r\n finished");

	if(PKE_SUCCESS != ret)
	{
		printf("\r\n ECDH %d A get key error, ret=%d", curve->eccp_p_bitLen, ret);
		return 1;
	}

	ret = eccp_getkey(curve, peer_prikey, peer_pubkey);
	if(PKE_SUCCESS != ret)
	{
		printf("\r\n ECDH %d B get key error, ret=%d", curve->eccp_p_bitLen, ret);
		return 1;
	}
	ret = ecdh_compute_key(curve, local_prikey, peer_pubkey, local_key,  keyByteLen, kdf);
	if(ECDH_SUCCESS != ret)
	{
		printf("\r\n ECDH %d test error 1, ret = %02x", curve->eccp_p_bitLen, ret);
		return 1;
	}

printf("\r\n begin");
for(i=0;i<80;i++)
	ret = ecdh_compute_key(curve, peer_prikey, local_pubkey, peer_key,  keyByteLen, kdf);
printf("\r\n finished");

	if(ECDH_SUCCESS != ret || memcmp(local_key, peer_key,keyByteLen))
	{
		printf("\r\n ECDH %d test error 2, ret = %02x", curve->eccp_p_bitLen, ret);
		return 1;
	}

	return 0;
}


unsigned char ECDH_all_test(void)
{
	printf("\r\n\r\n =================== ECDH test =================== ");

#if 0
//	sleep_ms(5);
	if(ecdh_speed_test(secp256r1))
		return 1;
#endif

#if 1

	/********** standard data test **********/
#if (ECCP_MAX_BIT_LEN >= 160)
	if(ecdh_std_test_brainpoolp160r1())
		return 1;
#endif

#if (ECCP_MAX_BIT_LEN >= 192)
	if(ecdh_std_test_secp192r1())
		return 1;
#endif

#if (ECCP_MAX_BIT_LEN >= 224)
	if(ecdh_std_test_secp224r1())
		return 1;
#endif

#if (ECCP_MAX_BIT_LEN >= 256)
	if(ecdh_std_test_secp256r1())
		return 1;
#endif

#if (ECCP_MAX_BIT_LEN >= 384)
	if(ecdh_std_test_secp384r1())
		return 1;
#endif

#if (ECCP_MAX_BIT_LEN >= 512)
	if(ecdh_std_test_brainpoolp512r1())
		return 1;
#endif

#if (ECCP_MAX_BIT_LEN >= 521)
	if(ecdh_std_test_secp521r1())
		return 1;
#endif

	/********** rand data test **********/
#if (ECCP_MAX_BIT_LEN >= 160)
	if(ecdh_rand_test(brainpoolp160r1, "brainpoolp160r1"))
		return 1;
#endif

#if (ECCP_MAX_BIT_LEN >= 192)
	if(ecdh_rand_test(secp192r1, "secp192r1"))
		return 1;
#endif

#if (ECCP_MAX_BIT_LEN >= 224)
	if(ecdh_rand_test(secp224r1, "secp224r1"))
		return 1;
#endif

#if (ECCP_MAX_BIT_LEN >= 256)
	if(ecdh_rand_test(secp256r1, "secp256r1"))
		return 1;
#endif

#if (ECCP_MAX_BIT_LEN >= 384)
	if(ecdh_rand_test(secp384r1, "secp384r1"))
		return 1;
#endif

#if (ECCP_MAX_BIT_LEN >= 512)
	if(ecdh_rand_test(brainpoolp512r1, "brainpoolp512r1"))
		return 1;
#endif

#if (ECCP_MAX_BIT_LEN >= 521)
	if(ecdh_rand_test(secp521r1, "secp521r1"))
		return 1;
#endif

#endif

	return 0;
}

void user_init()
{
	gpio_set_func(LED1|LED2|LED3|LED4, AS_GPIO);
	gpio_set_output_en(LED1|LED2|LED3|LED4, 1);
	gpio_set_input_en(LED1|LED2|LED3|LED4, 0);
	gpio_write(LED1|LED2|LED3|LED4, 1);

	ECDH_all_test();
}

void main_loop (void)
{
	sleep_ms(500);

	gpio_toggle(LED1|LED2|LED3|LED4);
}

#endif

