/*
 * CAN_Controller.c
 *
 *  Created on: Mar 8, 2026
 *      Author: Caroline Nguyen
 *      Contact: c_nguyen@insa-toulouse.fr
 *
 */

#ifndef CONTROLLER_CONTROLLER_H_
#define CONTROLLER_CONTROLLER_H_
/* ================================================================
 * 								Includes
 * ================================================================ */
#include "main.h"
#include "CAN_ID.h"
#include "CAN_Controller.h"
#include "tim_dashboard_server_app.h"
#include "carto_mode.h"
#include "log_config.h"

/* ================================================================
 * 								BLE Define
 * ================================================================ */
#define BLE_STATUS_BIT        //Value

/* Light Control Bits */
#define HEADLIGHT_BIT         //Value
#define RIGHT_BLINKER_BIT     //Value
#define LEFT_BLINKER_BIT      //Value
#define WARNING_BIT           //Value
#define STOP_LIGHT_BIT        //Value

/* CAN error Bits */
#define CAN_BUS_OFF_BIT       //Value
#define CAN_BUS_TX_ERR_BIT    //Value

/* ================================================================
 * 								ADC Define
 * ================================================================ */

#define ADC_REF_9 3750	// max
#define ADC_REF_8 3300
#define ADC_REF_7 2990
#define ADC_REF_6 2400
#define ADC_REF_5 1900
#define ADC_REF_4 1500
#define ADC_REF_3 1150
#define ADC_REF_2 900
#define ADC_REF_1 500
#define ADC_REF_0 205
/* ================================================================
 * 								Variables
 * ================================================================ */
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;

static uint16_t global_speed;
/* ================================================================
 * 						Functions Prototypes
 * ================================================================ */
CANSPI_StatusTypeDef CAN_Receive_Callback(void);


#endif /* CONTROLLER_CONTROLLER_H_ */
