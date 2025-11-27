#include <stdint.h>

// where is the LED connected?
// Port: A
// Pin:  5


#define PERIPH_BASE 		0x40000000UL
#define AHB1PERIPH_OFFSET	0x0020000UL
#define AHB1PERIPH_BASE		(PERIPH_BASE + AHB1PERIPH_OFFSET)
#define GPIOA_OFFSET		0x0UL // Only the trailing zeros matter

#define GPIOA_BASE 			(AHB1PERIPH_BASE + GPIOA_OFFSET)

#define RCC_OFFSET			0x3800UL
#define RCC_BASE 			(AHB1PERIPH_BASE + RCC_OFFSET)

#define GPIOAEN				(1U<<0) // 0b 0000 0000 0000 0000 0000 0000 0000 0001

#define PIN5 				(1U<<5)
#define LED_PIN				PIN5

#define __IO volatile

typedef struct {
	volatile uint32_t MODER; // The address of the struct is the same as the first item
	volatile uint32_t DUMMY[4]; // This is so that the offset of ODR matches the GPIO Registers offset
	volatile uint32_t ODR;
}GPIO_TypeDef;

typedef struct{
	uint32_t DUMMY[12];
	volatile uint32_t AHB1ENR;
}RCC_TypeDef;

#define RCC					((RCC_TypeDef*) RCC_BASE)
#define GPIOA				((GPIO_TypeDef*) GPIOA_BASE)

int main(void){
	// 1. Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	// 2. Set PA5 as output pin
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~ (1U<<11);

	while(1){
		// 3. Toggle PA5
		GPIOA->ODR ^= LED_PIN;
		for(int i = 0; i < 100000; i++){} // Adds delay

	}
}
