#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "tim.h"
#include "led.h"

static void tim2_callback(void);

int main(void){

	led_init();
	uart2_tx_init();
	tim2_1hz_interrupt_init();

	while(1){

	}
}

static void tim2_callback(void){
	printf("A second has passed \n\r");
	led_toggle();
}

void TIM2_IRQHandler(void){
	// Need to clear Update Interrupt flag
	TIM2->SR &= ~SR_UIF;
	tim2_callback();
}
