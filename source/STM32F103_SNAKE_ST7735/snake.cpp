/*
* STM32F103 BLUEPILL ST7735 TFT SNAKE GAME
* w/stm32duino Core
* ************************************
* Author: Emre Cebeci
* GitHub: https://github.com/emreyigitcbc
* Date: 31 May 2024
* ************************************
This code is based on George Sinodakis's STM32 Snake game
https://github.com/GeorgeSinodakis/cpp_stm32f103_snakeGame

Special thanks to George Sinodakis!

Check snake.h for definitions.
*/

#include "snake.h"
#include <math.h>
#include <algorithm>
#include "TFT.h"
#include "stdlib.h"

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

int AREA_X_SIZE = ST7735_TFTWIDTH;
int AREA_Y_SIZE = ST7735_TFTHEIGHT;
// These are used for borders (shrinking)
int AREA_X_OFFSET_LEFT = 0;
int AREA_X_OFFSET_RIGHT = 0;
int AREA_Y_OFFSET_UP = 0;
int AREA_Y_OFFSET_DOWN = 2 * cube_size; // 16 pixels by default is for "Score: 0" text

long lastEat = 0;          // timer for apple point counter
long shrinkTimer = 0;      // timer for area shrinker
int speedVariable = 0;     // a variable depends on snake size to control game speed
int shrinkPlace = 0;       // randomly chosen shrink direction
int overAllPoints = 0;     // all points!
char scoreNumber[6] = "0"; // string of overAllPoints

// I implemented TFT library with this class because it is better and faster.
Game::Game(uint8_t CS, uint8_t RS, uint8_t RST) : TFT(CS, RS, RST)
{
  UP = {0, -1};
  DOWN = {0, +1};
  LEFT = {-1, 0};
  RIGHT = {+1, 0};
}

// Init of game, TFT screen also inits here.
void Game::init()
{
  begin();        // TFT Begin
  setRotation(2); // TFT Rotation (try 0,1,2,3)
  // If game restarted, clear snake and create new!
  snake.clear();
  snake.push_back(std::make_pair(7, 5));
  snake.push_back(std::make_pair(6, 5));
  snake.push_back(std::make_pair(5, 5));
  snake.push_back(std::make_pair(4, 5));
  // Set borders by default
  AREA_X_OFFSET_LEFT = 0;
  AREA_X_OFFSET_RIGHT = 0;
  AREA_Y_OFFSET_UP = 0;
  AREA_Y_OFFSET_DOWN = 2 * cube_size;
  // Timers & point system
  lastEat = 0;
  shrinkTimer = 0;
  shrinkPlace = 0;
  overAllPoints = 0;
  itoa(overAllPoints, scoreNumber, 10); // converting overAllPoints to char[]
  // Set apple location
  apple = {3, 7};
  // Set start direction
  dir = DOWN;
  shrinkTimer = millis();
  // Clear screen & setup
  fillScreen(backgroundColor);
  fillRect(0, AREA_Y_SIZE - 2 * cube_size, ST7735_TFTWIDTH, 2 * cube_size, borderColor);
  stroke(0, 0, 0); // Text color
  setTextSize(1);  // Text size
  text("Your Score:", 5, AREA_Y_SIZE - 8);
  text("0", 78, AREA_Y_SIZE - 8);
}

void Game::draw()
{
  /* Shrink Controller
  It randomly shrinks game area every SHRINK_TIMER ms
  There are some checks for if snake or apple is in pending shrink area, it does not shrink that area
  but this part is a little bit buggy.
  */

  if (SHRINK_AREA)
  {
    if (millis() - shrinkTimer > SHRINK_TIMER)
    {
      // shrink randomly
      shrinkPlace = random(1, 6);
      if (shrinkPlace == 1 && AREA_Y_OFFSET_UP <= 3 * cube_size && snake[0].second > AREA_Y_OFFSET_UP / cube_size && snake[snake.size() - 1].second > AREA_Y_OFFSET_UP / cube_size && apple.second > AREA_Y_OFFSET_UP / cube_size)
      {
        // top
        AREA_Y_OFFSET_UP += 1 * cube_size;
        fillRect(0, 0, ST7735_TFTWIDTH, AREA_Y_OFFSET_UP, borderColor);
      }
      else if (shrinkPlace == 2 && AREA_Y_OFFSET_DOWN <= 3 * cube_size && snake[0].second < (AREA_Y_SIZE - AREA_Y_OFFSET_DOWN) / cube_size && snake[snake.size() - 1].second < (AREA_Y_SIZE - AREA_Y_OFFSET_DOWN) / cube_size && apple.second < (AREA_Y_SIZE - AREA_Y_OFFSET_DOWN) / cube_size)
      {
        // bottom
        AREA_Y_OFFSET_DOWN += 1 * cube_size;
        fillRect(0, ST7735_TFTHEIGHT - AREA_Y_OFFSET_DOWN, ST7735_TFTWIDTH, AREA_Y_OFFSET_DOWN, borderColor);
      }
      else if (shrinkPlace == 3 && AREA_X_OFFSET_RIGHT <= 3 * cube_size && snake[0].first < (AREA_X_SIZE - AREA_X_OFFSET_RIGHT) / cube_size && snake[snake.size() - 1].first < (AREA_X_SIZE - AREA_X_OFFSET_RIGHT) / cube_size && apple.first < (AREA_X_SIZE - AREA_X_OFFSET_RIGHT) / cube_size)
      {
        // right
        AREA_X_OFFSET_RIGHT += 1 * cube_size;
        fillRect(ST7735_TFTWIDTH - AREA_X_OFFSET_RIGHT, 0, AREA_X_OFFSET_RIGHT, ST7735_TFTHEIGHT, borderColor);
      }
      else if (shrinkPlace == 4 && AREA_X_OFFSET_LEFT <= 3 * cube_size && snake[0].first > AREA_X_OFFSET_LEFT / cube_size && snake[snake.size() - 1].first > AREA_X_OFFSET_LEFT / cube_size && apple.first > AREA_X_OFFSET_LEFT / cube_size)
      {
        // left
        AREA_X_OFFSET_LEFT += 1 * cube_size;
        fillRect(0, 0, AREA_X_OFFSET_LEFT, ST7735_TFTHEIGHT, borderColor);
      }
      shrinkTimer = millis();
    }
  }
  // GAME SPEED CONTROLLER
  speedVariable = snake.size() * 13;
  if (speedVariable < 0)
    speedVariable = 0;
  delay(GAME_SPEED - speedVariable);
  u16 color;
  u16 x, y;

  // erase last cube of the snake and redraw it
  x = last.first * cube_size;
  y = last.second * cube_size;
  fillRect(x, y, cube_size, cube_size, backgroundColor);
  if (x < AREA_X_OFFSET_LEFT || y < AREA_Y_OFFSET_UP)
    fillRect(x, y, cube_size, cube_size, borderColor);
  if (x >= AREA_X_SIZE - AREA_X_OFFSET_RIGHT || y >= AREA_Y_SIZE - AREA_Y_OFFSET_DOWN)
    fillRect(x, y, cube_size, cube_size, borderColor);
  delay(10);
  for (int i = 0; i < snake.size(); i++)
  {
    x = snake[i].first * cube_size;
    y = snake[i].second * cube_size;
    fillRect(x, y, cube_size, cube_size, snakeColor);
  }
  // draw apple
  x = apple.first * cube_size;
  y = apple.second * cube_size;
  fillRect(x, y, cube_size, cube_size, appleColor);
  // draw score
  stroke(0, 0, 0);
  setTextSize(1);
  fillRect(0, AREA_Y_SIZE - 2 * cube_size, ST7735_TFTWIDTH, 2 * cube_size, borderColor);
  text("Your Score:", 5, AREA_Y_SIZE - 8);
  text(scoreNumber, 78, AREA_Y_SIZE - 8);
}

void Game::move()
{
  // if the apple has not been eaten
  if (snake[0] != apple)
  {
    last = snake[snake.size() - 1];
    snake.pop_back();
  }
  else
  {
    int plus = (45000 / (millis() - lastEat)) + 1;
    overAllPoints += plus;
    itoa(overAllPoints, scoreNumber, 10);
    new_apple();
  }

  if (lastEat != 0 && millis() - lastEat > 7000 && snake.size() > 4)
  {
    lastEat = millis();
    lastPiece = snake[snake.size() - 1];
    snake.pop_back();
    fillRect(lastPiece.first * cube_size, lastPiece.second * cube_size, cube_size, cube_size, ST7735_BLACK);
  }

  snake.insert(snake.begin(), {snake[0].first, snake[0].second});
  snake[0].first += dir.first;
  snake[0].second += dir.second;

  if (snake[0].first < AREA_X_OFFSET_LEFT / cube_size)
    snake[0].first = AREA_X_SIZE / cube_size - AREA_X_OFFSET_RIGHT / cube_size - 1;
  if (snake[0].second < AREA_Y_OFFSET_UP / cube_size)
    snake[0].second = AREA_Y_SIZE / cube_size - AREA_Y_OFFSET_DOWN / cube_size - 1;
  if (snake[0].first >= (AREA_X_SIZE / cube_size - AREA_X_OFFSET_RIGHT / cube_size))
    snake[0].first = AREA_X_OFFSET_LEFT / cube_size;
  if (snake[0].second >= (AREA_Y_SIZE / cube_size - AREA_Y_OFFSET_DOWN / cube_size))
    snake[0].second = AREA_Y_OFFSET_UP / cube_size;
}

void Game::loss_check()
{
  for (auto cube : snake)
  {
    if (std::count(snake.begin(), snake.end(), cube) > 1)
    {
      loss_message();
      // reset the game
      init();
      return;
    }
  }
}
void Game::loss_message()
{
  String score = "Score: ";
  fillScreen(lossBackColor);
  stroke(255, 255, 255);
  setTextSize(2);
  text("You lost!!!", 0, 40);
  text("Score:", 0, 90);
  text(scoreNumber, 80, 90);
  delay(2000);
}

void Game::new_apple()
{
  lastEat = millis();

  int x, y;
  bool flag = false;
  bool flag2 = false;
  while (!flag)
  {
    flag = true;
    while (!flag2)
    {
      x = rand() % (AREA_X_SIZE);
      y = rand() % (AREA_Y_SIZE);
      if (x > AREA_X_OFFSET_LEFT && x < (AREA_X_SIZE - AREA_X_OFFSET_RIGHT) && y > AREA_Y_OFFSET_UP && y < (AREA_Y_SIZE - AREA_Y_OFFSET_DOWN))
        flag2 = true;
    }
    x = x / cube_size;
    y = y / cube_size;

    for (int i = 0; i < snake.size(); i++)
    {
      if (snake[i] == std::make_pair(x, y))
        flag = false;
    }
    if (snake[0].first == x || snake[0].second == y)
      flag = false;
  }
  apple = std::make_pair(x, y);
}

void Game::goUp()
{
  if (dir == LEFT || dir == RIGHT)
    dir = UP;
}

void Game::goDown()
{
  if (dir == LEFT || dir == RIGHT)
    dir = DOWN;
}

void Game::goLeft()
{
  if (dir == UP || dir == DOWN)
    dir = LEFT;
}

void Game::goRight()
{
  if (dir == UP || dir == DOWN)
    dir = RIGHT;
}