/*
 * STM32F103 BLUEPILL ST7735 TFT SNAKE GAME
 * w/stm32duino Core
 * ************************************
 * Author: Emre Cebeci
 * GitHub: https://github.com/emreyigitcbc
 * Date: 31 May 2024
 * ************************************
 */
#include <vector>
#include <utility> // std::pair, std::make_pair
#include <TFT.h>

// dont recommend changing this
#define cube_size (int)8

// if you dont want to shrink the game area by time set this to 0
#define SHRINK_AREA 1
// shrink timer (ms)
#define SHRINK_TIMER 7000
// the formula for game speed is:
// GAME_SPEED - (13 * SNAKE_LENGTH)
// higher values mean SLOWER, lower values mean FASTER
#define GAME_SPEED 190

// background colors for game screen
#define backgroundColor ST7735_BLACK // overall background color
#define borderColor ST7735_WHITE	 // if SHRINK_AREA is 1, the border color
#define lossBackColor ST7735_BLUE	 // game over back color

// these colors are inverted. Blue is red, red is blue :D
#define appleColor ST7735_BLUE
#define snakeColor ST7735_RED

class Game : public TFT
{
public:
	std::pair<int, int> UP;
	std::pair<int, int> DOWN;
	std::pair<int, int> LEFT;
	std::pair<int, int> RIGHT;
	std::vector<std::pair<int, int>> snake;
	std::pair<int, int> apple;
	std::pair<int, int> dir;
	std::pair<int, int> last;
	std::pair<int, int> lastPiece;

public:
	Game(uint8_t CS, uint8_t RS, uint8_t RST);
	void init();
	void draw();
	void move();
	void loss_check();
	void new_apple();
	void loss_message();
	void goUp();
	void goDown();
	void goLeft();
	void goRight();
};