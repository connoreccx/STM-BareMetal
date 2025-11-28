#include "stm32f4xx.h"
#include "adc.h"

#define GPIOAEN			(1U<<0)
#define ADC1EN			(1u<<8)
#define ADC_CH1			(1U<<0) // 0B = 0001
#define ADC_SEQ_LEN_1	(0x00)
#define CR2_ADON		(1U<<0)
#define CR2_SWSTART		(1U<<30)
#define SR_EOC			(1U<<1)
#define CR2_CONT		(1U<<1)

/* ADC configured with 3 channels
 * ch2, ch3, ch5
 * First = ch5
 * Second = ch2
 * Third = ch3
 *
 * In the sequence register, we need to put the binary number of the channel in sequence of sampling order
 * */

void pa1_adc_init(void){

	// ---------Configure the ADC GPIO pin---------
	// Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	// Set the mode of the PA1 to analog mode (in mode register)
	GPIOA->MODER |= (1U<<2);
	GPIOA->MODER |= (1U<<3);

	// ---------Configure the ADC module---------
	// Enable clock access to ADC
	RCC->APB2ENR |= ADC1EN;

	// Conversion sequence start
	ADC1->SQR3 = ADC_CH1; // No OR since we are setting the value cleanly

	// Conversion sequence length, sets everything to ZERO
	ADC1->SQR1 = ADC_SEQ_LEN_1;

	// Enable ADC Module
	ADC1->CR2 |= CR2_ADON; //Only want to update this bit
}

void start_conversion(void){
	// Enable Continuous Conversion
	ADC1->CR2 |= CR2_CONT;

	// Start ADC Conversion
	ADC1->CR2 |= CR2_SWSTART;
}

uint32_t adc_read(void){
	// Wait for the conversion to be complete
	while(!(ADC1->SR & SR_EOC)){}

	// Read the converted result
	return ADC1->DR;
}
