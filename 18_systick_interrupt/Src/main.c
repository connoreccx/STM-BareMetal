#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "led.h"

static void systick_callback(void);

int main(void){

	led_init();
	uart2_tx_init();
	systick_1hz_interrupt();

	while(1){

	}
}

static void systick_callback(void){
	printf("A second has passed \n\r");
	led_toggle();
}

void SysTick_Handler(void){ // Defined in the vector table
	// Do something
	systick_callback();
}
