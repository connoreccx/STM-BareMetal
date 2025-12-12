#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "led.h"

static void dma_callback(void);

int main(void){
	int len = 31;
	char message[31] = "Hello from STM32 DMA Transfer \n\r";

	led_init();
	uart2_tx_init();
	dma1_stream6_init((uint32_t) message, (uint32_t) &(USART2->DR), (uint32_t) len);

	while(1){

	}
}

static void dma_callback(void){
	led_on();
}

void DMA1_Stream6_IRQHandler(void){
	// Check for transfer complete
	if(DMA1->HISR & HISR_TCIF6){
		// Clear Interrupt flag
		DMA1->HIFCR = HIFCR_CTCIF6;

		// Do something...
		dma_callback();

	}
}
