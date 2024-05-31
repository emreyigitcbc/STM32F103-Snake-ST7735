/*
 * STM32F103 BLUEPILL ST7735 TFT SNAKE GAME
 * w/stm32duino Core
 * ************************************
 * Author: Emre Cebeci
 * GitHub: https://github.com/emreyigitcbc
 * Date: 31 May 2024
 * ************************************
 * This code is not for Stm32CubeIde or Keil!
 * This code must be uploaded/compiled via Arduino IDE.
 * Tested on Arduino IDE v2
 * Tested with STM32F103C8T6 original & clone.
 * Works only for ST7735 chip. (RoboClass RC-10008, KMRTM2018-SPI V.1 board)
 * ************************************
 * For other chips:
 * It may work with other chips but you have to change Init code in the TFT library.
 * In TFT.cpp file, below the TFT::begin function, you may uncomment "initR(INITR_REDTAB)"
 * and comment initG(). See if it works. There are other init functions, check Adafruit_ST7735.cpp.
 * ************************************
 * Features:
 * - If snake doesn't eat apple for 7000ms and its length greater than 4; it loses 1 piece.
 * - Apples are stale as time elapses. The earlier apples are eaten, the higher score
 * earned.
 * - When snake eats apple, the game speed and snake length increases.
 * - Game region is shrinking randomly. (From top, bottom, left or right)
 * - When snake comes to border, it spawns other side of the screen.
 * ************************************
 * Known Bugs:
 * - For some reason, the colors are wrong. ST7735_RED is blue and ST7735_BLUE is red :D
 * but ST7735_WHITE and ST7735_BLACK working well.
 * - When area shrinks and apple has been eaten recently or snake is on the border
 * snake and apple may spawn inside the shrinked border (white place)
 */

// SPI library from stm32duino core
#include <SPI.h>
// TFT library with ST7735
#include <TFT.h>
#include "snake.h"

// PIN DEFINITION FOR STM32F103
#define CS PA3
#define DC_RS PA4
#define RST PA2
/*
Button Connections By default:
**********
PA9 : UP
PA10: DOWN
PA11: LEFT
PA12: RIGHT
*/

Game snake = Game(CS, DC_RS, RST);

void setup()
{
  // Init snake
  snake.init();
  // Set pins to input; these are buttons to move snake.
  // Connect buttons to GND
  pinMode(PA9, INPUT_PULLUP);
  pinMode(PA10, INPUT_PULLUP);
  pinMode(PA11, INPUT_PULLUP);
  pinMode(PA12, INPUT_PULLUP);
}
void checkButtons()
{

  if (!digitalRead(PA9))
  {
    snake.goUp();
  }
  if (!digitalRead(PA10))
  {
    snake.goDown();
  }
  if (!digitalRead(PA11))
  {
    snake.goLeft();
  }
  if (!digitalRead(PA12))
  {
    snake.goRight();
  }
}

void loop()
{
  snake.draw();
  snake.loss_check();
  checkButtons();
  snake.move();
}