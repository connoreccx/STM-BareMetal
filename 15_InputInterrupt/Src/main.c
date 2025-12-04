#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "tim.h"
#include "exti.h"
#include "led.h"

static void exti_callback(void);

int main(void){

	pc13_exti_init();
	uart2_tx_init();
	led_init();

	while(1){

	}
}

static void exti_callback(void){
	printf("BTN Pressed... \n\r");
	led_toggle();
}

void EXTI15_10_IRQHandler(void){
	// Need to identify PC13
	if((EXTI->PR & LINE13) != 0){ // Execute if the pending register is 1, meaning interupt has occurred
		// Clear PR flag to rearm
		EXTI->PR |= LINE13;

		// Do something...
		exti_callback();
	}
}
