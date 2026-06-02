/*
 * CAN_Controller.h
 *
 *  Created on: Mar 8, 2026
 *      Author: Caroline Nguyen
 *      Contact: c_nguyen@insa-toulouse.fr
 */

#ifndef CAN_CAN_CONTROLLER_H_
#define CAN_CAN_CONTROLLER_H_

/* ================================================================
 *                          Includes
 * ================================================================ */
#include "CANSPI.h"
#include "CAN_ID.h"
#include "log_config.h"
/* ================================================================
 *                          Defines
 * ================================================================ */
#define CAN_PINGPONG_TIMEOUT    something
#define CAN_PING_BYTE           something

/* CAN Extended ID layout: Hidden :))) */
#define CAN_MAKE_ID(something, something, something)  ((uint32_t)(((something) << 16) | ((something) << 8) | (something)))
#define CAN_GET_CMD(something)             (((something) >> something) & 0xFFFF)
#define CAN_CMD_ID(something)             ((uint32_t)((something) & 0xFFFF))

#define BLINKER_NONE  Value
#define BLINKER_LEFT  Value
#define BLINKER_RIGHT Value
#define WARNING 	Value


#define MOTOR_PROPORTIONAL_GAIN Value
#define MOTOR_CMD_FORWARD_LENGTH Value
/* ================================================================
 *                      	Structures
 * ================================================================ */

/* ================================================================
 *                      Public Functions
 * ================================================================ */

/* Specific CAN commands */

/*	Ping Pong 	*/
CANSPI_StatusTypeDef CAN_SendPingPong(void);
CANSPI_StatusTypeDef CAN_CheckPingPong(CAN_RxHeaderTypeDef CAN_RxHeader);


/* Motors Orders */
CANSPI_StatusTypeDef CAN_ElecMotorEnable(void);
CANSPI_StatusTypeDef CAN_ElecMotorDisable(void);
CANSPI_StatusTypeDef CAN_ElecMotorForward(uint16_t Current_Cmd);

/* Lights */
CANSPI_StatusTypeDef CAN_HeadlightOn(void);
CANSPI_StatusTypeDef CAN_Blinker_On(uint8_t blinker_state);
CANSPI_StatusTypeDef CAN_StopLightOn(void);
CANSPI_StatusTypeDef CAN_HeadlightOff(void);
CANSPI_StatusTypeDef CAN_Blinker_Off();
CANSPI_StatusTypeDef CAN_StopLightOff(void);

void CAN_Send_Test_All(void);

#endif /* CAN_CAN_CONTROLLER_H_ */
