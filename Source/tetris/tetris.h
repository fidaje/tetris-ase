#include "LPC17xx.h"
#include "GLCD/GLCD.h"

// Configurazione del Gioco
#define ROWS 20
#define COLS 10
#define BLK_SIZE 15 // dimensioni in pixel del blocco del tetramino 
#define OFFSET_X 10
#define OFFSET_Y 10 // offset iniziale per il blocco in caduta


// Enum che indica gli stati del gioco
typedef enum {
	PAUSED,
	PLAYING,
	GAME_OVER
} GameState;

typedef struct {
	
	uint8_t type;
	uint16_t color;
	int x, y, rotation;
	int blocks[4][4][2];  // 4 possibili rotazioni, 4 blocchi, 2 coordinate per blocco
	
} Tetromino;


// Funzioni
void Tetris_Init(void);
void drawInterface(void);
void printPoints(void);

void updateField(int newX, int newY, int newRot);
int checkCollision(int newX, int newY, int newRot); 
void placeTetromino(int newX, int newY, int newRot);
void checkLines(void);
int spawnTetromino(void);
void drawTetromino(void);
void drawBlock(int x, int y, uint16_t color);

void rotateTetramino(void); 
void moveLeft(void); 
void moveRight(void); 
void hardDrop(void); 


