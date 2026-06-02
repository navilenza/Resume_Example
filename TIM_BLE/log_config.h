/*
 * log_config.h
 *
 *  Created on: May 3, 2026
 *      Author: Caroline Nguyen
 *      Contact: c_nguyen@insa-toulouse.fr
 */

#ifndef LOG_CONFIG_H_
#define LOG_CONFIG_H_

#include <stdio.h>
#include <inttypes.h>

/* To control log level on TIM_APP Folder*/
#define LOG_TIM_APP				1
#define LOG_TIM_BLE				1

/* CAN Level: 0 = Off, 1 = Minimal, 2 = All */
#define LOG_CAN_LEVEL			0

// Macro for the structure
#define LOG_DISPATCH(level, fmt, ...) \
		printf("[%s] " fmt "\r\n", level, ##__VA_ARGS__)

// TIM APP LOGS
#if LOG_TIM_APP
#define TIM_APP_DBG_MSG(fmt, ...)  LOG_DISPATCH("APP", fmt, ##__VA_ARGS__)
#define TIM_APP_ERR_MSG(fmt, ...)  LOG_DISPATCH("ERR", fmt, ##__VA_ARGS__)
#else
#define TIM_APP_DBG_MSG(fmt, ...)
#define TIM_APP_ERR_MSG(fmt, ...)
#endif

// TIM BLE LOGS
#if LOG_TIM_BLE
#define TIM_BLE_MSG(fmt, ...)      LOG_DISPATCH("BLE", fmt, ##__VA_ARGS__)
#else
#define TIM_BLE_MSG(fmt, ...)
#endif

// CAN and MCP LOGS
#if (LOG_CAN_LEVEL == 2)
#define CAN_DBG_MSG(fmt, ...)      LOG_DISPATCH("CAN", fmt, ##__VA_ARGS__)
#define MCP2515_DBG_MSG(fmt, ...)  LOG_DISPATCH("MCP", fmt, ##__VA_ARGS__)
#define CAN_ERR_MSG(fmt, ...)      LOG_DISPATCH("CAN_ERR", fmt, ##__VA_ARGS__)
#define MCP2515_ERR_MSG(fmt, ...)  LOG_DISPATCH("MCP_ERR", fmt, ##__VA_ARGS__)

#elif (LOG_CAN_LEVEL == 1)
#define CAN_DBG_MSG(fmt, ...)      LOG_DISPATCH("CAN", fmt, ##__VA_ARGS__)
#define MCP2515_DBG_MSG(fmt, ...)
#define CAN_ERR_MSG(fmt, ...)      LOG_DISPATCH("CAN_ERR", fmt, ##__VA_ARGS__)
#define MCP2515_ERR_MSG(fmt, ...)

#else
#define CAN_DBG_MSG(fmt, ...)
#define MCP2515_DBG_MSG(fmt, ...)
#define CAN_ERR_MSG(fmt, ...)
#define MCP2515_ERR_MSG(fmt, ...)
#endif

#endif /* LOG_CONFIG_H_ */
