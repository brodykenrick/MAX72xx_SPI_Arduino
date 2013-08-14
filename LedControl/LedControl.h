/*
 *    LedControl.h - A library for controlling LEDs with a MAX7219/MAX7221
 *    Copyright (c) 2007 Eberhard Fahle
 *
 *    Extended to (attempt to) support all alpha characters.
 *    Copyright (c) 2013 Brody Kenrick
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 * 
 *    This permission notice shall be included in all copies or 
 *    substantial portions of the Software.
 * 
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef LedControl_h
#define LedControl_h

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

//I don't use/test the LED matrix options so I am disabling that code
#define NO_LED_MATRIX

/*
 * Segments to be switched on for characters and digits on
 * 7-Segment Displays
 */

//aaaaaa
//f    b
//fggggb
//e    c
//eddddc [dec]p
//   pabcdefg


static const byte PROGMEM charTable[128] = {
    // Numbers : 0 .. 7
    B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,
    // Numbers : 89AbcdEF
    B01111111,B01111011,B01110111,B00011111,B00001101,B00111101,B01001111,B01000111,
    //Non-printable
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    // ' ' (space), '!'                                                    '(apostrophe)
    B00000000,B10100000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000010,
    // '(', '(', '*', '+'
    B00000000,B00000000,B00000000,B00000000,
    //',' '-' '.'/
    B10000000,B00000001,B10000000,
    // '/'
    B00000000,
    //
    // Characters of numbers : '0' .. '7'
    B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,
    //
    // Characters of numbers : '8' and '9'
    B01111111,B01111011,
    //':', ';', '<', '=', '>'
    B00000110,B00000110,B00000000,B00001001,B00000000,
    //'?' (BADish) [But we can make the preceding DP on instead (externally) which looks pretty good....]
    B01100101,
    //
    // '@'
    B00000000,
    //'A', 'B'>'b' .. 'F'
    B01110111,B00011111,B01001110,B00111101,B01001111,B01000111,
    //'G'>'g'
    B01111011,
    //
    //'H', 'I', 'J',
    B00110111,B00110000,B00111100,
    //'K'>k (BAD)
    B00001111,
    //'L'
    B00001110,
    //'M'>'n' (BAD- but if repeated looks OK) 'N'>'n' 'O'
    B00010101,B00010101,B01111110,
    //'P'
    B01100111,B01110011,B00000101,B01011011,B00001111,B00111110,
    //'V'>'U', 'W'>'U' (BAD- but if repeated looks OK)
    B00111110,B00111110,
    //
    // 'X'>'H' (BAD) , 'Y'>'y', 'Z' 
    B00110111,B00111011,B01101101,
    //'[' (interpreted as first half of double letter M), '\', ']' (interpreted as second half of double letter M) , '^' .. '_'
    B01100110,B00000000,B01110010,B00000000,B00001000,
    // '`'
    B00000000,
    // 'a', ,f,
    B01110111,B00011111,B00001101,B00111101,B01001111,B01000111,
    //'g'
    B01111011,
    //
    // 'h',   'i',     'j'
    B00010111,B00010000,B00111100,
    //'k' (BAD)
    B00001111,B00001110,
    //'m'>'n' (BAD - but if repeated looks OK), 'n', 'o'
    B00010101,B00010101,B00011101,
    //'p'
    B01100111,B01110011,B00000101,B01011011,B00001111,B00011100,
    // 'v'>'u', 'w'>'u' (BAD - but if repeated looks OK)
    B00011100,B00011100,
    //
    // 'x'>'H' (BAD), 'y', 'z'
    B00110111,B00111011,B01101101,
    // '{' (interpreted as first half of double letter W), '|', '}' (interpreted as second half of double letter M), '~', DEL
    B00011110,B00000110,B00111100,B01000000,B00000000
};


#define DOUBLE_CHAR_UPPERCASE_M_LHS  ('[')
#define DOUBLE_CHAR_UPPERCASE_M_RHS  (']')

#define DOUBLE_CHAR_UPPERCASE_W_LHS  ('{')
#define DOUBLE_CHAR_UPPERCASE_W_RHS  ('}')

#define DIGITS_PER_DISPLAY (8)
#define DIGITS_PER_DISPLAY_STR (DIGITS_PER_DISPLAY+1) //'\0' terminated

class LedControl
{
 private :
    /* The array for shifting the data to the devices */
    byte spidata[16];
    /* Send out a single command to the device */
    void spiTransfer(int addr, byte opcode, byte data);

    /* We keep track of the led-status for all 8 devices in this array */
    byte status[64];
    /* Data is shifted out of this pin*/
    int SPI_MOSI;
    /* The clock is signaled on this pin */
    int SPI_CLK;
    /* This one is driven LOW for chip selectzion */
    int SPI_CS;
    /* The maximum number of devices we use */
    int maxDevices;
    
 public:
 


 
    /* 
     * Create a new controler 
     * Params :
     * dataPin		pin on the Arduino where data gets shifted out
     * clockPin		pin for the clock
     * csPin		pin for selecting the device 
     * numDevices	maximum number of devices that can be controled
     */
    LedControl(int dataPin, int clkPin, int csPin, int numDevices=1);

    /*
     * Gets the number of devices attached to this LedControl.
     * Returns :
     * int	the number of devices on this LedControl
     */
    int getDeviceCount();

    /* 
     * Set the shutdown (power saving) mode for the device
     * Params :
     * addr	The address of the display to control
     * status	If true the device goes into power-down mode. Set to false
     *		for normal operation.
     */
    void shutdown(int addr, bool status);

    /* 
     * Set the number of digits (or rows) to be displayed.
     * See datasheet for sideeffects of the scanlimit on the brightness
     * of the display.
     * Params :
     * addr	address of the display to control
     * limit	number of digits to be displayed (1..8)
     */
    void setScanLimit(int addr, int limit);

    /* 
     * Set the brightness of the display.
     * Params:
     * addr		the address of the display to control
     * intensity	the brightness of the display. (0..15)
     */
    void setIntensity(int addr, int intensity);

    /* 
     * Switch all Leds on the display off. 
     * Params:
     * addr	address of the display to control
     */
    void clearDisplay(int addr);
#if !defined(NO_LED_MATRIX)
    /* 
     * Set the status of a single Led.
     * Params :
     * addr	address of the display 
     * row	the row of the Led (0..7)
     * col	the column of the Led (0..7)
     * state	If true the led is switched on, 
     *		if false it is switched off
     */
    void setLed(int addr, int row, int col, boolean state);

    /* 
     * Set all 8 Led's in a row to a new state
     * Params:
     * addr	address of the display
     * row	row which is to be set (0..7)
     * value	each bit set to 1 will light up the
     *		corresponding Led.
     */
    void setRow(int addr, int row, byte value);

    /* 
     * Set all 8 Led's in a column to a new state
     * Params:
     * addr	address of the display
     * col	column which is to be set (0..7)
     * value	each bit set to 1 will light up the
     *		corresponding Led.
     */
    void setColumn(int addr, int col, byte value);
#endif
    /* 
     * Display a hexadecimal digit on a 7-Segment Display
     * Params:
     * addr	address of the display
     * digit	the position of the digit on the display (0..7)
     * value	the value to be displayed. (0x00..0x0F)
     * dp	sets the decimal point.
     */
    void setDigit(int addr, int digit, byte value, boolean dp);

    /* 
     * Display a character on a 7-Segment display.
     * Tries to represent all characters. Some look bad but all are represented.
     * See charTable to see what is set.
     * Also note that some single characters are overloaded to be used
     *  for creating better double character representation of letters
     *  For example '[' ']' and '{' '}' are used to represent half of W and M
     * Params:
     * addr	address of the display
     * digit	the position of the character on the display (0..7)
     * value	the character to be displayed. 
     * dp	sets the decimal point.
     */
    void setChar(int addr, int digit, char value, boolean dp);
    
/* 
     * Display a string of characters on a 7-Segment display.
     * Tries to represent all characters but there are some that are just plain impossible.
     * See charTable to see what is set.
     * Params:
     * addr	    address of the display
     * text     Text to be used
     * decimals If the decimal point is set
     * max_length_of_in_arrays
     * duration_ms - Time in milliseconds to display the message (per 8 characters to display -- longer strings scroll and thus need more time)
     * delay_function - The function to be used for sleeping between characters (allows code that doesn't have busy use of delay() -- tested for low power operations with Narcoleptic)
     */
    void setDisplayAndScroll(int addr, const char * text, const boolean * decimals, int max_length_of_in_arrays,
                             unsigned long duration_ms, void (*delay_function)(unsigned long  duration_ms)   );

/* 
     * Static heper function that:
     * Modifies a string to better be shown on a 7-Segment display.
     * Doubles some characters -- w == uu, m == nn.
     * Moves decimal points back onto preceding letter  ("42.195"  is only 5 characters as the 2 has its DP set).
     * Uses special letters for W and M.
     * Puts the DP of a '?' onto preceding letter.
     *
     * Params:
     * in_text     Text to be modified
     * out_text     Modified text
     * out_decimals Modified texts DPs
     * length_of_out_arrays
     */
    void static modify_string_for_better_display(const char * in_text, char * out_text, boolean * out_decimals, int length_of_out_arrays);
};

#endif	//LedControl.h



