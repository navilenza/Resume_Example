/*
 * CAN_Controller.c
 *
 *  Created on: Mar 8, 2026
 *      Author: Caroline Nguyen
 *      Contact: c_nguyen@insa-toulouse.fr
 *
 * Based on MCP2515 CAN Drivers and Steering_Wheel_v2 (TIM)
 */

/* ================================================================
 * 								Includes
 * ================================================================ */

#include "CAN_Controller.h"
/* ================================================================
 *                       		Variables
 * ================================================================ */
uint8_t ping_pending  = 1;  /* 0 = no ping sent */
uint8_t pingpong_count = 0;

/* ================================================================
 *                      Private Functions
 * ================================================================ */
CANSPI_StatusTypeDef CAN_SendMessage(uint8_t src, uint8_t dst, uint16_t cmd, uint8_t dlc, uint8_t *data);

/**
 * @brief   Sends a Ping Frame to test CAN activity and send it over BLE
 * @param[in] CAN_RxHeader   Received CAN frame header
 * @return  CANSPI_StatusTypeDef
 * @retval  CANSPI_TIMEOUT
 * @retval  CANSPI_OK
 * @retval	CANSPI_BUSY
 */
CANSPI_StatusTypeDef CAN_CheckPingPong(CAN_RxHeaderTypeDef CAN_RxHeader)
{

    if (CAN_RxHeader.ExtId == ((uint32_t)TIM_PONG << something))
    {
        pingpong_count = 0;
        ping_pending  = 0;
    	TIM_APP_DBG_MSG("RX: Ping Pong \n");
        return CANSPI_OK;
    }
    else if (pingpong_count >= CAN_PINGPONG_TIMEOUT)
    {
        /* BLE_Notify(timeout_payload, len) */

    	TIM_APP_ERR_MSG("TX/RX Fail:Ping timeout \n");
        return CANSPI_TIMEOUT;
    }
    else
    {
        pingpong_count++;
        return CANSPI_BUSY;
    }
}

/* ================================================================
 * 					Public Function definition
 * ================================================================ */
/**
 * @brief   Sends a CAN frame via the MCP2515 SPI CAN controller
 * @param[in] address  CAN identifier (CAN extended)
 * @param[in] dlc      Data Length
 * @param[in] message  Pointer to the data buffer
 *
 * @return  CANSPI_StatusTypeDef
 * @retval  CANSPI_OK           Frame loaded and RTS sent.
 * @retval  CANSPI_BUSY         All TX buffers occupied.
 * @retval  CANSPI_INVALID_ARG  NULL pointer or dlc > 8.
 */
CANSPI_StatusTypeDef CAN_SendMessage(uint8_t src, uint8_t dst, uint16_t cmd, uint8_t dlc, uint8_t *data)
{

    if (data == NULL || dlc > 8)
    	{
    	TIM_APP_ERR_MSG("Invalid Argument in CAN_SendMessage \n");
    	return CANSPI_INVALID_ARG;
    	}

    /* Create the CAN Extended Id by concatenating each field of 8 bits like this :
     * cmd + dest + source
     */
    uint32_t address;
    address = ((uint32_t)(something) << something) | ((uint32_t)(something) << something) | ((uint32_t)(something));

    CAN_TxHeaderTypeDef txHeader;
    txHeader.DLC   = dlc;
    txHeader.ExtId = address & 0x1FFFFFFF;
    txHeader.StdId = 0;
    txHeader.IDE   = CAN_ID_EXT;
    txHeader.RTR   = CAN_RTR_DATA;

    return CANSPI_Transmit(&txHeader, data);
}

/**
 * @brief   Sends a Ping Frame to test CAN activity
 * @param[in] void
 * @return  CANSPI_StatusTypeDef
 * @retval  CANSPI_BUS_OFF
 * @retval  CANSPI_OK
 */
CANSPI_StatusTypeDef CAN_SendPingPong(void)
{
    if (CANSPI_isBussOff() == CANSPI_BUS_OFF)
    {
//        return CANSPI_BUS_OFF;
    }
    uint8_t ping = CAN_PING_BYTE;
    CANSPI_StatusTypeDef txStatus = CAN_SendMessage(STEERING_WHEEL_ADDR, BROADCAST_ADDR, TIM_PING, TIM_PING_LENGTH, &ping);

    if (txStatus != CANSPI_OK)
    {
    	TIM_APP_ERR_MSG("TX Fail: Ping not send \n");
        return txStatus;
    }
    /* ping pong variable status change */
    ping_pending  = 1;
    pingpong_count = 0;
    TIM_APP_DBG_MSG("TX: Ping sent, waiting for pong \n");

    return CANSPI_OK;
}


/* ================================================================
 *                      CAN Motors Command
 * ================================================================ */
/**
 * @brief   Sends the order to Accelerate to the Motor
 * @param[in] void
 * @return  CANSPI_StatusTypeDef
 * @retval  CANSPI_INVALID_ARG
 * @retval  CANSPI_BUSY
 * @retval  CANSPI_OK
 */
CANSPI_StatusTypeDef CAN_ElecMotorEnable(void)
{
    uint8_t cmd = 1;
    CANSPI_StatusTypeDef status = CAN_SendMessage(STEERING_WHEEL_ADDR, ELECTRICAL_MOTOR_ADDR, TIM_ELEC_MOTOR_CMD, 1, &cmd);
    if (status != CANSPI_OK)
    {
    	TIM_APP_ERR_MSG("TX Fail: CAN_ElecMotorEnable\n");
    }
    else
    {
    	TIM_APP_DBG_MSG("TX: CAN_ElecMotorEnable \n");
    }
    return status;
}

/**
 * @brief   Sends the order to Stop the Motor
 * @param[in] void
 * @return  CANSPI_StatusTypeDef
 * @retval  CANSPI_INVALID_ARG
 * @retval  CANSPI_BUSY
 * @retval  CANSPI_OK
 */
CANSPI_StatusTypeDef CAN_ElecMotorDisable(void)
{
    uint8_t cmd = 0;
    CANSPI_StatusTypeDef status = CAN_SendMessage(STEERING_WHEEL_ADDR, ELECTRICAL_MOTOR_ADDR, TIM_ELEC_MOTOR_CMD, 1, &cmd);
    if (status != CANSPI_OK)
    {
    	TIM_APP_ERR_MSG("TX Fail: CAN_ElecMotorDisable \n");
    }
    else
    {
		TIM_APP_DBG_MSG("TX: CAN_ElecMotorDisable \n");
    }
    return status;
}

/**
 * @brief   Send the Current Command based on the Carto Mode
 * @param[in] Current_Cmd
 * @return  CANSPI_StatusTypeDef
 * @retval  CANSPI_INVALID_ARG
 * @retval  CANSPI_BUSY
 * @retval  CANSPI_OK
 */
CANSPI_StatusTypeDef CAN_ElecMotorForward(uint16_t Current_Cmd)
{
	uint8_t cmd[2] = { ((Current_Cmd * MOTOR_PROPORTIONAL_GAIN) >> something) & 0xFF, (Current_Cmd * MOTOR_PROPORTIONAL_GAIN) & 0xFF };
	CANSPI_StatusTypeDef status = CAN_SendMessage(STEERING_WHEEL_ADDR, ELECTRICAL_MOTOR_ADDR, TIM_CMD_ELEC_MOTOR_FORWARD, TIM_ELEC_MOTOR_FORWARD_LENGTH, cmd);
    if (status != CANSPI_OK)
    {
    	TIM_APP_ERR_MSG("TX Fail: CAN_ElecMotorForward \n");
    }
    else
    {
		TIM_APP_DBG_MSG("TX: CAN_ElecMotorForward \n");
    }
    return status;
}
/* ================================================================
 *                      CAN Lights Command
 * ================================================================ */

/**
 * @brief   Send the Signal Headlights On
 * @param[in] void
 * @return  CANSPI_StatusTypeDef
 * @retval  CANSPI_INVALID_ARG
 * @retval  CANSPI_BUSY
 * @retval  CANSPI_OK
 */
CANSPI_StatusTypeDef CAN_HeadlightOn(void)
{
	uint8_t headlight_status = 1;
	CANSPI_StatusTypeDef status = CAN_SendMessage(STEERING_WHEEL_ADDR, GLOBAL_LIGHT_ADDR, TIM_LIGHTS_CMD, TIM_LIGHTS_LENGTH, &headlight_status);
    if (status != CANSPI_OK)
    {
    	TIM_APP_ERR_MSG("TX Fail: CAN_HeadlightOn \n");
    }
    else
    {
		TIM_APP_DBG_MSG("TX: CAN_HeadlightOn \n");
    }
    return status;
}

/**
 * @brief   Send the Signal Blinker On
 * @param[in] void
 * @return  CANSPI_StatusTypeDef
 * @retval  CANSPI_INVALID_ARG
 * @retval  CANSPI_BUSY
 * @retval  CANSPI_OK
 */
CANSPI_StatusTypeDef CAN_Blinker_On(uint8_t blinker_state)
{
    uint8_t blinker = blinker_state;
    CANSPI_StatusTypeDef status = CAN_SendMessage(STEERING_WHEEL_ADDR, GLOBAL_LIGHT_ADDR, TIM_BLINKERS_CMD, TIM_BLINKERS_LENGTH, &blinker);
    if (status != CANSPI_OK)
    {
    	TIM_APP_ERR_MSG("TX Fail: CAN_Blinker_On \n");
    }
    else
    {
		TIM_APP_DBG_MSG("TX: CAN_Blinker_On \n");
    }
    return status;
}

/**
 * @brief   Send the Signal to set the Stop Light
 * @param[in] void
 * @return  CANSPI_StatusTypeDef
 * @retval  CANSPI_INVALID_ARG
 * @retval  CANSPI_BUSY
 * @retval  CANSPI_OK
 */
CANSPI_StatusTypeDef CAN_StopLightOn(void)
{
    uint8_t brake_status = 1;
    CANSPI_StatusTypeDef status = CAN_SendMessage(STEERING_WHEEL_ADDR, GLOBAL_LIGHT_ADDR, TIM_STOP_LIGHT_CMD, TIM_LIGHTS_LENGTH, &brake_status);

    if (status != CANSPI_OK)
    {
    	TIM_APP_ERR_MSG("TX Fail: CAN_StopLightOn \n");
    }
    else
    {
		TIM_APP_DBG_MSG("TX: CAN_StopLightOn \n");
    }
    return status;
}

/**
 * @brief   Send the Signal Headlights On
 * @param[in] void
 * @return  CANSPI_StatusTypeDef
 * @retval  CANSPI_INVALID_ARG
 * @retval  CANSPI_BUSY
 * @retval  CANSPI_OK
 */
CANSPI_StatusTypeDef CAN_HeadlightOff(void)
{
	uint8_t headlight_status = 0;
	CANSPI_StatusTypeDef status = CAN_SendMessage(STEERING_WHEEL_ADDR, GLOBAL_LIGHT_ADDR, TIM_LIGHTS_CMD, TIM_LIGHTS_LENGTH, &headlight_status);
    if (status != CANSPI_OK)
    {
    	TIM_APP_ERR_MSG("TX Fail: CAN_HeadlightOff \n");
    }
    else
    {
		TIM_APP_DBG_MSG("TX: CAN_HeadlightOff \n");
    }
    return status;
}

/**
 * @brief   Send the Signal Blinker On
 * @param[in] void
 * @return  CANSPI_StatusTypeDef
 * @retval  CANSPI_INVALID_ARG
 * @retval  CANSPI_BUSY
 * @retval  CANSPI_OK
 */
CANSPI_StatusTypeDef CAN_Blinker_Off()
{
    uint8_t blinker = BLINKER_NONE;
    CANSPI_StatusTypeDef status = CAN_SendMessage(STEERING_WHEEL_ADDR, GLOBAL_LIGHT_ADDR, TIM_BLINKERS_CMD, TIM_BLINKERS_LENGTH, &blinker);
    if (status != CANSPI_OK)
    {
    	TIM_APP_ERR_MSG("TX Fail: CAN_Blinker_Off \n");
    }
    else
    {
		TIM_APP_DBG_MSG("TX: CAN_Blinker_Off \n");
    }
    return status;
}

/**
 * @brief   Send the Signal to set the Stop Light
 * @param[in] void
 * @return  CANSPI_StatusTypeDef
 * @retval  CANSPI_INVALID_ARG
 * @retval  CANSPI_BUSY
 * @retval  CANSPI_OK
 */
CANSPI_StatusTypeDef CAN_StopLightOff(void)
{
    uint8_t brake_status = 0;
    CANSPI_StatusTypeDef status = CAN_SendMessage(STEERING_WHEEL_ADDR, GLOBAL_LIGHT_ADDR, TIM_STOP_LIGHT_CMD, TIM_LIGHTS_LENGTH, &brake_status);
    if (status != CANSPI_OK)
    {
    	TIM_APP_ERR_MSG("CAN_StopLightOff not send \n");
    }
    else
    {
		TIM_APP_DBG_MSG("CAN_StopLightOff \n");
    }
    return status;
}

/* ================================================================
 *                      	TEST
 * ================================================================ */
/**
 * @brief   Test all the CAN Send Function
 * @param[in] void
 * @return  void
 */
void CAN_Send_Test_All(void)
{
	int i;
	for (i = 0; i < 20; i++)
	{
	    CAN_StopLightOn();
	    HAL_Delay(100);  // 0.1 s
	}
	CAN_StopLightOff();

	for (i = 0; i < 20; i++)
	{
	    CAN_Blinker_On(BLINKER_LEFT);
	    HAL_Delay(100);
	}
	CAN_Blinker_Off(BLINKER_LEFT);

	for (i = 0; i < 20; i++)
	{
	    CAN_Blinker_On(BLINKER_RIGHT);
	    HAL_Delay(100);
	}
	CAN_Blinker_Off(BLINKER_RIGHT);

	for (i = 0; i < 20; i++)
	{
	    CAN_HeadlightOn();
	    HAL_Delay(100);
	}
	CAN_HeadlightOff();

	for (i = 0; i < 20; i++)
	{
	    CAN_ElecMotorForward(0);
	    HAL_Delay(100);
	}

	for (i = 0; i < 20; i++)
	{
	    CAN_ElecMotorDisable();
	    HAL_Delay(100);
	}

	for (i = 0; i < 20; i++)
	{
	    CAN_SendPingPong();
	    HAL_Delay(100);
	}
}
