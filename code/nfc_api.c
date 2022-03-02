/**
 * @nfc_api.c
 * @brief C file for NFC related A.P.I
 * @copyright @parblock
 * Copyright (c) 2022 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */

#include "hw.h"
#include "api_isohfllhw.h"
#include "platform_config.h"
#include "iso14443.h"
#include "nfc_api.h"
#include "ndef.h"
#include "swmTrace_api.h"
#include "string.h"

/* ***************************************************************************
 * GLOBALS
 * *************************************************************************** */

/* Refer Section 2.1 - NFCForum-TS-Type-2-Tag_1.1 */

static uint8_t NFC_TYPE2_TAG_MEM[NFC_TYPE2_TAG_MEMSIZE] = { 0x11, 0x22, 0x33, 0x44,         /* UID 0:3 */
															 0xB1, 0x20, 0x00, 0x07,        /* UID 4:7 */
															 0x08, 0x09,                    /* UID 8:9 */
															 0x00, 0x00,                    /* Lock ? 0xFF (tag read only) : 0x00 (tag is read/write) */
															 0xE1, 0x10, 0x06, 0x00,        /* CC0 0xE1=>NDEF message; CC1 0x10=>Version number 1.0; CC2
                                                                                             *  0x06=>Data memory size/8 (in this case 6*8=48bytes); CC3
                                                                                             *  bit7-bit4 == 0 (read access granted), bit3-bit0 == 0 (write access granted)*/
															 0x03, 0x0F,                    /* TLV Tag=> NDEF message 0x03; TLV Length => Length of NDEF
																							 * message; */
															 0xD1,                          /* NDEF message: record */
															 0x01,                          /* NDEF message: Type length */
															 0x0B,                          /* NDEF message: Payload length */
															 0x55,                          /* NDEF message: Type */
															 0x01, 0x6F, 0x6E, 0x73,        /* NDEF message: Payload */
															 0x65, 0x6D, 0x69, 0x2E,        /* NDEF message: Payload */
															 0x63, 0x6F, 0x6D,              /* NDEF message: Payload */
															 0xFE,                          /* TLV Terminator */
															 0x00, 0x00, 0x00, 0x00,        /* Memory fill */
															 0x00, 0x00, 0x00, 0x00,        /* Memory fill */
															 0x00, 0x00, 0x00, 0x00,        /* Memory fill */
															 0x00, 0x00, 0x00, 0x00,        /* Memory fill */
															 0x00, 0x00, 0x00, 0x00,        /* Memory fill */
															 0x00, 0x00, 0x00, 0x00,        /* Memory fill */
															 0x00, 0x00, 0x00, 0x00,        /* Memory fill */
															 0x00, 0x00                     /* Memory fill */
};


/* Type A config tables in RAM */
static uint8_t Layer3Source[HF_IO_RAM_INIT_ISOALAYER3];

static ndef_layout_t record = {0};
static uint32_t tag_wr_num = 0;

/* ***************************************************************************
 * NFC IP HELPER FUNCTIONS
 * *************************************************************************** */
static inline uint8_t _isohf_getHFIORAMbyte_local(uint8_t offset)
{
    return ((uint8_t *)(HF_IO_RAM_START_ADD))[offset];
}

static inline void _isohf_setHFIORAMbyte_local(uint8_t offset, uint8_t data)
{
    ((uint8_t *)(HF_IO_RAM_START_ADD))[offset] = data;
}

uint32_t _LLHW_isohf_compareIORAM2Mem_local(HFCTRL isohf,  uint8_t *pComp, uint32_t byte_size)
{
    for (uint32_t i = 0; i < byte_size; i++)
    {
        if (pComp[i] != _isohf_getHFIORAMbyte_local(i))
        {
            return -1;
        }
    }
    return 0;
}

void _LLHW_isohf_copyMem2IORAM_local(HFCTRL isohf, uint8_t *pSource, uint8_t offset, uint32_t bytesLength)
{

    for (uint32_t i = 0; i < bytesLength; i++)
    {
        _isohf_setHFIORAMbyte_local(offset + i, (uint8_t)pSource[i]);
    }

}

void _LLHW_isohf_configIORAM4TypeALayer3_local(HFCTRL isohf, uint8_t *Layer3Source)
{
    _LLHW_isohf_copyMem2IORAM_local(isohf,
                                    Layer3Source,
                                    (uint8_t)(HF_IO_RAM_EMPTY_OFFSET >> 2),
                                    HF_IO_RAM_INIT_ISOALAYER3);
}

void _isohf_configTypeALayer3BootAndWait_local(HFCTRL isohf, uint8_t *Layer3Source)
{
    /* Fill the RAM for Layer 3 */
    _LLHW_isohf_configIORAM4TypeALayer3_local(isohf, Layer3Source);

    /* Wait */
    _LLHW_isohf_waitForRx(isohf, 0x0);
}

/* ***************************************************************************
 * NFC API
 * *************************************************************************** */

void NFC_ClearTagData(void)
{
	memset(&NFC_TYPE2_TAG_MEM[16], 0x00, (sizeof(NFC_TYPE2_TAG_MEM) - 16));
}

void NFC_Write2TagData(uint8_t block_num, uint8_t *pdata)
{
	uint8_t wr_idx = block_num * NFC_TYPE2_TAG_BYTES_PER_BLOCK;

	memcpy(&NFC_TYPE2_TAG_MEM[wr_idx], pdata, NFC_TYPE2_TAG_BYTES_PER_BLOCK);
}

void NFC_ReadFromTagData(uint8_t *p_resp, uint8_t block_num)
{
    uint16_t start_index = 0;
    uint16_t end_index = 0;
    uint8_t j = 0;

    start_index = block_num * NFC_TYPE2_TAG_BYTES_PER_BLOCK;          /* offset from command */
    end_index = start_index + NFC_TYPE2_READ_RSP_BYTES;           	  /* offset + 16bytes = end index */
    for (uint8_t i = start_index; i < end_index; i++)    			  /* copy all data from start index to end_index */
    {
        p_resp[j++] = NFC_TYPE2_TAG_MEM[i];
    }
}

void NFC_Read(uint8_t block_num)
{
	uint8_t read_rsp_buff[NFC_TYPE2_READ_RSP_BYTES] = {0,};
	uint32_t read_rsp_len = 0;

    if(block_num < NFC_TYPE2_TAG_BLOCKS)
    {
        /* Block number within range,
         * Respond with NFC_TYPE2_READ_RSP_BYTES
         * read from the tag.
         */
        NFC_ReadFromTagData(read_rsp_buff, block_num);
        read_rsp_len = NFC_TYPE2_READ_RSP_BYTES;
    }
    else
    {
        /* Block number exceeds total number of blocks
         * Set the response to NACK, don't copy payload
         */
    	read_rsp_buff[0] = NFC_TYPE2_NACK_INVALID_ARG_RSP;
    	read_rsp_len = sizeof(read_rsp_buff[0]);
    }

    _LLHW_isohf_copyMem2IORAM_local(HFCTRL_IP, read_rsp_buff,
    									0, read_rsp_len);

    _LLHW_isohf_launchTx(HFCTRL_IP, 0, 3, read_rsp_len, 0);
}


void NFC_Write(uint8_t block_num, uint8_t *pData)
{
    /* Prepare and send response */

	uint8_t resp_msg;

    if(block_num < NFC_TYPE2_TAG_BLOCKS)
    {
    	/* Block number within range,
    	 * set the response to ACK and
    	 * copy the payload into the tag.
    	 */
    	resp_msg = NFC_TYPE2_ACK_RSP;
        NFC_Write2TagData(block_num, pData);
    }
    else
    {
        /* Block number exceeds total number of blocks
         * Set the response to NACK, don't copy payload
         */
    	resp_msg = NFC_TYPE2_NACK_INVALID_ARG_RSP;
    }

	_LLHW_isohf_copyMem2IORAM_local(HFCTRL_IP, &resp_msg, 0, sizeof(resp_msg));
	_LLHW_isohf_launchTx(HFCTRL_IP, 0, 3, sizeof(resp_msg), 0);

	_LLHW_isohf_waitForRx(HFCTRL_IP, 0x0);
}


void NFC_Initialize(void)
{
    /* Configure NFC peripheral */
    SYSCTRL->NFC_CFG = NFC_EN;
    _isohf_setProtocolWP(HFCTRL_IP, 0xFF00);
    _isohf_setReboundFilterCfg(HFCTRL_IP, 0x7);
    _isohf_setDigitalCnt2Cfg(HFCTRL_IP, 0x0);

    /* Enable interrupts for NFC*/
    _isohf_enableEndOfComIt(HFCTRL_IP);

    NVIC_ClearPendingIRQ(NFC_IRQn);
    NVIC_EnableIRQ(NFC_IRQn);

    /* Setup anti collision response */
    Layer3Source[0]  = NFC_TYPE2_ATQA_ANTI_COLLISON_RSP;
    Layer3Source[1]  = NFC_TYPE2_ATQA_PLATFORM_INFO_RSP;
    Layer3Source[2]  = NFC_TYPE2_TAG_MEM[0];    		/* UID0 */
    Layer3Source[3]  = NFC_TYPE2_TAG_MEM[1];    		/* UID1 */
    Layer3Source[4]  = NFC_TYPE2_TAG_MEM[2];    		/* UID2 */
    Layer3Source[5]  = NFC_TYPE2_TAG_MEM[3];    		/* UID3 */
    Layer3Source[6]  = NFC_TYPE2_TAG_MEM[4];    		/* UID4 */
    Layer3Source[7]  = NFC_TYPE2_TAG_MEM[5];    		/* UID5 */
    Layer3Source[8]  = NFC_TYPE2_TAG_MEM[6];    		/* UID6 */
    Layer3Source[9]  = NFC_TYPE2_TAG_MEM[7];    		/* UID7 */
    Layer3Source[10] = NFC_TYPE2_TAG_MEM[8];    	    /* UID8 */
    Layer3Source[11] = NFC_TYPE2_TAG_MEM[9];    	    /* UID9 */
    Layer3Source[12] = NFC_TYPE2_SAK_NOT_COMPLETE_RSP;
    Layer3Source[13] = NFC_TYPE2_SAK_NOT_ISO144434_RSP;

    /* Waiting for the end of the boot if the boot triggers the HF */
    _isohf_setProtocolUID(HFCTRL_IP, 1);
    _LLHW_isohf_waitUntilPlatformHand(HFCTRL_IP);

    /* IO RAM configuration for Layer 3 */
    _isohf_configTypeALayer3BootAndWait_local(HFCTRL_IP, &Layer3Source[0]);

}

void NFC_Handler(void)
{
    if(_isohf_getEndOfComStatus(HFCTRL_IP) != 0)
    {
        uint8_t *ptr = (uint8_t *)PLATFORM_HF_BUFFER_ADDR;
        uint8_t cmd[NFC_TYPE2_MAX_CMD_LEN] = {0,};

        /* Clear the end of comm status */
        _isohf_clearEndOfComStatus(HFCTRL_IP);

        /* wait until platform can access IORAM buffer */
        _LLHW_isohf_waitUntilPlatformHand(HFCTRL_IP);

        /* check if no error in the RX frame */
        if (! _isohf_getRxErrorStatus(HFCTRL_IP))
        {
            /* Copy the Command */
            memcpy(cmd, ptr, sizeof(cmd));
        }

        switch(cmd[CMD_IDX])
        {
            case NFC_TYPE2_HLTA_CMD:
            {
                _LLHW_isohf_waitForRx(HFCTRL_IP, HF_P_CTRL_BACK2HALT);
            }
            break;

            case NFC_TYPE2_READ_CMD:
            {
                NFC_Read(cmd[BLOCK_NUM_IDX]);
            }
            break;

            case NFC_TYPE2_WRITE_CMD:
            {
                uint8_t ndef_tlv_len_idx = NFC_TYPE2_TAG_DATA_BLOCK_OFFSET +
                NFC_TYPE2_TAG_NDEF_TLV_LENGTH_OFFSET;

                NFC_Write(cmd[BLOCK_NUM_IDX], &cmd[DATA_START_IDX]);

                /* Write completed - See 6.4.3 of NFCForum-TS-Type-2-Tag_1.1
                * Write procedure is completed when the length field of the
                * NDEF TLV in the Tag is updated with the correct non-zero length value
                */
                if(NFC_TYPE2_TAG_MEM[ndef_tlv_len_idx] != 0)
                {
                    /* Update the number of successful tag writes */
                    tag_wr_num += 1;

                    memset(&record, 0x00, sizeof(record));

                    uint8_t text[NFC_TYPE2_TAG_DATA_BLOCK_BYTES] = {0};
                    uint8_t ndef_record_start_idx = NFC_TYPE2_TAG_DATA_BLOCK_OFFSET +
                    NFC_TYPE2_TAG_NDEF_TLV_VALUE_OFFSET;

                    ndef_record_parse(&NFC_TYPE2_TAG_MEM[ndef_record_start_idx], &record);
                    ndef_payload_parse(&record, text);

                    swmLogInfo("		%s was written into the tag!\r\n", text);
                }
            }
            break;

            default:
            {
                /* Mute */
                _LLHW_isohf_waitForRx(HFCTRL_IP, HF_P_CTRL_BACK2HALT);
            }
        }
    }
}

void NFC_IRQHandler(void)
{
    /* The EndOfComStatus is set on Tx/RX complete*/
    __NOP();
}
