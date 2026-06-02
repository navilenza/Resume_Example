/*
 * controller.c
 *
 *  Created on: Mar 8, 2026
 *      Author: Caroline Nguyen
 *  Contact: c_nguyen@insa-toulouse.fr
 *
 */

/* ================================================================
 * 								Includes
 * ================================================================ */
#include "controller.h"

extern ADC_HandleTypeDef hadc1;
/* ================================================================
 * 					Private Prototype definition
 * ================================================================ */
uint8_t ADCToModesCarto(uint32_t ADC_Value);
uint8_t Get_Carto_Selector_Mode(void);

/* ================================================================
 * 						Function definition
 * ================================================================ */

/**
 * @brief   Callback for CAN MessagesCANSPI_Receive
 * @param[in] void
 * @return  CANSPI_StatusTypeDef
 * @retval  CANSPI_ERROR
 * @retval  CANSPI_OK
 */
CANSPI_StatusTypeDef CAN_Receive_Callback(void)
{
	CAN_RxHeaderTypeDef pCAN_RxHeader;
	uint8_t             CAN_RxData[8];

	CANSPI_StatusTypeDef status = CANSPI_Receive(&pCAN_RxHeader, CAN_RxData);
	/* Check CAN Status */
	if (status != CANSPI_OK)
	{
		TIM_DASHBOARD_SERVER_APP_SetStatusBit(CAN_BUS_OFF_BIT);
		return status;
	}
	else
	{
		TIM_DASHBOARD_SERVER_APP_ClearStatusBit(CAN_BUS_OFF_BIT);
	}


	/*
	 *rxData
	 */
	/// For testing purpose to delete
	const uint8_t ping_pending = 1;
	/* Check that the card is connected to the CAN */
	if (ping_pending == 1)
	{
		CAN_CheckPingPong(pCAN_RxHeader);
	}

	// Check for Speed Data CAN ID
	if (pCAN_RxHeader.ExtId == CAN_MAKE_ID(POSITION_MONITORING_ADDR, DASHBOARD_ADDR, TIM_SPEED_DATA))
	{
		// Extract from CAN Data the speed
		global_speed = ((uint16_t)CAN_RxData[2] << 8) | CAN_RxData[3];
		if (global_speed > 600){ global_speed = 60; }
		TIM_DASHBOARD_SERVER_APP_UpdateSpeed(global_speed);
	}

	// Trame de Reset
	if (CAN_GET_CMD(pCAN_RxHeader.ExtId) == CAN_CMD_ID(TIM_RESET))
	{
		TIM_APP_DBG_MSG("Reset Command received, restarting the system \n");
		// reset
	}

	return CANSPI_OK;
}



/**
 * @brief   Callback for GPIO's
 * @param[in] GPIO Port
 * @param[in] GPIO Pinn
 * @return  none
 */
 
 //TODO ANTI REBOND
void HAL_GPIO_EXTI_Callback(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	BSP_LED_Toggle(LED_GREEN);

	// Interruption pour le module CAN
	if (GPIO_Pin == INT_SPI_Pin) {
		TIM_APP_DBG_MSG("MCP2515 Interrupt received \n");

		/* Ping Pong Test */
		CAN_SendPingPong();
		CAN_Receive_Callback();
	}

	if (GPIO_Pin == Accelerator_Pin) {
		// Send cmd to Motors
		CAN_ElecMotorEnable();
	}
	else
	{
		CAN_ElecMotorDisable();
	}

	if (GPIO_Pin == Headlight_Pin) {
		// Pin en Pull Up =>  Reset = bouton appuyé
		if (HAL_GPIO_ReadPin(Headlight_GPIO_Port, Headlight_Pin) == GPIO_PIN_RESET)
		{
			CAN_HeadlightOn();
			TIM_DASHBOARD_SERVER_APP_SetStatusBit(HEADLIGHT_BIT);
		} else {
			CAN_HeadlightOff();
			TIM_DASHBOARD_SERVER_APP_ClearStatusBit(HEADLIGHT_BIT);
		}
	}

	if (GPIO_Pin == RightBlinker_Pin)
	{
		if (HAL_GPIO_ReadPin(RightBlinker_GPIO_Port, RightBlinker_Pin) == GPIO_PIN_RESET)
		{
			CAN_Blinker_On(BLINKER_RIGHT);
			TIM_DASHBOARD_SERVER_APP_SetStatusBit(RIGHT_BLINKER_BIT);
		}
		else
		{
			CAN_Blinker_Off();
			TIM_DASHBOARD_SERVER_APP_ClearStatusBit(RIGHT_BLINKER_BIT);
		}
	}

		if(GPIO_Pin == LeftBlinker_Pin))
		{
			if (HAL_GPIO_ReadPin(LeftBlinker_GPIO_Port, LeftBlinker_Pin) == GPIO_PIN_RESET) {
				CAN_Blinker_On(BLINKER_LEFT);
				TIM_DASHBOARD_SERVER_APP_SetStatusBit(LEFT_BLINKER_BIT);
			}
			else
			{
				CAN_Blinker_Off();
				TIM_DASHBOARD_SERVER_APP_ClearStatusBit(LEFT_BLINKER_BIT);
			}
		}

		if ((GPIO_Pin == Warning_Pin))
		{
			if (HAL_GPIO_ReadPin(Warning_GPIO_Port, Warning_Pin) == GPIO_PIN_RESET) {
				CAN_Blinker_On(WARNING);
				TIM_DASHBOARD_SERVER_APP_SetStatusBit(WARNING_BIT);
			} else {
				CAN_Blinker_Off();
				TIM_DASHBOARD_SERVER_APP_ClearStatusBit(WARNING_BIT);
			}
		}

	if (GPIO_Pin == Break_Pedal_Pin) {
		// Pin en Pull Up =>  Reset = bouton appuyé

		if (HAL_GPIO_ReadPin(Break_Pedal_GPIO_Port, Break_Pedal_Pin) == GPIO_PIN_RESET)
		{
			TIM_DASHBOARD_SERVER_APP_SetStatusBit(STOP_LIGHT_BIT);
		}
		else
		{
			TIM_DASHBOARD_SERVER_APP_ClearStatusBit(STOP_LIGHT_BIT);
		}
	}
}

/**
 * @brief
 * @param[in]
 * @return
 */

uint8_t Get_Carto_Selector_Mode(void)
{
	uint32_t adc_value = 0;
	static uint8_t last_mode = 0xFF;

	if (HAL_ADC_Start(&hadc1) == HAL_OK)
	{
		// Increase timeout to ensure the conversion actually finishes
		if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
		{
			adc_value = HAL_ADC_GetValue(&hadc1);
			uint8_t mode_carto = ADCToModesCarto(adc_value);

			if (mode_carto != last_mode)
			{
				last_mode = mode_carto;
				APP_DBG_MSG("[I] Mode Carto : %u \n", mode_carto);
				TIM_DASHBOARD_SERVER_APP_UpdateSelector(mode_carto);
			}
		}
		HAL_ADC_Stop(&hadc1);
	}

	return ADCToModesCarto(adc_value);
}

/**
 * @brief   Convert ADC value to a mode
 * @param[in] raw ADC value
 * @return  ADC Value
 */
uint8_t ADCToModesCarto(uint32_t ADC_Value)
{
	if (ADC_Value <= ADC_REF_0) {
		return 1;
	} else if (ADC_Value <= ADC_REF_1) {
		return 2;
	} else if (ADC_Value <= ADC_REF_2) {
		return 3;
	} else if (ADC_Value <= ADC_REF_3) {
		return 4;
	} else if (ADC_Value <= ADC_REF_4) {
		return 5;
	} else if (ADC_Value <= ADC_REF_5) {
		return 6;
	} else if (ADC_Value <= ADC_REF_6) {
		return 7;
	} else if (ADC_Value <= ADC_REF_7) {
		return 8;
	} else if (ADC_Value <= ADC_REF_8) {
		return 9;
	} else {
		return 0;
	}
}

/**
 * @brief   Callback for the Timer that send the command to the motor
 * @param[in] Timer Handler
 * @return  none
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM16)
	{
		uint16_t Current_Cmd = current_map[Get_Carto_Selector_Mode()][global_speed/50];
		CAN_ElecMotorForward(Current_Cmd);
		CAN_Receive_Callback();

		TIM_DASHBOARD_SERVER_APP_NOTIFY_EXAMPLE_HMI();
	}
}

