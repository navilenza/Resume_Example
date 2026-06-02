/*
 * CANSPI.c
 *
 *  Created on: Mar 8, 2026
 *  Based on Microchip Drivers for Arduino
 *  Modified by: Caroline Nguyen
 *  Contact: c_nguyen@insa-toulouse.fr
 *  CAN SPI Driver made to match the TIM CAN bus
 */

/* ================================================================
 * 							Includes
 * ================================================================ */
#include "CANSPI.h"


/* ================================================================
 * 					Private Function Prototypes
 * ================================================================ */
static uint32_t convertReg2ExtendedCANid(uint8_t tempRXBn_EIDH, uint8_t tempRXBn_EIDL, uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL);
static uint32_t convertReg2StandardCANid(uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL) ;
static void convertCANid2Reg(uint32_t tempPassedInID, uint8_t canIdType, id_reg_t *passedIdReg);
void CAN_Test_Conversion(void);
/* ================================================================
 * 						Private Variable
 * ================================================================ */
static ctrl_status_t ctrlStatus;
static ctrl_error_status_t errorStatus;
static id_reg_t idReg;

/* ================================================================
 * 					Public Function definition
 * ================================================================ */

/**
 * @brief   Put the MCP2515 in Sleep Mode
 * @param   void
 * @return  void
 */
void CANSPI_Sleep(void)
{
	/* Clear CAN bus wakeup interrupt */
	MCP2515_BitModify(MCP2515_CANINTF, 0x40, 0x00);

	/* Enable CAN bus activity wakeup */
	MCP2515_BitModify(MCP2515_CANINTE, 0x40, 0x40);

	MCP2515_SetSleepMode();
}

/**
 * @brief   Initialize the register for the SPI connection
 * @param   void
 * @return  CANSPI_ERROR
 * @retval  CANSPI_OK
 */
CANSPI_StatusTypeDef CANSPI_Initialize(void)
{
	RXF0 RXF0reg;
	RXF1 RXF1reg;
	RXF2 RXF2reg;
	RXF3 RXF3reg;
	RXF4 RXF4reg;
	RXF5 RXF5reg;
	RXM0 RXM0reg;
	RXM1 RXM1reg;

	/* Intialize Rx Mask values */
	RXM0reg.RXM0SIDH = 0x00;
	RXM0reg.RXM0SIDL = 0x00;
	RXM0reg.RXM0EID8 = 0x00;
	RXM0reg.RXM0EID0 = 0x00;

	RXM1reg.RXM1SIDH = 0x00;
	RXM1reg.RXM1SIDL = 0x00;
	RXM1reg.RXM1EID8 = 0x00;
	RXM1reg.RXM1EID0 = 0x00;

	/* Intialize Rx Filter values */
	RXF0reg.RXF0SIDH = 0x00;
	RXF0reg.RXF0SIDL = 0x00;      //Standard Filter
	RXF0reg.RXF0EID8 = 0x00;
	RXF0reg.RXF0EID0 = 0x00;

	RXF1reg.RXF1SIDH = 0x00;
	RXF1reg.RXF1SIDL = 0x08;      //Extended Filter
	RXF1reg.RXF1EID8 = 0x00;
	RXF1reg.RXF1EID0 = 0x00;

	RXF2reg.RXF2SIDH = 0x00;
	RXF2reg.RXF2SIDL = 0x00;
	RXF2reg.RXF2EID8 = 0x00;
	RXF2reg.RXF2EID0 = 0x00;

	RXF3reg.RXF3SIDH = 0x00;
	RXF3reg.RXF3SIDL = 0x00;
	RXF3reg.RXF3EID8 = 0x00;
	RXF3reg.RXF3EID0 = 0x00;

	RXF4reg.RXF4SIDH = 0x00;
	RXF4reg.RXF4SIDL = 0x00;
	RXF4reg.RXF4EID8 = 0x00;
	RXF4reg.RXF4EID0 = 0x00;

	RXF5reg.RXF5SIDH = 0x00;
	RXF5reg.RXF5SIDL = 0x08;
	RXF5reg.RXF5EID8 = 0x00;
	RXF5reg.RXF5EID0 = 0x00;

	/* Intialize MCP2515, check SPI */
	if(MCP2515_Initialize()!= MCP2515_OK)
	{
		MCP2515_ERR_MSG("SPI communication failed");
		return CANSPI_ERROR;
	}

	if(Verify_Oscillator_Presence() != MCP2515_OK)
	{
		MCP2515_ERR_MSG("Quartz not responding");
		return CANSPI_ERROR;
	}
	/* Change mode as configuration mode */
	if(MCP2515_SetConfigMode() != MCP2515_OK)
	{
		MCP2515_ERR_MSG("Switch to config mode failed");
		return CANSPI_ERROR;
	}

	/* Configure filter & mask */
	MCP2515_WriteByteSequence(MCP2515_RXM0SIDH, MCP2515_RXM0EID0, &(RXM0reg.RXM0SIDH));
	MCP2515_WriteByteSequence(MCP2515_RXM1SIDH, MCP2515_RXM1EID0, &(RXM1reg.RXM1SIDH));
	MCP2515_WriteByteSequence(MCP2515_RXF0SIDH, MCP2515_RXF0EID0, &(RXF0reg.RXF0SIDH));
	MCP2515_WriteByteSequence(MCP2515_RXF1SIDH, MCP2515_RXF1EID0, &(RXF1reg.RXF1SIDH));
	MCP2515_WriteByteSequence(MCP2515_RXF2SIDH, MCP2515_RXF2EID0, &(RXF2reg.RXF2SIDH));
	MCP2515_WriteByteSequence(MCP2515_RXF3SIDH, MCP2515_RXF3EID0, &(RXF3reg.RXF3SIDH));
	MCP2515_WriteByteSequence(MCP2515_RXF4SIDH, MCP2515_RXF4EID0, &(RXF4reg.RXF4SIDH));
	MCP2515_WriteByteSequence(MCP2515_RXF5SIDH, MCP2515_RXF5EID0, &(RXF5reg.RXF5SIDH));


	/* Accept All (Standard + Extended) */
	MCP2515_WriteByte(MCP2515_RXB0CTRL, 0x04);    //Enable BUKT, Accept Filter 0
	MCP2515_WriteByte(MCP2515_RXB1CTRL, 0x01);    //Accept Filter 1

	/*
	 * Explanation for the calculation needed to parameter the CAN bus
	 * Matching the parameter from the TIM CAN:
	 * Quartz used for MCP2515 : 8 MHz
	 * Time Quantum = 250 ns
	 * Time for one bit : 4000 ns
	 * Baud Rate : 250 kbit/s
	 * tq = 2 * (BRP(0) + 1) / 8000000 = 0.250us
	 * tbit = (SYNC_SEG(1 fixed) + PROP_SEG + PS1 + PS2)
	 * tbit = 1tq + 5tq + 6tq + 4tq = 16tq
	 * 16tq * 250ns = 4000ns
	 */
	/* 00(SJW 1tq) 000000(BRP=0) */
	MCP2515_WriteByte(MCP2515_CNF1, 0x00);
	/* 1(BTLMODE) 1(SAM) 101(PS1=6tq) 100(PROP=5tq) */
	MCP2515_WriteByte(MCP2515_CNF2, 0xE5);
	/* 1(SOF) 0(WAKFIL) 000 011(PS2=4tq) */
	MCP2515_WriteByte(MCP2515_CNF3, 0x83);

	/* Normal mode */
	if(MCP2515_SetNormalMode() != MCP2515_OK)
	{
		MCP2515_ERR_MSG("Switch to normal mode failed");
		return CANSPI_ERROR;
	}

	/* Enable One-Shot Mode (OSM) */
	MCP2515_BitModify(MCP2515_CANCTRL, 0x08, 0x08);
	MCP2515_DBG_MSG("MCP2515 Init OK");

	return CANSPI_OK;
}

/**
 * @brief   Transmit CAN message
 * @param   txHeader    Pointer to CAN Tx header (StdId/ExtId, IDE, RTR, DLC)
 * @param   txData      Pointer to data buffer (up to 8 bytes)
 * @return  CANSPI_OK, CANSPI_BUSY, or CANSPI_INVALID_ARG
 */
CANSPI_StatusTypeDef CANSPI_Transmit(CAN_TxHeaderTypeDef *txHeader, uint8_t *txData)
{
	if (txHeader == NULL || txData == NULL)
	{
		CAN_ERR_MSG("Invalid Argument in CANSPI_Transmit");
		return CANSPI_INVALID_ARG;
	}

	idReg.tempSIDH = 0;
	idReg.tempSIDL = 0;
	idReg.tempEID8 = 0;
	idReg.tempEID0 = 0;

	uint32_t canId;
	uint8_t  canIdType;

	if (txHeader->IDE == CAN_ID_EXT)
	{
		canId     = txHeader->ExtId;
		canIdType = dEXTENDED_CAN_MSG_ID_2_0B;
	}
	else
	{
		canId     = txHeader->StdId;
		canIdType = dSTANDARD_CAN_MSG_ID_2_0B;
	}

	convertCANid2Reg(canId, canIdType, &idReg);

	ctrlStatus.ctrl_status = MCP2515_ReadStatus();

	uint8_t targetBuffer = 0xFF;
	uint8_t rtsCommand = 0;
	uint8_t loadCommand = 0;

	if (ctrlStatus.TXB0REQ == 0) {
		targetBuffer = MCP2515_TXB0CTRL;
		loadCommand  = MCP2515_LOAD_TXB0SIDH;
		rtsCommand   = MCP2515_RTS_TX0;
	} else if (ctrlStatus.TXB1REQ == 0) {
		targetBuffer = MCP2515_TXB1CTRL;
		loadCommand  = MCP2515_LOAD_TXB1SIDH;
		rtsCommand   = MCP2515_RTS_TX1;
	} else if (ctrlStatus.TXB2REQ == 0) {
		targetBuffer = MCP2515_TXB2CTRL;
		loadCommand  = MCP2515_LOAD_TXB2SIDH;
		rtsCommand   = MCP2515_RTS_TX2;
	}

	if (targetBuffer != 0xFF) {
		MCP2515_BitModify(targetBuffer, 0x08, 0x00);
		MCP2515_LoadTxSequence(loadCommand, &(idReg.tempSIDH), txHeader->DLC, txData);
		MCP2515_RequestToSend(rtsCommand);

		CAN_DBG_MSG("TX Success: Buffer=0x%02X, ID=0x%08" PRIX32 ", DLC=%" PRIu32 ", Data=[0x%02X, 0x%02X]",loadCommand, canId, txHeader->DLC, txData[0], (txHeader->DLC > 1) ? txData[1] : 0);
		return CANSPI_OK;
	}
	CAN_ERR_MSG("CAN Busy can't Transmit");
	return CANSPI_BUSY;
}
/**
 * @brief   Receive CAN message
 * @param   rxHeader    Pointer to CAN Rx header to fill (StdId/ExtId, IDE, RTR, DLC)
 * @param   rxData      Pointer to data buffer to fill (up to 8 bytes)
 * @return  CANSPI_OK, CANSPI_NO_MSG, or CANSPI_INVALID_ARG
 */
CANSPI_StatusTypeDef CANSPI_Receive(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData)
{
	rx_reg_t         rxReg;
	ctrl_rx_status_t rxStatus;

	/* Check for early error*/
	if (rxHeader == NULL || rxData == NULL)
	{
		CAN_ERR_MSG("Invalid Argument in CANSPI_Receive");
		return CANSPI_INVALID_ARG;
	}

	rxStatus.ctrl_rx_status = MCP2515_GetRxStatus();

	if (rxStatus.rxBuffer == 0)
	{
		CAN_ERR_MSG("No msg in MCP2515 Rx Buffer");
		return CANSPI_NO_MSG;
	}

	uint8_t rxInstruction = 0; // To track which buffer we read for the log

	/* Read the different Rx buffer */
	if ((rxStatus.rxBuffer == MSG_IN_RXB0) || (rxStatus.rxBuffer == MSG_IN_BOTH_BUFFERS))
	{
		rxInstruction = MCP2515_READ_RXB0SIDH;
		MCP2515_ReadRxSequence(rxInstruction, rxReg.rx_reg_array, sizeof(rxReg.rx_reg_array));
		MCP2515_BitModify(MCP2515_CANINTF, 0x01, 0x00);  // Clear RX0IF
	}
	else if (rxStatus.rxBuffer == MSG_IN_RXB1)
	{
		rxInstruction = MCP2515_READ_RXB1SIDH;
		MCP2515_ReadRxSequence(rxInstruction, rxReg.rx_reg_array, sizeof(rxReg.rx_reg_array));
		MCP2515_BitModify(MCP2515_CANINTF, 0x02, 0x00);  // Clear RX1IF
	}

	/* Convert the rx register to a standard or extended id */
	if (rxStatus.msgType == dEXTENDED_CAN_MSG_ID_2_0B)
	{
		rxHeader->IDE   = CAN_ID_EXT;
		rxHeader->ExtId = convertReg2ExtendedCANid(rxReg.RXBnEID8, rxReg.RXBnEID0,
				rxReg.RXBnSIDH, rxReg.RXBnSIDL);
		rxHeader->StdId = 0;
	}
	else
	{
		CAN_ERR_MSG("Error Standard ID detected");
		rxHeader->IDE   = CAN_ID_STD;
		rxHeader->StdId = convertReg2StandardCANid(rxReg.RXBnSIDH, rxReg.RXBnSIDL);
		rxHeader->ExtId = 0;
	}


    rxHeader->RTR              = CAN_RTR_DATA;
    rxHeader->DLC              = rxReg.RXBnDLC;
    rxHeader->Timestamp        = 0;
    rxHeader->FilterMatchIndex = 0;

    rxData[0] = rxReg.RXBnD0;
    rxData[1] = rxReg.RXBnD1;
    rxData[2] = rxReg.RXBnD2;
    rxData[3] = rxReg.RXBnD3;
    rxData[4] = rxReg.RXBnD4;
    rxData[5] = rxReg.RXBnD5;
    rxData[6] = rxReg.RXBnD6;
    rxData[7] = rxReg.RXBnD7;
//	}
	/* Debug */
	CAN_DBG_MSG("Reception success : Instruction (register) = %" PRIu8 ", CAN_ID = 0x%" PRIX32 ", DLC = %" PRIu32 ", DataPtr = %p",
	            (uint8_t)rxInstruction,rxHeader->ExtId,(uint32_t)rxHeader->DLC, (void*)rxData);
	return CANSPI_OK;
}

/*
 * @brief   Check message buffer
 * @param   void
 * @return  count, the number of message in buffer
 */
uint8_t CANSPI_messagesInBuffer(void)
{
	uint8_t messageCount = 0;

	ctrlStatus.ctrl_status = MCP2515_ReadStatus();

	if(ctrlStatus.RX0IF != 0)
	{
		messageCount++;
	}

	if(ctrlStatus.RX1IF != 0)
	{
		messageCount++;
	}
	CAN_DBG_MSG("Number of message in buffer: %u ", messageCount);
	return (messageCount);
}

/*
 * @brief   Check if the bus is off
 * @param   void
 * @return  CANSPI_StatusTypeDef
 */
CANSPI_StatusTypeDef CANSPI_isBussOff(void)
{
	errorStatus.error_flag_reg = MCP2515_ReadByte(MCP2515_EFLG);

	if(errorStatus.TXBO == 1)
	{
		CAN_ERR_MSG("CAN BUS OFF");
		return CANSPI_BUS_OFF;
	}

	return CANSPI_OK;
}

/*
 * @brief   Check for Rx Passive Error register
 * @param   void
 * @return  CANSPI_StatusTypeDef
 */
CANSPI_StatusTypeDef CANSPI_isRxErrorPassive(void)
{
	errorStatus.error_flag_reg = MCP2515_ReadByte(MCP2515_EFLG);

	if(errorStatus.RXEP == 1)
	{
		CAN_ERR_MSG("CAN passive Rx Error");
		return CANSPI_ERR_PASSIVE;
	}

	return CANSPI_OK;
}

/*
 * @brief   Check for Tx Passive Error register
 * @param   void
 * @return  CANSPI_StatusTypeDef
 */
CANSPI_StatusTypeDef CANSPI_isTxErrorPassive(void)
{
	errorStatus.error_flag_reg = MCP2515_ReadByte(MCP2515_EFLG);

	if(errorStatus.TXEP == 1)
	{
		CAN_ERR_MSG("CAN passive Tx Error");
		return CANSPI_ERR_PASSIVE;
	}

	return CANSPI_OK;
}

/*
 * @brief   Convert register value to extended CAN ID
 * @param[in] tempRXBn_EIDH
 * @param[in] tempRXBn_EIDL
 * @param[in] tempRXBn_SIDH
 * @param[in] tempRXBn_SIDL
 * @return  Extended CAN ID
 */
static uint32_t convertReg2ExtendedCANid(uint8_t tempRXBn_EIDH, uint8_t tempRXBn_EIDL, uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL) 
{
	uint32_t returnValue = 0;
	uint32_t ConvertedID = 0;
	uint8_t CAN_standardLo_ID_lo2bits;
	uint8_t CAN_standardLo_ID_hi3bits;

	CAN_standardLo_ID_lo2bits = (tempRXBn_SIDL & 0x03);
	CAN_standardLo_ID_hi3bits = (tempRXBn_SIDL >> 5);
	ConvertedID = (tempRXBn_SIDH << 3);
	ConvertedID = ConvertedID + CAN_standardLo_ID_hi3bits;
	ConvertedID = (ConvertedID << 2);
	ConvertedID = ConvertedID + CAN_standardLo_ID_lo2bits;
	ConvertedID = (ConvertedID << 8);
	ConvertedID = ConvertedID + tempRXBn_EIDH;
	ConvertedID = (ConvertedID << 8);
	ConvertedID = ConvertedID + tempRXBn_EIDL;
	returnValue = ConvertedID;

	CAN_DBG_MSG("ID Conversion: EIDH=0x%02X, EIDL=0x%02X, SIDH=0x%02X, SIDL=0x%02X -> ExtID: %" PRIu32,
			tempRXBn_EIDH, tempRXBn_EIDL, tempRXBn_SIDH, tempRXBn_SIDL, ConvertedID);
	return (returnValue);
}

/*
 * @brief   Convert register value to Standard CAN ID
 * @param[in] tempRXBn_SIDH
 * @param[in] tempRXBn_SIDL
 * @return  Standard CAN ID
 */
static uint32_t convertReg2StandardCANid(uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL) 
{
	uint32_t standardId;

	// SIDH contains bits 10-3, SIDL bits 7-5 contain bits 2-0
	standardId = ((uint32_t)tempRXBn_SIDH << 3) | (tempRXBn_SIDL >> 5);

	CAN_DBG_MSG("ID Conversion: SIDH=0x%02X, SIDL=0x%02X -> StdID: %" PRIu32, tempRXBn_SIDH, tempRXBn_SIDL, standardId);
	return standardId;
}

/*
 * @brief   Convert CAN ID to register value
 * @param[in] tempPassedInID
 * @param[in] canIdType
 * @param[in] *passedIdReg
 * @return  None
 */
static void convertCANid2Reg(uint32_t tempPassedInID, uint8_t canIdType, id_reg_t *passedIdReg)
{
	if (passedIdReg == NULL) return;

	if (canIdType == dEXTENDED_CAN_MSG_ID_2_0B)
	{
		// Extended ID (29 bits)
		passedIdReg->tempEID0 = (uint8_t) (tempPassedInID & 0xFF);
		passedIdReg->tempEID8 = (uint8_t) ((tempPassedInID >> 8) & 0xFF);

		// SIDL Mapping:
		// bits 16-17 -> SIDL 0-1
		// bit 3      -> EXIDE Set (1)
		// bits 18-20 -> SIDL 5-7
		passedIdReg->tempSIDL = (uint8_t) ((tempPassedInID >> 16) & 0x03);
		passedIdReg->tempSIDL |= 0x08;
		passedIdReg->tempSIDL |= (uint8_t) (((tempPassedInID >> 18) & 0x07) << 5);

		passedIdReg->tempSIDH = (uint8_t) ((tempPassedInID >> 21) & 0xFF);

		CAN_DBG_MSG("ID To Reg (EXT): ID %" PRIu32 " -> SIDH:0x%02X, SIDL:0x%02X, EID8:0x%02X, EID0:0x%02X",
		            tempPassedInID, passedIdReg->tempSIDH, passedIdReg->tempSIDL, passedIdReg->tempEID8, passedIdReg->tempEID0);
	}
	else
	{
		// Standard ID (11 bits)
		// Ensure the high registers are wiped and EXIDE bit (bit 3) is 0
		passedIdReg->tempEID8 = 0;
		passedIdReg->tempEID0 = 0;
		passedIdReg->tempSIDL = (uint8_t) ((tempPassedInID & 0x07) << 5);
		passedIdReg->tempSIDH = (uint8_t) ((tempPassedInID >> 3) & 0xFF);

		CAN_DBG_MSG("ID To Reg (STD): ID %" PRIu32 " -> SIDH:0x%02X, SIDL:0x%02X",tempPassedInID, passedIdReg->tempSIDH, passedIdReg->tempSIDL);	}
}

/*
 * @brief   Test of the conversion with some CAN Extended ID
 * @param[in] None
 * @return  None
 */
void CAN_Test_Conversion(void)
{
	uint32_t test_ids[] = {
			0x04620000,
			0x4223002,
			0x4203002,
			0x460fe02,
			0x404fe02,
			0x1FFFFFFF,
			0x00000000,
	};

	for (int i = 0; i < 7; i++)
	{
		uint32_t original_id = test_ids[i];
		id_reg_t reg = {0};

		/* ENCODE: ID to registers*/
		convertCANid2Reg(original_id, dEXTENDED_CAN_MSG_ID_2_0B, &reg);

		/* DECODE old vs new */
		uint32_t decoded_new = convertReg2ExtendedCANid(reg.tempEID8, reg.tempEID0, reg.tempSIDH, reg.tempSIDL);

		CAN_DBG_MSG("--- Test %d ---\r\n", i);
		CAN_DBG_MSG("Original : 0x%08lX\r\n", original_id);
		CAN_DBG_MSG("Regs: SIDH=0x%02X SIDL=0x%02X EID8=0x%02X EID0=0x%02X\r\n",reg.tempSIDH, reg.tempSIDL, reg.tempEID8, reg.tempEID0);
		CAN_DBG_MSG("New decode: 0x%08lX  %s\r\n\r\n", decoded_new, (original_id == decoded_new) ? "OK" : "FAIL");
	}
}
