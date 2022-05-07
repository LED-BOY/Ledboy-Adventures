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
*/

const  uint8_t ledBoyHead[] = {
  0b11111100, 0b10100111, 0b10100111, 0b10100100, 0b10111111, 0b10100111, 0b10100100, 0b00111100
};

const  uint8_t ledBoyBody1[] = {
  0b00000010, 0b00111111, 0b00001111, 0b00001111, 0b00001111, 0b00111111, 0b00000010, 0b00000000
};

const  uint8_t ledBoyBody2[] = {
  0b00001000, 0b00001111, 0b00111111, 0b00001111, 0b00111111, 0b00001111, 0b00001000, 0b00000000
};

const  uint8_t enemy1A[] = {
  0b00010001, 0b11111001, 0b11101111, 0b00111000, 0b00111000, 0b11101111, 0b11111001, 0b00010001
};

const  uint8_t enemy1B[] = {
  0b00001010, 0b00111010, 0b11101110, 0b11111000, 0b11111000, 0b11101110, 0b00111010, 0b00100010
};

const  uint8_t cloud[] = {
  0b01001000, 0b01000100, 0b01000010, 0b01000010, 0b01000100, 0b01000100, 0b01011000, 0b00100000
};

const  uint8_t soild[] = {
  0b00000000, 0b00000100, 0b00000000, 0b00000000, 0b00100000, 0b00000001, 0b01000000, 0b00000000
};

const  uint8_t sun1[] = {
  0b00010010, 0b10001001, 0b01000111, 0b00101111, 0b00011111, 0b10011111, 0b01011111, 0b00101111
};

const  uint8_t sun2[] = {
  0b01000100, 0b00100011, 0b00010111, 0b10001111, 0b01011111, 0b00111111, 0b00011111, 0b00001111
};

const  uint8_t trees[] = {
  0b00000000, 0b00111100, 0b00100010, 0b11100001, 0b00000001, 0b11100001, 0b00100110, 0b00011000
};

const  uint8_t bird[] = {
  0b00110000, 0b00001000, 0b00000100, 0b00001000, 0b00110000, 0b00001000, 0b00000100, 0b00011000
};

const  uint8_t battIcon[] = {
  0x3f, 0x21, 0x21, 0x21, 0x21, 0x21, 0x33, 0x1e
};

const uint8_t titleText[] = {
  0x00, 0xff, 0x81, 0xbf, 0xa0, 0xa0, 0xa0, 0xe0, 0x00, 0xff, 0x81, 0xb5, 0xb1, 0xff, 0x00,
  0x00, 0xff, 0x81, 0x9d, 0x9d, 0x49, 0x22, 0x1c, 0x00, 0xff, 0x89, 0x89, 0x4a, 0x34, 0x00,
  0x3c, 0x42, 0x99, 0x99, 0x42, 0x3c, 0x03, 0x05, 0x06, 0xf8, 0x80, 0xf8, 0x06, 0x05, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x1b, 0x19, 0x19, 0x1b, 0xfc, 0x00, 0xff, 0x81, 0x9d,
  0x9d, 0x49, 0x22, 0x1c, 0x00, 0x07, 0x19, 0x23, 0x4c, 0x88, 0x4c, 0x23, 0x19, 0x07, 0x00,
  0xff, 0x81, 0xb5, 0xb1, 0xb1, 0xff, 0x00, 0xff, 0x83, 0xe6, 0x0c, 0x38, 0xdf, 0x81, 0xff,
  0x00, 0x03, 0x01, 0xfd, 0x81, 0xfd, 0x01, 0x03, 0x00, 0xff, 0x81, 0xbf, 0xa0, 0xbf, 0x81,
  0xff, 0x00, 0xff, 0xf9, 0x19, 0x39, 0xf6, 0xe0, 0x00, 0xff, 0x81, 0xb5, 0xb1, 0xff, 0x00,
  0x00, 0xfe, 0xb3, 0xb5, 0xb5, 0x85, 0xfb, 0x00
};
