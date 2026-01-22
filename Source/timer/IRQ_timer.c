/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/

#include "LPC17xx.h"
#include "timer.h"
#include "../tetris/tetris.h"
#include "../GLCD/GLCD.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
uint16_t SinTable[45] =                                       
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

extern volatile Tetromino currentTetromino;

extern volatile GameState state;

extern volatile uint8_t slow_mode;

void TIMER0_IRQHandler (void)
{
		if (state == PLAYING)
			updateField(currentTetromino.x, currentTetromino.y + 1, currentTetromino.rotation);

    LPC_TIM0->IR = 1;			
    return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
		slow_mode = 0; 
    
    disable_timer(1);
    
    LPC_TIM1->IR = 1;
}


void TIMER2_IRQHandler (void)
{
	static int ticks=0;	
	LPC_DAC->DACR = (SinTable[ticks]<<6);
	ticks++;
	if(ticks==45) ticks=0;

  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
