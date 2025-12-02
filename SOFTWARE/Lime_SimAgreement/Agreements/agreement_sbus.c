#include "agreement_sbus.h"
#include <string.h>
#include <stdio.h>

static void encode(RC_Key_V3A_t* scr, uint8_t* tar, uint8_t* tarLen);
static void decode(uint8_t* scr, RC_Key_V3A_t* tar);
static void debugOutRes(uint8_t* scr, char resStr[128]);

agreementCodec_t agrCodec_SBUS = 
{
	.name = "SBUS",
	
	.uart_baudRate = 100000,
	.uart_wordLength = 8,
	.uart_parity = 'E',
	.uart_stopBits = 2,
	
	.encode = encode,
	.decode = decode,
	.debugOutRes = debugOutRes,
};

typedef struct {
    uint16_t channels[16];  // 16 analog channels(0 ~ 2047)
    bool ch17;              // digital channel 17
    bool ch18;              // digital channel 18
    bool frame_lost;
    bool failsafe;
    bool valid;
} SBUS_data_t;

SBUS_data_t sbus_data;

static void decode_SBUS(uint8_t raw[25], SBUS_data_t *sbus)
{
	if(raw == NULL || sbus == NULL)
		return;

    /* Check frame validity */
    sbus->valid = (raw[0] == 0x0F && raw[24] == 0x00);
    if (!sbus->valid) return;

    /* Decode 22 bytes of SBUS data */
    sbus->channels[0]  = (raw[1]    | (raw[2] << 8))  & 0x07FF;
    sbus->channels[1]  = (raw[2] >> 3 | (raw[3] << 5))  & 0x07FF;
    sbus->channels[2]  = (raw[3] >> 6 | (raw[4] << 2) | (raw[5] << 10)) & 0x07FF;
    sbus->channels[3]  = (raw[5] >> 1 | (raw[6] << 7))  & 0x07FF;
    sbus->channels[4]  = (raw[6] >> 4 | (raw[7] << 4))  & 0x07FF;
    sbus->channels[5]  = (raw[7] >> 7 | (raw[8] << 1) | (raw[9] << 9))  & 0x07FF;
    sbus->channels[6]  = (raw[9] >> 2 | (raw[10] << 6)) & 0x07FF;
    sbus->channels[7]  = (raw[10] >> 5 | (raw[11] << 3)) & 0x07FF;
    sbus->channels[8]  = (raw[12]    | (raw[13] << 8)) & 0x07FF;
    sbus->channels[9]  = (raw[13] >> 3 | (raw[14] << 5)) & 0x07FF;
    sbus->channels[10] = (raw[14] >> 6 | (raw[15] << 2) | (raw[16] << 10)) & 0x07FF;
    sbus->channels[11] = (raw[16] >> 1 | (raw[17] << 7)) & 0x07FF;
    sbus->channels[12] = (raw[17] >> 4 | (raw[18] << 4)) & 0x07FF;
    sbus->channels[13] = (raw[18] >> 7 | (raw[19] << 1) | (raw[20] << 9)) & 0x07FF;
    sbus->channels[14] = (raw[20] >> 2 | (raw[21] << 6)) & 0x07FF;
    sbus->channels[15] = (raw[21] >> 5 | (raw[22] << 3)) & 0x07FF;

    /* decode flags byte (index 23) */
    uint8_t flags = raw[23];
    sbus->ch17 = flags & 0x01;
    sbus->ch18 = flags & 0x02;
    sbus->frame_lost = flags & 0x04;
    sbus->failsafe = flags & 0x08;
}

void encodeSBUS(SBUS_data_t *sbus, uint8_t tar[25]) 
{
	if(sbus == NULL || tar == NULL)
		return;

    /* Set frame header and footer */
    tar[0] = 0x0F;  /* SBUS frame header */
    tar[24] = 0x00; /* SBUS frame footer */
    
    /* Clear channel data area (prevent old data interference) */
    for (int i = 1; i <= 22; i++) {
        tar[i] = 0;
    }
    
    /* Pack 16 channels of 11-bit data into 22 bytes */
    /* Channel 0 */
    tar[1] = sbus->channels[0] & 0xFF;         /* Low 8 bits */
    tar[2] = (sbus->channels[0] >> 8) & 0x07;  /* High 3 bits */
    
    /* Channel 1 */
    tar[2] |= (sbus->channels[1] << 3) & 0xF8; /* Low 5 bits */
    tar[3] = (sbus->channels[1] >> 5) & 0x3F;  /* High 6 bits */
    
    /* Channel 2 */
    tar[3] |= (sbus->channels[2] << 6) & 0xC0; /* Low 2 bits */
    tar[4] = (sbus->channels[2] >> 2) & 0xFF;  /* Middle 8 bits */
    tar[5] = (sbus->channels[2] >> 10) & 0x01; /* High 1 bit */
    
    /* Channel 3 */
    tar[5] |= (sbus->channels[3] << 1) & 0xFE; /* Low 7 bits */
    tar[6] = (sbus->channels[3] >> 7) & 0x0F;  /* High 4 bits */
    
    /* Channel 4 */
    tar[6] |= (sbus->channels[4] << 4) & 0xF0; /* Low 4 bits */
    tar[7] = (sbus->channels[4] >> 4) & 0x7F;  /* High 7 bits */
    
    /* Channel 5 */
    tar[7] |= (sbus->channels[5] << 7) & 0x80; /* Low 1 bit */
    tar[8] = (sbus->channels[5] >> 1) & 0xFF;  /* Middle 8 bits */
    tar[9] = (sbus->channels[5] >> 9) & 0x03;  /* High 2 bits */
    
    /* Channel 6 */
    tar[9] |= (sbus->channels[6] << 2) & 0xFC; /* Low 6 bits */
    tar[10] = (sbus->channels[6] >> 6) & 0x1F; /* High 5 bits */
    
    /* Channel 7 */
    tar[10] |= (sbus->channels[7] << 5) & 0xE0; /* Low 3 bits */
    tar[11] = (sbus->channels[7] >> 3) & 0xFF;  /* High 8 bits */
    
    /* Channel 8 */
    tar[12] = sbus->channels[8] & 0xFF;         /* Low 8 bits */
    tar[13] = (sbus->channels[8] >> 8) & 0x07;  /* High 3 bits */
    
    /* Channel 9 */
    tar[13] |= (sbus->channels[9] << 3) & 0xF8; /* Low 5 bits */
    tar[14] = (sbus->channels[9] >> 5) & 0x3F;  /* High 6 bits */
    
    /* Channel 10 */
    tar[14] |= (sbus->channels[10] << 6) & 0xC0; /* Low 2 bits */
    tar[15] = (sbus->channels[10] >> 2) & 0xFF;   /* Middle 8 bits */
    tar[16] = (sbus->channels[10] >> 10) & 0x01; /* High 1 bit */
    
    /* Channel 11 */
    tar[16] |= (sbus->channels[11] << 1) & 0xFE; /* Low 7 bits */
    tar[17] = (sbus->channels[11] >> 7) & 0x0F;  /* High 4 bits */
    
    /* Channel 12 */
    tar[17] |= (sbus->channels[12] << 4) & 0xF0; /* Low 4 bits */
    tar[18] = (sbus->channels[12] >> 4) & 0x7F;  /* High 7 bits */
    
    /* Channel 13 */
    tar[18] |= (sbus->channels[13] << 7) & 0x80; /* Low 1 bit */
    tar[19] = (sbus->channels[13] >> 1) & 0xFF;  /* Middle 8 bits */
    tar[20] = (sbus->channels[13] >> 9) & 0x03;  /* High 2 bits */
    
    /* Channel 14 */
    tar[20] |= (sbus->channels[14] << 2) & 0xFC; /* Low 6 bits */
    tar[21] = (sbus->channels[14] >> 6) & 0x1F;  /* High 5 bits */
    
    /* Channel 15 */
    tar[21] |= (sbus->channels[15] << 5) & 0xE0; /* Low 3 bits */
    tar[22] = (sbus->channels[15] >> 3) & 0xFF;  /* High 8 bits */
    
    /* Set flags byte (index 23) */
    tar[23] = 0;
    if (sbus->ch17) tar[23] |= 0x01;
    if (sbus->ch18) tar[23] |= 0x02;
    if (sbus->frame_lost) tar[23] |= 0x04;
    if (sbus->failsafe) tar[23] |= 0x08;
}

static void encode(RC_Key_V3A_t* scr, uint8_t* tar, uint8_t* tarLen)
{	
	if(scr == NULL || tar == NULL || tarLen == NULL)
		return;

	sbus_data.channels[0] = scr->ch[0] >> 1;
	sbus_data.channels[1] = scr->ch[1] >> 1;
	sbus_data.channels[2] = scr->ch[2] >> 1;
	sbus_data.channels[3] = scr->ch[3] >> 1;

	sbus_data.channels[4] = scr->l_down ? 0 : scr->l_mid ? 1024 : 2047;
	sbus_data.channels[5] = scr->r_down ? 0 : scr->r_mid ? 1024 : 2047;
	sbus_data.channels[6] = scr->joy_l ? 2047 : 0;
	sbus_data.channels[7] = scr->joy_r ? 2047 : 0;

	encodeSBUS(&sbus_data, tar);
	
	/* calculate tarLen */
	*tarLen = 25;
}

static void decode(uint8_t* scr, RC_Key_V3A_t* tar)
{
	if(scr == NULL || tar == NULL)
		return;

	decode_SBUS(scr, &sbus_data);

	tar->ch[0] = sbus_data.channels[0] << 1;
	tar->ch[1] = sbus_data.channels[1] << 1;
	tar->ch[2] = sbus_data.channels[2] << 1;
	tar->ch[3] = sbus_data.channels[3] << 1;

	tar->l_down = sbus_data.channels[4] < 682;
	tar->l_up   = sbus_data.channels[4] > 1364;
	tar->l_mid  = !(tar->l_down || tar->l_up);

	tar->r_down = sbus_data.channels[5] < 682;
	tar->r_up   = sbus_data.channels[5] > 1364;
	tar->r_mid  = !(tar->r_down || tar->r_up);

	tar->joy_l  = sbus_data.channels[6] > 1024;
	tar->joy_r  = sbus_data.channels[7] > 1024;
}

static void debugOutRes(uint8_t* scr, char resStr[128])
{
	if(scr == NULL || resStr == NULL)
		return;

	decode_SBUS(scr, &sbus_data);
	
	int32_t len = 0;

	len += snprintf(resStr, 128, "DEBUS Res:");
	len += snprintf(resStr + len, 128 - len, "ch1:%d ", sbus_data.channels[0]);
	len += snprintf(resStr + len, 128 - len, "ch2:%d ", sbus_data.channels[1]);
	len += snprintf(resStr + len, 128 - len, "ch3:%d ", sbus_data.channels[2]);
	len += snprintf(resStr + len, 128 - len, "ch4:%d ", sbus_data.channels[3]);
	len += snprintf(resStr + len, 128 - len, "ch5:%d ", sbus_data.channels[4]);
	len += snprintf(resStr + len, 128 - len, "ch6:%d ", sbus_data.channels[5]);
	len += snprintf(resStr + len, 128 - len, "ch7:%d ", sbus_data.channels[6]);
	len += snprintf(resStr + len, 128 - len, "ch8:%d ", sbus_data.channels[7]);

	len += snprintf(resStr + len, 128 - len, "\n");
	
	resStr[127] = '\0';
}
