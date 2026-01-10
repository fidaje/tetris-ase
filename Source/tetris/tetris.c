
#include "tetris.h"
#include <stdio.h>
#include "../RIT/RIT.h"
#include "../timer/timer.h"

extern int lfsr_step(int current_state, int tap);
static uint8_t lfsr_seed = 0x55;
#define LFSR_TAP 0X1D

uint16_t game_field[ROWS][COLS];

uint16_t volatile current_score = 0;
uint16_t high_score = 0;
uint16_t lines = 0;

uint8_t volatile firstSpawn;

GameState state;
Tetromino currentTetromino;

// array che contiene i le informazioni dei tetromino
const Tetromino tetroShapes[7] = {
	// I (Asta) - Cyan
    { .type=0, .color=Cyan, .blocks = {
        {{0,1}, {1,1}, {2,1}, {3,1}}, // 0° Orizzontale
        {{2,0}, {2,1}, {2,2}, {2,3}}, // 90° Verticale
        {{0,2}, {1,2}, {2,2}, {3,2}}, // 180° Orizzontale (spostata giù)
        {{1,0}, {1,1}, {1,2}, {1,3}}  // 270° Verticale (spostata sx)
    }},
    // J (L inversa) - Blue
    { .type=1, .color=Blue, .blocks = {
        {{0,0}, {0,1}, {1,1}, {2,1}}, // 0°
        {{1,0}, {2,0}, {1,1}, {1,2}}, // 90°
        {{0,1}, {1,1}, {2,1}, {2,2}}, // 180°
        {{1,0}, {1,1}, {0,2}, {1,2}}  // 270°
    }},
    // L (L normale) - Orange
    { .type=2, .color=Orange, .blocks = {
        {{2,0}, {0,1}, {1,1}, {2,1}}, // 0°
        {{1,0}, {1,1}, {1,2}, {2,2}}, // 90°
        {{0,1}, {1,1}, {2,1}, {0,2}}, // 180°
        {{0,0}, {1,0}, {1,1}, {1,2}}  // 270°
    }},
    // O (Quadrato) - Yellow (Non ruota, tutte uguali)
    { .type=3, .color=Yellow, .blocks = {
        {{1,0}, {2,0}, {1,1}, {2,1}},
        {{1,0}, {2,0}, {1,1}, {2,1}},
        {{1,0}, {2,0}, {1,1}, {2,1}},
        {{1,0}, {2,0}, {1,1}, {2,1}}
    }},
    // S (Serpente S) - Green
    { .type=4, .color=Green, .blocks = {
        {{1,0}, {2,0}, {0,1}, {1,1}}, // 0°
        {{1,0}, {1,1}, {2,1}, {2,2}}, // 90°
        {{1,1}, {2,1}, {0,2}, {1,2}}, // 180°
        {{0,0}, {0,1}, {1,1}, {1,2}}  // 270°
    }},
    // T (T-Shape) - Purple
    { .type=5, .color=Magenta, .blocks = {
        {{1,0}, {0,1}, {1,1}, {2,1}}, // 0° (T verso l'alto)
        {{1,0}, {1,1}, {2,1}, {1,2}}, // 90° (T destra)
        {{0,1}, {1,1}, {2,1}, {1,2}}, // 180° (T basso)
        {{1,0}, {0,1}, {1,1}, {1,2}}  // 270° (T sinistra)
    }},
    // Z (Serpente Z) - Red
    { .type=6, .color=Red, .blocks = {
        {{0,0}, {1,0}, {1,1}, {2,1}}, // 0°
        {{2,0}, {1,1}, {2,1}, {1,2}}, // 90°
        {{0,1}, {1,1}, {1,2}, {2,2}}, // 180°
        {{1,0}, {0,1}, {1,1}, {0,2}}  // 270°
    }}

};


// inizializza il gioco
void Tetris_Init(){
	state = PAUSED;
	reset_timer(0);
	firstSpawn = 1;
	current_score = 0;
	lines = 0;
	
	int i, j;
	for(i=0; i<ROWS; i++) {
        for(j=0; j<COLS; j++) {
            game_field[i][j] = 0;
        }
    }
	
	LCD_Clear(Black);
	
	drawInterface();

}


// disegna l'interfaccia di gioco vuota
void drawInterface(){
	
	// larghezza campo di gioco
	uint16_t leftX = OFFSET_X - 1;
	uint16_t rightX = OFFSET_X + (BLK_SIZE * COLS);
	
	// altezza campo di gioco
	uint16_t topY = OFFSET_Y - 1;
	uint16_t bottomY = OFFSET_Y + (BLK_SIZE * ROWS);
	
	// linea orizzontale superiore
	LCD_DrawLine(leftX, topY, rightX, topY, White);
	
	// linea orizzontale inferiore
	LCD_DrawLine(leftX, bottomY, rightX, bottomY, White);
	
	// linea verticale di sinistra
	LCD_DrawLine(leftX, topY, leftX, bottomY, White);
	
	// linea verticale di destra
	LCD_DrawLine(rightX, topY, rightX, bottomY, White);
	
	
	// punteggi
	uint16_t lateX = rightX + BLK_SIZE;
	
	char score_str[6]; 
	
	GUI_Text(lateX, 15, (uint8_t *) "TOP", White, Black);
	sprintf(score_str, "%u", high_score);
	GUI_Text(lateX, 30, (uint8_t *)score_str,  White, Black);
	
	GUI_Text(lateX, 60, (uint8_t *) "SCORE", White, Black);
	GUI_Text(lateX, 75, (uint8_t *)"0",  White, Black);
	
	GUI_Text(lateX, 105, (uint8_t *) "LINES", White, Black);
	GUI_Text(lateX, 120, (uint8_t *)"0",  White, Black);
	
}


// scrive i punteggi sullo schermo
void printPoints(){
	// punteggi
	uint16_t rightX = OFFSET_X + (BLK_SIZE * COLS);
	uint16_t lateX = rightX + BLK_SIZE;
	
	char score_str[6]; 
	
	sprintf(score_str, "%u", high_score);
	GUI_Text(lateX, 30, (uint8_t *)score_str,  White, Black);
	
	sprintf(score_str, "%u", current_score);
	GUI_Text(lateX, 75, (uint8_t *)score_str,  White, Black);
	
	sprintf(score_str, "%u", lines);
	GUI_Text(lateX, 120, (uint8_t *)score_str,  White, Black);
}


// funzione chiamata da timer0 che fa scendere il tetromino
void updateField(int newX, int newY, int newRot){
	
	// controllo se la mossa/discesa ? possibile
	if (checkCollision(newX, newY, newRot)){
		
		// checkCollision ha ritornato 1, quindi devo fermare il pezzo alla posizione corrente
		placeTetromino(currentTetromino.x, currentTetromino.y, currentTetromino.rotation);
		checkLines();
		printPoints();
		
		if (spawnTetromino())
			drawTetromino();
		else 
			Tetris_Init();
			
		
	} else {
		
		uint16_t color = currentTetromino.color;
		
		currentTetromino.color = Black;
		drawTetromino();
		
		currentTetromino.rotation = newRot;
		currentTetromino.y = newY;
		currentTetromino.color = color;
		drawTetromino();
		
	}

}


// controlla se il movimento del blocco può essere fatto
// ritorna 0 se è permesso, 1 altrimenti
int checkCollision(int newX, int newY, int newRot){
	
	int i;
	int lcdX, lcdY;
	
	
	for (i = 0; i< 4; i++){
		
		int fieldPosX = newX + currentTetromino.blocks[newRot][i][0];
		int fieldPosY = newY + currentTetromino.blocks[newRot][i][1];
		
		// controllo bordi laterali
		if (fieldPosX < 0 || fieldPosX >= COLS)
			return 1;
		
		// controllo fondo
		if (fieldPosY > ROWS - 1)
			return 1;
		
		if (game_field[fieldPosY][fieldPosX] != 0)
			return 1;
				
	}
	
	
	return 0;
}

// posizione il tetromino nel campo da gioco quando arriva nella posizione finale
void placeTetromino(int newX, int newY, int newRot){

	int i;
	
	int blockX, blockY;
	
	for (i = 0; i < 4; i++){
		
		blockX = newX + currentTetromino.blocks[newRot][i][0];
		blockY = newY + currentTetromino.blocks[newRot][i][1];
		
		game_field[blockY][blockX] = currentTetromino.color;
		
	}
		
	char score_str[6]; 
	uint16_t rightX = OFFSET_X + (BLK_SIZE * COLS);
	uint16_t lateX = rightX + BLK_SIZE;
	current_score += 10;
	sprintf(score_str, "%u", current_score);
	GUI_Text(lateX, 75, (uint8_t *)score_str,  White, Black);

}

// controlla se una o più linee sono piene e, nel caso, le cancella
void checkLines(){
	int numLines = 0;
	int i, j, m, n;
	
	for(i = ROWS - 1; i >= 0; i--){
		
		for(j = 0; j < COLS; j++){
			
			if (game_field[i][j] != 0){
				continue;
			}else 
				break;

		}
		if (j >= COLS){
			numLines++;
			//deleteLine(i);
			for (m = i; m > 0; m--)
				for (n = 0; n < COLS; n++)
						game_field[m][n] = game_field[m-1][n];
			
			for (m = 0; m < COLS; m++)
				game_field[0][m] = 0;
			
			i++; // serve per controllare se ci sono pi? linee da cancellare
		}
	}
	
	if (numLines > 0){
		
		lines += numLines;
		
		current_score += (numLines == 4) ? 600 : 100 * numLines;
		
		for (i = ROWS-1; i >= 0; i--) {
			for (j = COLS-1; j >= 0; j--) {
					int lcdX = OFFSET_X + (j * BLK_SIZE);
					int lcdY = OFFSET_Y + (i * BLK_SIZE);
					drawBlock(lcdX, lcdY, game_field[i][j]);
				
			}
		}
	}
}
	
	
// crea un nuovo tetromino
// ritorna 0 se non è possibile creare un nuovo tetromino e fa terminare la partita
int spawnTetromino(){
	
	// funzione assembly per la generazione casuale del nuovo tetromino
	lfsr_seed = (uint8_t) lfsr_step(lfsr_seed, LFSR_TAP);
	
	uint8_t random = lfsr_seed % 7;
	
	currentTetromino = tetroShapes[random];
	currentTetromino.rotation = 0;
	currentTetromino.x = (COLS >> 1) - 2;
	currentTetromino.y = 0;
	
	if (checkCollision(currentTetromino.x, currentTetromino.y, currentTetromino.rotation)) {
				
				// non chiamo più updateField
        disable_timer(0);
				int i;
		
				if (current_score > high_score)
					high_score = current_score;
				
				state = GAME_OVER;
        
				LCD_Clear(Red);
				
				
        GUI_Text(80, 120, (uint8_t *) "GAME OVER", White, Red);
        GUI_Text(80, 160, (uint8_t *) "Score:", White, Red);
        
        
        char score_str[6];
				sprintf(score_str, "%u", current_score);
        GUI_Text(140, 160, (uint8_t *)score_str, White, Red);
				
				for(i = 1250000; i > 0; i--)
					__ASM("nop");
				
				return 0;
    }
		
		return 1;
	
}


// disegna il tetromino
void drawTetromino(){
	
	int i;
	int lcdX, lcdY;
	
	for (i = 0; i < 4; i++){
	
		int fieldPosX = currentTetromino.x + currentTetromino.blocks[currentTetromino.rotation][i][0];
		int fieldPosY = currentTetromino.y + currentTetromino.blocks[currentTetromino.rotation][i][1];
		
		lcdX = OFFSET_X + (fieldPosX * BLK_SIZE);
    lcdY = OFFSET_Y + (fieldPosY * BLK_SIZE);
        
    drawBlock(lcdX, lcdY, currentTetromino.color);
	}
}


// ruota il tetromino
void rotateTetramino() {
	
	int newRot = (currentTetromino.rotation + 1) % 4;
    
   
    if (checkCollision(currentTetromino.x, currentTetromino.y, newRot) == 0) {
        
        uint16_t oldColor = currentTetromino.color;
        currentTetromino.color = Black;
        drawTetromino();
        
        currentTetromino.rotation = newRot; 
        
        currentTetromino.color = oldColor;
        drawTetromino();
    }
} 

	
// sposta il tetromino a sinistra
void moveLeft() {
	
	//disable_RIT();
	
	if (checkCollision(currentTetromino.x - 1, currentTetromino.y, currentTetromino.rotation) == 0) {
        
        uint16_t oldColor = currentTetromino.color;
        currentTetromino.color = Black;
        drawTetromino();
        
        currentTetromino.x--; 
        
        currentTetromino.color = oldColor;
        drawTetromino();
    }
	
	//enable_RIT();
} 


// muove il tetromino a destra
void moveRight() {
	
	if (checkCollision(currentTetromino.x + 1, currentTetromino.y, currentTetromino.rotation) == 0) {
        
        uint16_t oldColor = currentTetromino.color;
        currentTetromino.color = Black;
        drawTetromino();
        
        currentTetromino.x++; 
        
        currentTetromino.color = oldColor;
        drawTetromino();
    }
} 


// fa cadere istantaneamente il tetromino nella sua posizione finale
void hardDrop() {
	
	// timer disabilitato per evitare di aggiornare lo stato del gioco 2 volte
	disable_timer(0);
	
	int y;
	
	for (y = currentTetromino.y; y < ROWS - 1; y++)
		if (checkCollision(currentTetromino.x, y+1, currentTetromino.rotation))
			break;
	
	uint16_t color = currentTetromino.color;
		
	currentTetromino.color = Black;
	drawTetromino();
			
	currentTetromino.y = y;
	currentTetromino.color = color;
	drawTetromino();
	
	updateField(currentTetromino.x, currentTetromino.y + 1, currentTetromino.rotation);
		/* y+1 serve per forzare il ramo if di update field, cioè per eseguire:
				- placeTetromino(currentTetromino.x, currentTetromino.y, currentTetromino.rotation);
				- checkLines();
				- printPoints();
				- if (spawnTetromino())
						drawTetromino();
		*/
		
	
	// riabilito il timer perché il turno è finito
	reset_timer(0);
	enable_timer(0);
				
} 
	
