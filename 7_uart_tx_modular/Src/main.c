#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>
#include "uart.h"

int main(void){

	uart2_tx_init();

	while(1){
		printf("My baby is the bestest of babies of all time!!!!!!!!!!!!!!! \n\r");
	}
}
