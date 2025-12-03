#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "tim.h"

int timestamp = 0;

// Setup: Connect a jumper wire from PA5 to PA6

int main(void){

	tim2_pa5_output_compare(); // Signal Source
	tim3_pa6_input_capture(); // Reader

	while(1){
		// Wait until edge is captured
		while(!(TIM3->SR & SR_CCR1IF)){}

		// Read value
		timestamp = TIM3->CCR1;
	}
}
