#include "uart.h"

#define GPIOAEN			(1U<<0)
#define UART2EN			(1U<<17)

#define CR1_TE			(1U<<3)
#define CR1_RE			(1U<<2)
#define CR1_UE			(1U<<13)
#define CR1_RXNEIE		(1U<<5)

#define SR_TXE			(1U<<7)

#define SYS_FREQ		16000000
#define APB1_CLK		SYS_FREQ

#define UART_BAUDRATE	115200

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);

void uart2_write(int ch);

int __io_putchar(int ch){
	uart2_write(ch);
	return ch;
}

void uart2_rx_interrupt_init(void){ // Clean this up later, it still has TX enabled
	/*********************************configure UART GPIO pin************************************/
	// Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	// Set PA2 mode to alt function mode, check the mode register in reference manual to find truth table
	GPIOA->MODER &=~ (1U<<4);
	GPIOA->MODER |= (1U<<5);

	// Set PA2 alternate function type to UART TX (AF7), AFRL = AFR[0], AFRH = AFR[1]
	GPIOA->AFR[0] |= (1U<<8);
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &=~ (1U<<11);

	// Set PA3 mode to alternate function mode
	GPIOA->MODER &=~ (1U<<6);
	GPIOA->MODER |= (1U<<7);

	// Set PA3 alternate function type to UART_RX (AF7), AFRL = AFR[0], AFRH = AFR[1]
	GPIOA->AFR[0] |= (1U<<12);
	GPIOA->AFR[0] |= (1U<<13);
	GPIOA->AFR[0] |= (1U<<14);
	GPIOA->AFR[0] &=~ (1U<<15);

	/*********************************configure UART module************************************/
	// Enable clock access to uart2
	RCC->APB1ENR |= UART2EN;

	// Config UART baudrate
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

	// Config transfer direction, look up USART_CR1 in reference manual
	USART2->CR1 = (CR1_TE | CR1_RE); // Not doing OR to clean the UART and sets everything to 0 other than the one you are interested in

	// Enable RXNE interrupt
	USART2->CR1 |= CR1_RXNEIE;

	// Enable UART2 Interrupt in NVIC
	NVIC_EnableIRQ(USART2_IRQn);

	// Enable UART module
	USART2->CR1 |= CR1_UE; // Already configured TE, so we will use OR

}


void uart2_rxtx_init(void){
	/*********************************configure UART GPIO pin************************************/
	// Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	// Set PA2 mode to alt function mode, check the mode register in reference manual to find truth table
	GPIOA->MODER &=~ (1U<<4);
	GPIOA->MODER |= (1U<<5);

	// Set PA2 alternate function type to UART TX (AF7), AFRL = AFR[0], AFRH = AFR[1]
	GPIOA->AFR[0] |= (1U<<8);
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &=~ (1U<<11);

	// Set PA3 mode to alternate function mode
	GPIOA->MODER &=~ (1U<<6);
	GPIOA->MODER |= (1U<<7);

	// Set PA3 alternate function type to UART_RX (AF7), AFRL = AFR[0], AFRH = AFR[1]
	GPIOA->AFR[0] |= (1U<<12);
	GPIOA->AFR[0] |= (1U<<13);
	GPIOA->AFR[0] |= (1U<<14);
	GPIOA->AFR[0] &=~ (1U<<15);

	/*********************************configure UART module************************************/
	// Enable clock access to uart2
	RCC->APB1ENR |= UART2EN;

	// Config UART baudrate
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

	// Config transfer direction, look up USART_CR1 in reference manual
	USART2->CR1 = (CR1_TE | CR1_RE); // Not doing OR to clean the UART and sets everything to 0 other than the one you are interested in

	// Enable UART module
	USART2->CR1 |= CR1_UE; // Already configured TE, so we will use OR

}

void uart2_tx_init(void){
	/*********************************configure UART GPIO pin************************************/
	// Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	// Set PA2 mode to alt function mode, check the mode register in reference manual to find truth table
	GPIOA->MODER &=~ (1U<<4);
	GPIOA->MODER |= (1U<<5);

	// Set PA2 alternate function type to UART TX (AF7), AFRL = AFR[0], AFRH = AFR[1]
	GPIOA->AFR[0] |= (1U<<8);
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &=~ (1U<<11);

	/*********************************configure UART module************************************/
	// Enable clock access to uart2
	RCC->APB1ENR |= UART2EN;

	// Config UART baudrate
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

	// Config transfer direction, look up USART_CR1 in reference manual
	USART2->CR1 = CR1_TE; // ORing sets both

	// Enable UART module
	USART2->CR1 |= CR1_UE; // Already configured TE, so we will use OR

}

char uart2_read(void){
	// Make sure the receive data register is not empty
	while(!(USART2->SR & SR_RXNE)){} //SR = Status Register

	// Read data
	return USART2->DR;
}

void uart2_write(int ch){
	// Make sure transmit data register is empty
	while(!(USART2->SR & SR_TXE)){} //SR = Status Register

	// Write to transmit data register
	USART2->DR = (ch & 0xFF);
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate){
	USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate){
	return ((PeriphClk + (BaudRate/2U))/BaudRate);
}
