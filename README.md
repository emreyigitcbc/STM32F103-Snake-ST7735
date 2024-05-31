
# STM32F103 Snake Game with ST7735 TFT

A snake game for STM32F103 (bluepill board), with ST7735 TFT screen using stm32duino core!

![Main Picture](https://github.com/emreyigitcbc/STM32F103-Snake-ST7735/blob/main/misc/1.jpg?raw=true)

## Description
- This code is not for Stm32CubeIde or Keil!
- This code must be uploaded/compiled via Arduino IDE. (You can upload the bin file with Stm32CubeProgrammer)
- Tested on Arduino IDE v2
- Tested with STM32F103C8T6 original & clone.
- Works only for ST7735 chip. (RoboClass RC-10008, KMRTM2018-SPI V.1 board)

## Features
- If snake doesn't eat apple for 7000ms and its length greater than 4; it loses 1 piece.
- Apples are stale as time elapses. The earlier apples are eaten, the higher score earned.
- When snake eats apple, the game speed and snake length increases.
- Game region is shrinking randomly. (From top, bottom, left or right)
- When snake comes to border, it spawns other side of the screen.
![Screen](https://github.com/emreyigitcbc/STM32F103-Snake-ST7735/blob/main/misc/2.jpeg?raw=true)
![Loss Screen](https://github.com/emreyigitcbc/STM32F103-Snake-ST7735/blob/main/misc/3.jpeg?raw=true)
## Known Bugs
- For some reason, the colors are wrong. ST7735_RED is blue and ST7735_BLUE is red :D But ST7735_WHITE and ST7735_BLACK working well.
- When area shrinks and apple has been eaten recently or snake is on the border snake and apple may spawn inside the shrinked border (white place).

#### Porting for Other Chips
- It may work with other chips but you have to change Init code in the TFT library.
- In TFT.cpp file, below the TFT::begin function, you may uncomment "initR(INITR_REDTAB)" and comment initG(). See if it works. There are other init functions, check Adafruit_ST7735.cpp.


This code is based on George Sinodakis's STM32 Snake game:
https://github.com/GeorgeSinodakis/cpp_stm32f103_snakeGame

**Special thanks to George Sinodakis!**
