/*
 * CANSPI.h
 *
 *  Created on: Mar 8, 2026
 *  Based on Microchip Drivers for Arduino
 *  Modified by: Caroline Nguyen
 *  Contact: c_nguyen@insa-toulouse.fr
 */

#ifndef __CAN_SPI_H
#define	__CAN_SPI_H

/* ================================================================
 * 								Includes
 * ================================================================ */
#include "main.h"
#include "MCP2515.h"
/* ================================================================
 * 								Defines
 * ================================================================ */
#define dSTANDARD_CAN_MSG_ID_2_0B 1
#define dEXTENDED_CAN_MSG_ID_2_0B 2 // CAN Extended

#define CAN_ID_STD    	0x00000000U
#define CAN_ID_EXT    	0x00000004U
#define CAN_RTR_DATA  	0x00000000U
#define CAN_RTR_REMOTE 	0x00000002U



/* ================================================================
 * 							   Structures
 * ================================================================ */
/**
  * @brief  CAN Tx message header structure definition
  */
typedef struct
{
  uint32_t StdId;    /*!< Specifies the standard identifier.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x7FF. */
  uint32_t ExtId;    /*!< Specifies the extended identifier.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x1FFFFFFF. */
  uint32_t IDE;      /*!< Specifies the type of identifier for the message that will be transmitted.
                          This parameter can be a value of @ref CAN_identifier_type */
  uint32_t RTR;      /*!< Specifies the type of frame for the message that will be transmitted.
                          This parameter can be a value of @ref CAN_remote_transmission_request */
  uint32_t DLC;      /*!< Specifies the length of the frame that will be transmitted.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 8. */
  FunctionalState TransmitGlobalTime; /*!< Specifies whether the timestamp counter value captured on start
                          of frame transmission, is sent in DATA6 and DATA7 replacing pData[6] and pData[7].
                          @note: Time Triggered Communication Mode must be enabled.
                          @note: DLC must be programmed as 8 bytes, in order these 2 bytes are sent.
                          This parameter can be set to ENABLE or DISABLE. */
} CAN_TxHeaderTypeDef;

/**
  * @brief  CAN Rx message header structure definition
  */
typedef struct
{
  uint32_t StdId;    /*!< Specifies the standard identifier.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x7FF. */
  uint32_t ExtId;    /*!< Specifies the extended identifier.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x1FFFFFFF. */
  uint32_t IDE;      /*!< Specifies the type of identifier for the message that will be transmitted.
                          This parameter can be a value of @ref CAN_identifier_type */
  uint32_t RTR;      /*!< Specifies the type of frame for the message that will be transmitted.
                          This parameter can be a value of @ref CAN_remote_transmission_request */
  uint32_t DLC;      /*!< Specifies the length of the frame that will be transmitted.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 8. */
  uint32_t Timestamp; /*!< Specifies the timestamp counter value captured on start of frame reception.
                          @note: Time Triggered Communication Mode must be enabled.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 0xFFFF. */
  uint32_t FilterMatchIndex; /*!< Specifies the index of matching acceptance filter element.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 0xFF. */
} CAN_RxHeaderTypeDef;

/* Error codes for CANSPI */
typedef enum
{
    CANSPI_OK           = 0x00U,
    CANSPI_ERROR        = 0x01U,
    CANSPI_BUSY         = 0x02U,
    CANSPI_TIMEOUT      = 0x03U,
    CANSPI_INVALID_ARG  = 0x04U,
    CANSPI_NO_MSG       = 0x05U,
	CANSPI_BUS_OFF		= 0x06U,
    CANSPI_ERR_PASSIVE  = 0x07U,
} CANSPI_StatusTypeDef;


/* ================================================================
 * 					Public Prototype Functions
 * ================================================================ */
CANSPI_StatusTypeDef CANSPI_Initialize(void);
void CANSPI_Sleep(void);
CANSPI_StatusTypeDef CANSPI_Transmit(CAN_TxHeaderTypeDef *txHeader, uint8_t *txData);
CANSPI_StatusTypeDef CANSPI_Receive(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData);
uint8_t CANSPI_messagesInBuffer(void);
CANSPI_StatusTypeDef CANSPI_isBussOff(void);
CANSPI_StatusTypeDef CANSPI_isRxErrorPassive(void);
CANSPI_StatusTypeDef CANSPI_isTxErrorPassive(void);
#endif	/* __CAN_SPI_H */
