#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "tim.h"
#include "exti.h"

#define GPIOAEN				(1U<<0)

#define PIN5 				(1U<<5)
#define LED					PIN5

static void exti_callback(void);

int main(void){

	// 1. Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	// 2. Set PA5 as output pin
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &= ~(1U<<11);

	pc13_exti_init();
	uart2_tx_init();

	while(1){

	}
}

static void exti_callback(void){
	printf("BTN Pressed... \n\r");
	GPIOA->ODR ^= LED; // Toggles LED
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
