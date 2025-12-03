#include "exti.h"

#define GPIOCEN			(1U<<2) // Since A->0
#define SYSCFGEN		(1U<<14)

// REMEMBER: PUSH BUTTON IS ON PC13

void pc13_exti_init(void){

	// Disable global interrupts (Good practice)
	__disable_irq();

	// Enable Clock access for GPIOC
	RCC->AHB1ENR |= GPIOCEN;

	// Set PC13 as input
	GPIOC->MODER &= ~(1U<<26);
	GPIOC->MODER &= ~(1U<<27);

	// Enable Clock access to SYSCFG (Connected to AHB2 Bus)
	RCC->APB2ENR |= SYSCFGEN;

	// Select Port C for EXTI13
	SYSCFG->EXTICR[3] |= (1U<<5); // We want EXTICR4 but -1 since we count from 0

	// Unmask EXTI13
	EXTI->IMR |= (1U<<13);

	// Select falling edge trigger
	EXTI->FTSR |= (1U<<13);

	// Enable EXTI line in NVIC
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	// Enable Global Interrupts
	__enable_irq();


}
