/**
 * @nfc_api.h
 * @brief Header file for NFC related A.P.I
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
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

#ifndef NFC_API_H_
#define NFC_API_H_

/**
 * @brief Initialize NFC hardware and global variables
 */
void NFC_Initialize(void);

/**
 * @brief Execute runtime NFC handler
 */
void NFC_Handler(void);

/**
 * @brief Reset the Data blocks in the Tag Memory Region
 */
void NFC_ClearTagData(void);

/**
 * @brief Write to the Data blocks in the Tag Memory Region
 *
 *  @param[in]	block_num	Block number to write to
 *  @param[in]	p_data		Pointer to the data to be written
 */
void NFC_Write2TagData(uint8_t block_num, uint8_t *pdata);

/**
 * @brief Write to the Data blocks in the Tag Memory Region
 *
 *  @param[in]	p_resp		Pointer to the response buffer
 *  @param[in]	block_num	Block number to write to
 *
 *
 */
void NFC_ReadFromTagData(uint8_t *ptr_resp, uint8_t block_num);

/**
 * @brief Process the read command from PCD and respond
 *
 *  @param[in]	block_num	Block number to read the response from
 *
 *
 */
void NFC_Read(uint8_t block_num);

/**
 * @brief Process the write command from PCD and respond
 *
 *  @param[in]	block_num	Block number to read the data to
 *  @param[in]	pData		Pointer to the data to be written
 *
 */
void NFC_Write(uint8_t block_num, uint8_t *pData);


#endif    /* ifndef NFC_API_H_ */
