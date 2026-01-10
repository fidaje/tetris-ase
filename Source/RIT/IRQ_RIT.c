/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../tetris/tetris.h"
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int down_1=0;
volatile int down_2=0;

extern GameState state;
extern Tetromino currentTetromino;
extern volatile uint8_t firstSpawn;
extern volatile uint16_t high_score;

void RIT_IRQHandler (void)
{					
	static int J_up=0;
	static int J_down=0;
	static int J_right=0;	
	static int J_left =0;
	
	if (state == PLAYING){	
		if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
			/* Joytick UP pressed */
			J_up++;
			switch(J_up){
				case 1:
					rotateTetramino();
					break;
			
				default:
					break;
			}
		}
		else{
				J_up=0;
		}
		
		if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
			/* Joytick DOWN pressed */
			J_down++;
			switch(J_down){
				case 1:
					//soft drop
					disable_timer(0);
					reset_timer(0);
					init_timer(0, STANDARD_GAME_SPEED >> 1);
					enable_timer(0);
					// dimezziamo valore del MR per avere una velocità di caduta 2 block/sec
					//LPC_TIM0 -> MR0 = STANDARD_GAME_SPEED >> 1;
					break;
				
				default:
					break;
			}
		}
		else{
			if (J_down != 0){
				J_down=0;
				disable_timer(0);
				//reset_timer(0);
				init_timer(0, STANDARD_GAME_SPEED);
				enable_timer(0);
				//LPC_TIM0 -> MR0 = STANDARD_GAME_SPEED;
			}
		}
		
		if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
			/* Joytick LEFT pressed */
			J_left++;
			switch(J_left){
				case 1:
					disable_timer(0);
					moveLeft();
					enable_timer(0);
					break;
				default:
					break;
			}
		}
		else{
				J_left=0;
		}
		
		if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
			/* Joytick RIGHT pressed */
			J_right++;
			switch(J_right){
				case 1:
					disable_timer(0);
					moveRight();
					enable_timer(0);
					break;
				default:
					break;
			}
		}
		else{
				J_right=0;
		}
	}
	/* button management */
	
	// KEY1
	if(down_1 !=0){
	down_1++;
	if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
		switch(down_1){
			case 2:				/* pay attention here: please see slides to understand value 2 */
					if (state == PAUSED){
						state = PLAYING;
						GUI_Text(175, 150, (uint8_t *) "PAUSE", Black, Black);
						enable_timer(0);
						if (firstSpawn){
							spawnTetromino();
							drawTetromino();
							firstSpawn = 0;
						}
		
					}
					else if (state == PLAYING){
						state = PAUSED;
						GUI_Text(175, 150, (uint8_t *) "PAUSE", White, Red);
						disable_timer(0);
					}
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_1=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
	// KEY2
	if (state == PLAYING){
		if(down_2 !=0){
		down_2++;
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
			switch(down_2){
				case 2:
					hardDrop();
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_2=0;		
			NVIC_EnableIRQ(EINT2_IRQn);							 /* disable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
			}
		}
	}

	reset_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
