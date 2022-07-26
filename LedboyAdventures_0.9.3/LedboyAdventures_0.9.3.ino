/*LedBoyAdventures for Attiny series 0,1,2 MCUs
  Flash CPU Speed 5MHz.
  this code is released under GPL v3, you are free to use and modify.
  released on 2022.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    To contact us: ledboy.net
    ledboyconsole@gmail.com
*/
#include "tinyOLED.h"
#include "sprites.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>

#if defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERB0)
#error "This sketch does't use TCA0 nor TCB0 to save flash, select Disabled."
#endif

#define LEDON PORTA.OUT &= ~PIN3_bm;// led ON, also resets the oled screen.
#define LEDOFF PORTA.OUT |= PIN3_bm;// led OFF
#define BUTTONLOW !(PORTA.IN & PIN6_bm)// button press low
#define BUTTONHIGH (PORTA.IN & PIN6_bm)// button not pressed high
#define wdt_reset() __asm__ __volatile__ ("wdr"::) // watchdog reset macro

uint8_t bitShift1 = 8;
uint8_t bitShift2 = 0;
uint8_t jumpLenght = 19;
bool soildState = false;
bool ledState = true;
uint16_t timer = 0;
uint16_t timer2 = 0;
uint16_t timer3 = 0;
volatile uint8_t  frameCounter = 0;
volatile uint16_t interruptTimer = 0;

void setup() {
  _PROTECTED_WRITE(WDT.CTRLA, WDT_PERIOD_4KCLK_gc);// enable watchdog 1 sec. aprox.
  PORTA.DIR = 0b10000110; // setup ports in and out
  PORTA.PIN6CTRL = PORT_PULLUPEN_bm;// button pullup

  TCB0.INTCTRL = TCB_CAPT_bm; // Setup Timer B as compare capture mode to trigger interrupt
  TCB0_CCMP = 5000;// CLK
  TCB0_CTRLA = (1 << 1) | TCB_ENABLE_bm;

  // We will be outputting PWM on PA3 on an 8-pin part PA3 - TCA0 WO0, pin 4 on 8-pin parts
  PORTMUX.CTRLC     = PORTMUX_TCA00_ALTERNATE_gc; // turn off PORTMUX, returning WO0 PORTMUX_TCA00_DEFAULT_gc for PA3. PORTMUX_TCA00_ALTERNATE_gc; for PA7
  takeOverTCA0();                               // this replaces disabling and resettng the timer, required previously.
  TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); // Single slope PWM mode, PWM on WO0
  TCA0.SINGLE.PER   = 255;                   // Count all the way up to (255) - 8-bit PWM. At 5MHz, this gives ~19.607kHz PWM
  TCA0.SINGLE.CMP0  = 10; // 4% duty cycle 3.8v aprox
  TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm; // enable the timer with no prescaler

  sei(); // enable interrupts
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);// configure sleep mode

  while (readSupplyVoltage() > 3800 && BUTTONHIGH) { // If voltage is above 3.8v the led is on to increase Amp consumption and lower batt.
    PORTA.DIR = (1 << 3); // set led pin as output.
  }

  if (BUTTONHIGH) goToSleep(); // if button is high it is assumed that the watchdog bite and there is no intention to wake up.

  PORTA.DIR = (1 << 3); // pin3 (LED) as output

  for (uint8_t x = 4; x < 40; x += 34) { // led PWM and led blinking
    buttonDebounce();
    while (BUTTONHIGH) {

      if (interruptTimer % x == 0) {

        if (readSupplyVoltage() < 2300) goToSleep(); // To preserve the LIC to stay above 2.2v no operation is allowed under 2.3v
        LEDON
      } else {
        LEDOFF
      }
    }
    LEDOFF
  }
  oled.begin();// start oled screen
}

void loop() {
  clearScreen();// screen clear
  intro();
  clearScreen();
  game();
}

void game() {
  uint8_t lives = 1;
  uint8_t playerXPos = 16;
  uint8_t playerYPos = 2;
  uint8_t enemiesPos [3] = {46, 80, 120};
  uint8_t score = 0;
  uint8_t lastEnemyPos = 0;
  int8_t backgroundXPos = 0;
  const uint8_t skyElementsPos[8] = {0, 7, 10, 17, 31, 37, 60, 66};
  bool birdState = false;
  bool jump = false;

  while (BUTTONLOW);

  while (lives > 0) {

    if ((interruptTimer - timer) > 50) { // updates every 50 milliseconds
      uint8_t y = 0;
      timer = interruptTimer;

      for (uint8_t x = 0; x < 3; x++) { // enemies movement based on timer TCB0
        if (enemiesPos [x] < 1 || enemiesPos [x] > 200) {

          if (enemiesPos [x] == 0) { // score count when enemies reaches 0 position
            score += 2;
          }
          enemiesPos [x] = 127;
        } else {

          if (frameCounter < 30) { // kind of random function based on timer TCB0
            enemiesPos [x]--;
          }

          if (frameCounter >= 30 && frameCounter < 40) {
            enemiesPos [x] -= 2;
          }

          if (frameCounter >= 40) {
            enemiesPos [x] -= 3;
            jumpLenght = 13;
          } else {
            jumpLenght = 19;
          }
        }
      }

      if (playerYPos < 2) { // player sprite fordware movement while jumping
        playerXPos++;
      } else if (playerXPos > 16 && playerYPos == 2) {// after jumping returns to start position.
        drawSprite((playerXPos + 7), playerYPos, 0x00, true);
        playerXPos--;
        y = 6;
      }

      for (uint8_t x = 0; x < 127; x += 8) { // draw trees and soild

        if ( x < (playerXPos - 1) || playerYPos > 1) {// trees animation.
          drawSprite(x, 1, trees, !soildState);
        }

        if (x < (playerXPos - y) || x > (playerXPos + 7 - (y / y)) ||  playerYPos < 1) {// soild animation
          drawSprite(x, 2, soild, soildState);
          drawSprite(x, 3, soild, soildState);
        }
      }
      soildState = !soildState;
    }

    if ((interruptTimer - timer2) > 100) {

      if (readSupplyVoltage() < 2600) goToSleep();
      timer2 = interruptTimer;

      oled.drawLine(0, 0, 127, 0x00);// clear sky

      if (backgroundXPos > -127) {
        backgroundXPos -= 2;
      } else {
        backgroundXPos = 0;
      }

      bool cloudState = false;

      for (uint8_t x = 0; x < 8; x ++) {// sky animation / scrolling
        cloudState = !cloudState;
        drawSprite((backgroundXPos + skyElementsPos[x]), 0, cloud, cloudState);
      }
      drawSprite((backgroundXPos + 48), 0, bird, birdState);
      drawSprite((backgroundXPos + 90), 0, bird, birdState);
    }

    if (BUTTONLOW && playerYPos == 2) {// ledboy jump
      jump = true;
      playerYPos = 1;
    }

    if (playerYPos != 2) {// while jumping bitshift to scoll up

      if (bitShiftLedBoySprite(playerXPos , playerYPos, jump)) {

        if (playerYPos == 1 && jump) {
          playerYPos = 0;
        } else {
          jump = false;
          playerYPos++;
        }
      }
    }

    if (frameCounter < 25) {// ledboy, enemies, and sun  sprites animation
      birdState = !birdState;
      drawSprite(119, 0, sun1, false);

      if (playerYPos == 2) {
        drawSprite(playerXPos, playerYPos, ledBoyHead, 0);
        drawSprite(playerXPos, (playerYPos + 1), ledBoyBody1, 0);
      }

      for (uint8_t x = 0; x < 3; x ++) {
        drawSprite(enemiesPos [x], 3, enemy1A, false);
      }
    } else {
      drawSprite(119, 0, sun2, false);

      if (playerYPos == 2) {
        drawSprite(playerXPos, (playerYPos + 1), ledBoyBody2, 0);
      }
      for (uint8_t x = 0; x < 3; x ++) {
        drawSprite(enemiesPos [x], 3, enemy1B, false);
      }
    }

    for (uint8_t x = 0; x < 3; x ++) {// check enemies colition with ledboy to end game
      if (enemiesPos [x] > (playerXPos - 7) && enemiesPos [x] < (playerXPos + 7) && (playerYPos + 1) == 3) {
        lives = 0;
      }
      if (enemiesPos [x] > (playerXPos + 120) && enemiesPos [x] < (playerXPos + 128) && (playerYPos + 1) == 3) {
        lives = 0;
      }
    }
  }

  clearScreen();// show score based on a  line
  drawSprite(score, 1, ledBoyHead, 0);
  oled.drawLine(0, 2, score, 0xFF);
  while ((interruptTimer - timer) < 2000);
}

void intro(void) {// intro screen.
  buttonDebounce();
  interruptTimer = 0;

  for (uint8_t x = 8; x > 0 && BUTTONHIGH; x--) {
    drawTitle(0, 1, x);
    while ((interruptTimer - timer) < 220);
    timer = interruptTimer;
  }

  drawTitle(0, 1, 0);
  while (BUTTONHIGH) {

    if ((interruptTimer - timer) > 150) {
      for (uint8_t x = 0; x < 127; x += 8) {
        drawSprite(x, 2, soild, soildState);
        drawSprite(x, 3, soild, soildState);
      }
      drawSprite(60, 2, ledBoyHead, false);
      drawSprite(60, 3, ledBoyBody1, false);
      soildState = !soildState;
      uint8_t voltageReading = map(readSupplyVoltage(), 2600, 3800, 0, 117);
      if (voltageReading < 0) voltageReading = 0;
      oled.drawLine(10, 0, voltageReading, 0b00111111); // draws voltage meter
      drawSprite(0, 0, battIcon, 0);
      timer = interruptTimer;
    }

    if (interruptTimer > 7000) {
      clearScreen();
      oled.ssd1306_send_command(0xAE);
      goToSleep();
    }
  }
}

bool bitShiftLedBoySprite(uint8_t xPos, uint8_t yPos, bool scrollUp) { // bit shift ledboy to scroll it vertically
  bool finished = false;

  if (scrollUp) {
    drawLedBoy(xPos, yPos, ledBoyHead, bitShift1, scrollUp);

    if (yPos == 1) {
      drawLedBoy(xPos, (yPos + 2), ledBoyBody1, bitShift2, !scrollUp);
    }

    if (frameCounter % 2 == 0) {
      drawLedBoy(xPos, (yPos + 1), ledBoyHead, bitShift2, !scrollUp);
    } else {
      drawLedBoy(xPos, (yPos + 1), ledBoyBody1, bitShift1, scrollUp);
    }
  } else {
    drawLedBoy(xPos, (yPos + 1), ledBoyBody1, bitShift1, scrollUp);

    if (frameCounter % 2 == 0) {
      drawLedBoy(xPos, yPos, ledBoyBody1, bitShift2, !scrollUp);
    } else {
      drawLedBoy(xPos, yPos, ledBoyHead, bitShift1, scrollUp);
    }
  }

  if ((interruptTimer - timer3) > jumpLenght) {// also controls jump lenght
    timer3 = interruptTimer;

    if (bitShift1 == 0 && bitShift2 == 8) {
      bitShift1 = 8;
      bitShift2 = 0;
      finished = true;
    } else {
      bitShift1--;
      bitShift2++;
    }
  }
  return finished;
}

void clearScreen (void) {
  for ( uint8_t x = 0; x < 4; x++) {
    oled.drawLine(0, x, 128, 0x00);
  }
}

void drawLedBoy (uint8_t column, uint8_t page, uint8_t sprite[8], uint8_t bitShift, bool scrollUp) {
  oled.setCursor(column, page);// position cursor column - page
  oled.ssd1306_send_data_start();
  for (uint8_t x = 0; x < 8 && scrollUp; x++) {
    oled.ssd1306_send_data_byte(sprite[x] << bitShift);
  }
  for (uint8_t x = 0; x < 8 && !scrollUp; x++) {
    oled.ssd1306_send_data_byte(sprite[x] >> bitShift);
  }
  oled.ssd1306_send_data_stop();
}

void drawTitle (uint8_t column, uint8_t page, uint8_t bitShift) {
  oled.setCursor(column, page);// position cursor column - page
  oled.ssd1306_send_data_start();
  for (uint16_t x = 0; x < 128; x++) {
    oled.ssd1306_send_data_byte(eeprom_read_byte((uint8_t*)x) << bitShift);
  }
  oled.ssd1306_send_data_stop();
}

void drawSprite (uint8_t column, uint8_t page, uint8_t sprite[], bool mirrored) {
  oled.setCursor(column, page);// position cursor column - page
  oled.ssd1306_send_data_start();
  for (uint8_t x = 0; x < 8 && !mirrored; x++) {
    oled.ssd1306_send_data_byte(sprite[x]);
  }
  for (uint8_t x = 8; x > 0 && mirrored; x--) {
    oled.ssd1306_send_data_byte(sprite[x]);
  }
  oled.ssd1306_send_data_stop();
}

void buttonDebounce(void) {
  timer = interruptTimer;
  while (BUTTONLOW || (interruptTimer - timer) < 150); // super simple button debounce
}

void goToSleep (void) {
  PORTA.PIN6CTRL  |= PORT_ISC_BOTHEDGES_gc; //attach interrupt to portA pin 3 keeps pull up enabled
  //_PROTECTED_WRITE(WDT.CTRLA, 0);
  TCA0.SPLIT.CTRLA = 0; //disable TCA0 and set divider to 1
  TCA0.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc | 0x03; //set CMD to RESET to do a hard reset of TCA0.
  PORTA.OUT |= PIN7_bm;// P CHANNEL mosfet High to deactivate
  sleep_enable();
  sleep_cpu();// go to sleep
}

uint16_t readSupplyVoltage() { //returns value in millivolts  taken from megaTinyCore example
  analogReference(VDD);
  VREF.CTRLA = VREF_ADC0REFSEL_1V5_gc;
  // there is a settling time between when reference is turned on, and when it becomes valid.
  // since the reference is normally turned on only when it is requested, this virtually guarantees
  // that the first reading will be garbage; subsequent readings taken immediately after will be fine.
  // VREF.CTRLB|=VREF_ADC0REFEN_bm;
  // delay(10);
  uint16_t reading = analogRead(ADC_INTREF);
  reading = analogRead(ADC_INTREF);
  uint32_t intermediate = 1023UL * 1500;
  reading = intermediate / reading;
  return reading;
}

ISR(TCB0_INT_vect) {// timmer
  wdt_reset(); // reset watchdog
  interruptTimer++;

  if (frameCounter < 50) {
    frameCounter++;
  } else {
    frameCounter = 0;
  }
  TCB0_INTFLAGS = 1; // clear flag
}

ISR(PORTA_PORT_vect) {
  sleep_disable();
  _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
}
