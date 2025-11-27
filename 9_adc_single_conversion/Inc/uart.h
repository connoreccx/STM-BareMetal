/*
 * uart.h
 *
 *  Created on: Nov 19, 2025
 *      Author: conno
 */

#ifndef UART_H_
#define UART_H_
#include "stm32f4xx.h"
#include <stdint.h>

void uart2_tx_init(void);
void uart2_rxtx_init(void);
char uart2_read(void);

#endif /* UART_H_ */
