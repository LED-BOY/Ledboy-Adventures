/*
    This file is part of LedBoyAdventures x.x.

    Foobar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <https://www.gnu.org/licenses/>.

    To contact us: ledboy.net
    ledboyconsole@gmail.com

   SSD1306xLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x32 displays

   @created: 2014-08-12
   @author: Neven Boyanov
   @date: 2020-04-14
   @author: Simon Merrett
   @version: 0.2

   Source code for original version available at: https://bitbucket.org/tinusaur/ssd1306xled
   Source code for ATtiny 0 and 1 series with megaTinyCore: https://github.com/SimonMerrett/tinyOLED
   megaTinyCore available at: https://github.com/SpenceKonde/megaTinyCore

   Changelog:
   - V0.2 use conditional #includes to take advantage of memory-saving TinyMegaI2CMaster library by David Johnson-Davies
   - V0.1 initial commit for sharing and comment. Main changes from Digistump and Tinusaur:
     - removed pgmspace use, as megaTinyCore doesn't need it to save arrays in flash (instead of RAM)
     - removed stdint, stdlib, interrupt, util/delay #includes
     - changed initialisation values, including to work initially only with 128*32 display
     - #defined _nofont_8x16 so that compiled code would fit on tiny402
     - removed Wire.writeAvailabe() from ssd1306_send_data_byte() as not using TinyWire wrapper
*/

#include <TinyMegaI2CMaster.h>

// ----------------------------------------------------------------------------

#define SSD1306		0x3C	// Slave address


// ----------------------------------------------------------------------------

class SSD1306Device: public Print {

  public:
    SSD1306Device(void);
    void begin(void);
    void ssd1306_send_command(uint8_t command);
    void ssd1306_send_data_byte(uint8_t byte);
    void ssd1306_send_data_start(void);
    void ssd1306_send_data_stop(void);
    void setCursor(uint8_t x, uint8_t y);
    void drawLine(uint8_t x0, uint8_t y0, uint8_t lineLenght, uint8_t dataValue);
    void ssd1306_send_command_start(void);
    void ssd1306_send_command_stop(void);
    virtual size_t write(byte c);
    using Print::write;
};

extern SSD1306Device oled;
