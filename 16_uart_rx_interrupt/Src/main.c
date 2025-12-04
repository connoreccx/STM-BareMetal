#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "led.h"

char key;

static void uart_callback(void);

int main(void){

	led_init();
	uart2_rx_interrupt_init();

	while(1){
		key = uart2_read();
	}
}

static void uart_callback(void){
	key = USART2->DR;

	if(key == '1'){
		led_on();
	}
	else{
		led_off();
	}
}

// found this in "startup_stm32f446retx.s"
void USART2_IRQHandler(void){
	// Check if RXNE is set
	if(USART2->SR & SR_RXNE){
		// Do something...
		uart_callback();
	}
}
