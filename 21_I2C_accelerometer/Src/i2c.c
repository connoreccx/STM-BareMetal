#include "stm32f4xx.h"
#include "i2c.h"

#define I2C1_EN					(1U<<21)
#define GPIOB_EN				(1U<<1)
#define I2C1_OPEN_DRAIN			((1U<<8) | (1U<<9))
#define FREQ_16MHZ				(1U<<4)
#define CR1_PE					(1U<<0)
#define CR1_START				(1U<<8)
#define CR1_ACK					(1U<<10)
#define CR1_STOP				(1U<<9)
#define SR1_SB					(1U<<0)
#define SR1_ADDR				(1U<<1)
#define SR1_TXE					(1U<<7)
#define SR1_RXNE				(1U<<6)
#define SR1_BTF					(1U<<2)
#define SR2_BUSY				(1U<<1)

// Can get these values through online calculator means
#define I2C_100kHZ				80			// 0B 0101 000 -> 80
#define SD_MODE_MAX_RISE_TIME	17

// Uses APB1 (45 MHz) for clock
// Alternate function AF4 (low) for both
// I2C1 CLK -> PB8
// I2C1 SDA -> PB9

void I2C1_init(void){
	// Enable Clock Access to I2C1
	RCC->APB1ENR |= I2C1EN;

	// Enable Clock Access to Port B
	RCC->AHB1ENR |= GPIOB_EN;

	// Configure PA8 and PA9 in alternate mode
	GPIOB->MODER |= (1U<<17);
	GPIOB->MODER &= ~(1U<<16);

	GPIOB->MODER |= (1U<<19);
	GPIOB->MODER &= ~(1U<<18);

	// Configure PB8 and PB9 output type to open drain
	GPIOB->OTYPER |= I2C1_OPEN_DRAIN;

	// Enable Pullup for PB8 and PB9
	GPIOB->PUPDR |= (1U<<16);
	GPIOB->PUPDR &= ~(1U<<17);

	GPIOB->PUPDR |= (1U<<18);
	GPIOB->PUPDR &= ~(1U<<19);

	// Enter Reset Mode
	I2C1->CR1 |= (1U<<15);

	// Come out of reset
	I2C1->CR1 &= ~(1U<<15);

	// Set frequency for 12C to match clock freq
	I2C1->CR2 |= FREQ_16MHZ;

	// Set I2C Clock to standard mode
	I2C1->CCR = I2C_100kHZ;

	// Set Max Rise time
	I2C1->TRISE = SD_MODE_MAX_RISE_TIME;

	// Enable I2C1 Module
	I2C1->CR1 |= CR1_PE;
}

void I2C1_byteRead(char saddr, char maddr, char* data){
	volatile int tmp;

	// Wait until it is not busy, since we are not using interrupts
	while(I2C1->SR2 & SR2_BUSY){}

	// Enable Start Condition
	I2C1->CR1 |= CR1_START;
	// Wait until start flag is set
	while(!(I2C1->SR1 & SR1_SB)){}
	// Transmit slave (target) address + write (default is 0)
	I2C1->DR = saddr << 1;
	// Wait until addr flag is set
	while(!(I2C1->SR1 & SR1_ADDR)){}
	// Clear addr flag
	tmp = I2C1->SR2;

	// Wait until transmitter is empty
	while(!(I2C1->SR1 & SR1_TXE)){}
	// Loads in Memory Addr and sends
	I2C1->DR = maddr;
	// Wait until transmitter is empty
	while(!(I2C1->SR1 & SR1_TXE)){}
	// Generate Restart Condition
	I2C1->CR1 |= CR1_START;
	// Wait for restart to be set
	while(!(I2C1->SR1 & SR1_SB)){}
	// Transmit the Slave ADDR + read (specify when setting the 1)
	I2C1->DR = saddr << 1 | 1;
	// Wait until addr flag is set
	while(!(I2C1->SR1 & SR1_ADDR)){}
	// Diable ACK
	I2C1->CR1 &= ~CR1_ACK;
	// Clear addr flag by reading SR2
	tmp = I2C1->SR2;

	// Generate STOP
	I2C1->CR1 |= CR1_STOP;
	// Wait until RXNE flag is set
	while(I2C1->SR1 & SR1_RXNE){}
	// Read data from DR
	*data++ = I2C1->DR;
}

void I2C1_burstRead(char saddr, char maddr, int n, char* data){
	volatile int tmp;

	// Wait until it is not busy, since we are not using interrupts
	while(I2C1->SR2 & SR2_BUSY){}
	// Generate Start Condition
	I2C1->CR1 |= CR1_START;
	// Wait until start flag is set
	while(!(I2C1->SR1 & SR1_SB)){}
	// Transmit slave (target) address + write (default is 0)
	I2C1->DR = saddr << 1;
	// Wait until addr flag is set
	while(!(I2C1->SR1 & SR1_ADDR)){}
	// Clear addr flag
	tmp = I2C1->SR2;

	// Wait until transmitter is empty
	while(!(I2C1->SR1 & SR1_TXE)){}
	// Loads in Memory Addr and sends
	I2C1->DR = maddr;
	// Wait until transmitter is empty
	while(!(I2C1->SR1 & SR1_TXE)){}
	// Generate Restart Condition
	I2C1->CR1 |= CR1_START;
	// Wait for restart to be set
	while(!(I2C1->SR1 & SR1_SB)){}
	// Transmit the Slave ADDR + read (specify when setting the 1)
	I2C1->DR = saddr << 1 | 1;
	// Wait until addr flag is set
	while(!(I2C1->SR1 & SR1_ADDR)){}
	// Clear addr flag by reading SR2
	tmp = I2C1->SR2;

	// Enable ACK
	I2C1->CR1 |= CR1_ACK;

	//
	while(n > 0U)
	{
		if(n == 1) // Check if n is 1
		{
			// Disable ACK
			I2C1->CR1 &= ~CR1_ACK;
			// Generate stop
			I2C1->CR1 |= CR1_STOP;
			// Wait for RXNE bit to be set
			while(!(I2C1->SR1 & SR1_RXNE)){}
			// Read data from DR
			*data++ = I2C1->DR;

			break;
		}
		else
		{
			// Wait for the data to arrive
			while(!(I2C1->SR1 & SR1_RXNE)){}
			// Read Data
			(*data++) = I2C1->DR;

			n--;
		}
	}
}

void I2C1_burstWrite(char saddr, char maddr, int n, char* data){
	volatile int tmp;

	// Wait until it is not busy, since we are not using interrupts
	while(I2C1->SR2 & SR2_BUSY){}
	// Generate Start Condition
	I2C1->CR1 |= CR1_START;
	// Wait until start flag is set
	while(!(I2C1->SR1 & SR1_SB)){}
	// Transmit slave (target) address + write (default is 0)
	I2C1->DR = saddr << 1;
	// Wait until addr flag is set
	while(!(I2C1->SR1 & SR1_ADDR)){}
	// Clear addr flag
	tmp = I2C1->SR2;

	// Wait until Data Register is Empty
	while(!(I2C1->SR1 & SR1_TXE)){}
	// Loads in Memory Addr and sends
	I2C1->DR = maddr;

	for(int i=0; i<n; i++)
	{
		// Wait until data register is empty
		while(!(I2C1->SR1 & SR1_TXE)){}
		// Transmit data
		I2C1->DR = *data++;
	}

	// Wait until data is done transfering
	while(!(I2C1->SR1 & SR1_BTF)){}
	// Generate Stop condition
	I2C1->CR1 |= CR1_STOP;
}



