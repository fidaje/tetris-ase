/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "LPC17xx.h"
#include "adc.h"
#include "../timer/timer.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short AD_current;   
unsigned short AD_last = 0xFF;     /* Last converted value               */
volatile float speed_factor = 1.0;


void ADC_IRQHandler(void) {
  	    
	// lettura del valore
	AD_current = ((LPC_ADC->ADGDR >> 4) & 0xFFF);

	// calcolo fattore di velocità
	speed_factor = 1.0f + ((float)AD_current / 4095.0f) * 4.0f;

}
