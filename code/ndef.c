/**
 * @ndef.h
 * @brief Source file for managing NDEF recor
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

#include <ndef.h>
#include <malloc.h>
#include <string.h>

char* ndef_uri_codes[NDEF_URI_ID_CODE_NUM] = { "",
												"http://www.",
												"https://www.",
												"http://",
												"https://",
												"tel:",
												"mailto:",
												"ftp://anonymous:anonymous@",
												"ftp://ftp.",
												"ftps://",
												"sftp://",
												"smb://",
												"nfs://",
												"ftp://",
												"dav://",
												"news:",
												"telnet://",
												"imap:",
												"rtsp://",
												"urn:",
												"pop:",
												"sip:",
												"sips:",
												"tftp:",
												"btspp://",
												"btl2cap://",
												"btgoep://",
												"tcpobex://",
												"irdaobex://",
												"file://",
												"urn:epc:id:",
												"urn:epc:tag:",
												"urn:epc:pat:",
												"urn:epc:raw:",
												"urn:epc:",
												"urn:nfc:"};

#define NDEF_PARSE_UNSUPPORTED					"not supported!"


void ndef_record_build(bool mb, bool me, bool cf, bool sr,
							 bool il, uint8_t tnf, char* type,
							 uint8_t type_siz, char* id, uint8_t id_siz,
							 char* payload, uint32_t payload_siz,
							 ndef_layout_t* record)
{
	uint8_t offset = 0;

	/* Total length of record =
	 * header (1 byte) + type length (1 byte) +
	 * payload length (1 byte or 4 bytes - if SR set or reset) +
	 * id length (1 byte - if IL is set) +
	 * size of the type field in bytes +
	 * size of the ID field in bytes +
	 * size of the payload in bytes
	 */
	record->len = NDEF_REC_HEADER_FIELD_SIZE + NDEF_REC_TYPE_LENGTH_FIELD_SIZE +
					(sr ? NDEF_REC_PAYLOAD_LENGTH_FIELD_SIZE_SHORT : NDEF_REC_PAYLOAD_LENGTH_FIELD_SIZE) +
					(il ? NDEF_REC_ID_LENGTH_FIELD_SIZE : 0) +
					type_siz + id_siz + payload_siz;

	/* With the record length established,
	 * allocate the message buffer.
	 */
	record->msg = (uint8_t*)malloc(record->len * sizeof(uint8_t));

	/* Fill the record bit-fields */
	record->msg[0] = (tnf & NDEF_REC_TNF_MASK) |
						(il ? NDEF_REC_IL_MASK : 0) |
						(sr ? NDEF_REC_SR_MASK : 0) |
						(cf ? NDEF_REC_CF_MASK : 0) |
						(me ? NDEF_REC_ME_MASK : 0) |
						(mb ? NDEF_REC_MB_MASK : 0);

	offset += NDEF_REC_HEADER_FIELD_SIZE;

	/* Update the type length field */
	record->type_len = type_siz;
	record->msg[offset] = type_siz;
	offset += NDEF_REC_TYPE_LENGTH_FIELD_SIZE;

	/* Update the payload length field */
	record->payload_len = payload_siz;

	if(sr)
	{
		record->msg[offset] = payload_siz;
		offset += NDEF_REC_PAYLOAD_LENGTH_FIELD_SIZE_SHORT;
	}
	else
	{
		/* payload length is a 4 byte field,
		 * that needs to be copied into the record
		 */
		uint8_t* p_data = (uint8_t*)(&payload_siz);
		memcpy(&record->msg[offset], p_data, NDEF_REC_PAYLOAD_LENGTH_FIELD_SIZE);
		offset += NDEF_REC_PAYLOAD_LENGTH_FIELD_SIZE;
	}

	/* Update the id length field if necessary */
	if(il)
	{
		record->id_len = id_siz;
		record->msg[offset] = id_siz;
		offset += NDEF_REC_ID_LENGTH_FIELD_SIZE;
	}

	/* Update the type field */
	record->type_offset = offset;
	memcpy(&record->msg[offset], type, type_siz);
	offset += type_siz;

	/* Update the id field if necessary */
	if(il)
	{
		record->id_offset = offset;
		memcpy(&record->msg[offset], id, id_siz);
		offset += id_siz;
	}

	/* Update the payload field */
	record->payload_offset = offset;
	memcpy(&record->msg[offset], payload, payload_siz);

	// NOTE: (record_length != (offset + paylod_siz)) - error in building the record

}

void ndef_record_parse(uint8_t* msg, ndef_layout_t* record)
{
	uint8_t  offset = 0;

	record->msg = msg;

	bool is_il = msg[offset] & NDEF_REC_IL_MASK;
	bool is_sr = msg[offset] & NDEF_REC_SR_MASK;

	offset += NDEF_REC_HEADER_FIELD_SIZE;

	/* Get the type length */
	record->type_len = msg[offset];
	offset += NDEF_REC_TYPE_LENGTH_FIELD_SIZE;

	/* Extract the payload length */
	if(is_sr)
	{
		record->payload_len = msg[offset];
		offset += NDEF_REC_PAYLOAD_LENGTH_FIELD_SIZE_SHORT;
	}
	else
	{
		uint8_t* p_data = (uint8_t*)(&record->payload_len);
		memcpy(p_data, &msg[offset], NDEF_REC_PAYLOAD_LENGTH_FIELD_SIZE);
		offset += NDEF_REC_PAYLOAD_LENGTH_FIELD_SIZE;
	}

	/* Get the ID length */
	if(is_il)
	{
		record->id_len = msg[offset];
		offset += NDEF_REC_ID_LENGTH_FIELD_SIZE;
	}

	/* Type field begins at this offset, update it */
	record->type_offset = offset;

	/* ID field */
	record->id_offset = record->type_offset + record->type_len;

	/* Payload field */
	record->payload_offset = record->id_offset + record->id_len;

	/* Record length */
	record->len = record->payload_offset + record->payload_len;

	return;
}


void ndef_payload_parse(ndef_layout_t* record, uint8_t* p_payload)
{
	uint8_t* type = NULL;

	/* Find the type */
	type = &(record->msg[record->type_offset]);

	if(record->type_len == 0x01)
	{
		/* Refer Section 3 - NFCForum-TS-RTD_Text_1.0 */
		if(*type == NDEF_TEXT_RECORD_TYPE)
		{
			/*this is a text record*/
			uint8_t status_byte = record->msg[record->payload_offset + NDEF_TEXT_RECORD_STATUS_BYTE_OFFSET];
			uint8_t iana_code_len = status_byte & NDEF_TEXT_RECORD_IANA_CODE_LEN_MASK;

			uint8_t text_offset = sizeof(status_byte) + iana_code_len;

			uint8_t text_len = record->payload_len - (sizeof(status_byte) + iana_code_len);

			memcpy(p_payload, &record->msg[record->payload_offset + text_offset], text_len);
		}

	}
}
