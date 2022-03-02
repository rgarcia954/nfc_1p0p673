/**
 * @ndef.h
 * @brief Header file for NDEF payload information
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
#ifndef NDEF_H_
#define NDEF_H_

#include <stdint.h>
#include <stdbool.h>

/* ***************************************************************************
 * DEFINES
 * *************************************************************************** */

/* Refer 2.1 - NFCForum-TS-Type-2-Tag_1.1 */
#define NFC_TYPE2_TAG_MEMSIZE					64U
#define NFC_TYPE2_TAG_BLOCKS					16U
#define NFC_TYPE2_TAG_BYTES_PER_BLOCK			(NFC_TYPE2_TAG_MEMSIZE / NFC_TYPE2_TAG_BLOCKS)

#define NFC_TYPE2_TAG_UID_BLOCK_IDX				0U
#define NFC_TYPE2_TAG_SERIAL_BLOCK_IDX			1U
#define NFC_TYPE2_TAG_LOCK_BLOCK_IDX			2U
#define NFC_TYPE2_TAG_CC_BLOCK_IDX				3U

#define NFC_TYPE2_TAG_DATA_BLOCK_START_IDX		4U
#define NFC_TYPE2_TAG_DATA_BLOCK_END_IDX		15U

#define NFC_TYPE2_TAG_DATA_BLOCK_OFFSET			(NFC_TYPE2_TAG_DATA_BLOCK_START_IDX * NFC_TYPE2_TAG_BYTES_PER_BLOCK)
#define NFC_TYPE2_TAG_DATA_BLOCKS				(NFC_TYPE2_TAG_DATA_BLOCK_END_IDX - NFC_TYPE2_TAG_DATA_BLOCK_START_IDX + 1)
#define NFC_TYPE2_TAG_DATA_BLOCK_BYTES			(NFC_TYPE2_TAG_DATA_BLOCKS * NFC_TYPE2_TAG_BYTES_PER_BLOCK)

#define NFC_TYPE2_TAG_NDEF_TLV_TAG_OFFSET		0U
#define NFC_TYPE2_TAG_NDEF_TLV_LENGTH_OFFSET	1U
#define NFC_TYPE2_TAG_NDEF_TLV_VALUE_OFFSET		2U

/* Refer Section 3.2 - NFCForum-TS-NDEF_1.0 */


#define NDEF_REC_HEADER_FIELD_SIZE						1
#define NDEF_REC_TYPE_LENGTH_FIELD_SIZE					1
#define NDEF_REC_PAYLOAD_LENGTH_FIELD_SIZE_SHORT		1
#define NDEF_REC_PAYLOAD_LENGTH_FIELD_SIZE				4
#define NDEF_REC_ID_LENGTH_FIELD_SIZE					1

#define NDEF_REC_MB_POS							7U
#define NDEF_REC_MB_MASK						((uint32_t)(0x01 << NDEF_REC_MB_POS))
#define NDEF_REC_ME_POS							6U
#define NDEF_REC_ME_MASK						((uint32_t)(0x01 << NDEF_REC_ME_POS))
#define NDEF_REC_CF_POS							5U
#define NDEF_REC_CF_MASK						((uint32_t)(0x01 << NDEF_REC_CF_POS))
#define NDEF_REC_SR_POS							4U
#define NDEF_REC_SR_MASK						((uint32_t)(0x01 << NDEF_REC_SR_POS))
#define NDEF_REC_IL_POS							3U
#define NDEF_REC_IL_MASK						((uint32_t)(0x01 << NDEF_REC_IL_POS))
#define NDEF_REC_TNF_POS						2U
#define NDEF_REC_TNF_MASK				    	((uint32_t)(0x07 << NDEF_REC_TNF_POS))

#define NDEF_CONTAINS_NDEF_TLV					0x03
#define NDEF_TERMINATOR_TLV						0xFE

#define NDEF_TEXT_RECORD_TYPE					0x54U
#define NDEF_URI_RECORD_TYPE					0x55U
#define NDEF_URI_ID_CODE_NUM					36U

/* Refer Section 3.2 - NFCForum-TS-RTD_Text_1.0 */

#define NDEF_TEXT_RECORD_STATUS_BYTE_OFFSET				0x00U
#define NDEF_TEXT_RECORD_IANA_CODE_BYTE_OFFSET			0x01U
#define NDEF_TEXT_RECORD_IANA_CODE_LEN_MASK				0x1FU

/* Refer Section 3.2 - NFCForum-TS-RTD_URI_1.0  */
#define NDEF_URI_RECORD_IDCODE_BYTE_OFFSET				0x00U

/* Refer Section 4.5 to 4.9 - NFCForum-TS-DigitalProtocol-1.0 */

#define NFC_TYPE2_HLTA_CMD						0x50U
#define NFC_TYPE2_ATQA_ANTI_COLLISON_RSP		0x44U
#define NFC_TYPE2_ATQA_PLATFORM_INFO_RSP		0x00U
#define NFC_TYPE2_SAK_NOT_ISO144434_RSP		    0x00U
#define NFC_TYPE2_SAK_NOT_COMPLETE_RSP			0x0FU

/* Refer Section 9 - NFCForum-TS-DigitalProtocol-1.0 */

#define NFC_TYPE2_READ_CMD_LEN					2U
#define NFC_TYPE2_WRITE_CMD_LEN					6U
#define NFC_TYPE2_MAX_CMD_LEN					NFC_TYPE2_WRITE_CMD_LEN

#define NFC_TYPE2_READ_CMD					    0x30U
#define NFC_TYPE2_WRITE_CMD						0xA2U
#define NFC_TYPE2_SECTOR_SELECT_CMD				0xC2U
#define NFC_TYPE2_ACK_RSP						0x0AU
#define NFC_TYPE2_NACK_INVALID_ARG_RSP			0x00U
#define NFC_TYPE2_NACK_CRC_ERR_RSP				0x01U
#define NFC_TYPE2_NACK_WRITE_ERR_RSP			0x05U

#define NFC_TYPE2_READ_RSP_BYTES				16U
#define NFC_TYPE2_WRITE_BYTES					NFC_TYPE2_TAG_BYTES_PER_BLOCK

typedef struct {
	uint8_t* msg;
	uint8_t len;

    uint8_t type_len;
    uint8_t type_offset;

    uint8_t id_len;
    uint8_t id_offset;

    uint8_t payload_len;
    uint8_t payload_offset;
} ndef_layout_t;

typedef enum {
	CMD_IDX = 0,
	BLOCK_NUM_IDX,
	DATA_START_IDX,
} ndef_cmd_idx_t;


/* ***************************************************************************
 * FUNCTION DECLARATIONS
 * *************************************************************************** */

void ndef_record_build(bool mb, bool me, bool cf, bool sr,
							 bool il, uint8_t tnf, char* type,
							 uint8_t type_siz, char* id, uint8_t id_siz,
							 char* payload, uint32_t payload_siz,
							 ndef_layout_t* record);

void ndef_record_parse(uint8_t* msg, ndef_layout_t* record);

void ndef_payload_parse(ndef_layout_t* record, uint8_t* p_payload);

#endif    /* NDEF_H_ */
