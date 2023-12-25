#include "LedControl.h"
#include "FontLEDClock.h"
#include <EEPROM.h>

//Button
#define B1 2
#define B2 3
#define B3 4
#define Normal 0
#define SetTime 1
#define SetTimeHour 1
#define SetTimeMin 2
#define Stopwatch 2

int mode;
int stopwatchSec;
bool stopwatchRunning = true;

//LDR
int analogPin = 5;
int val = 0;

LedControl lc = LedControl(11, 13, 10, 4);              // DIN,CLK,CS,Number of LED Module
uint8_t intensity = 8;
uint8_t hour  = 12, minute = 10, second = 0; //Variable Clock mode

void setup()
{
  Serial.begin(9600);
  //Button
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);
  pinMode(B3, INPUT_PULLUP);
  if (EEPROM.read(0) < 60)
  {
    EEPROM.get(0, second);
  }
  if (EEPROM.read(1) < 60)
  {
    EEPROM.get(1, minute);
  }
  if (EEPROM.read(1) < 24)
  {
    EEPROM.get(2, hour);
  }

  //Dot Matrix
  int devices = lc.getDeviceCount();                    /* find no of LED modules */
  for (int address = 0; address < devices; address++)   /* set up each device         */
  { /* set up each device         */
    lc.shutdown(address, false);                        /* turn off power saving mode */
    lc.setIntensity(address, 2);                        /* set intensity=2 of 8       */
    lc.clearDisplay(address);                           /* clear screen               */
  }                                                     /* set up each device         */
  noInterrupts();           /* disable all interrupts             */
  TCCR1A  = 0;
  TCCR1B  = 0;
  TCNT1   = 3036;           /* preload timer 65536-16MHz/256/1Hz  */
  TCCR1B |= (1 << CS12);    /* 256 prescaler                      */
  TIMSK1 |= (1 << TOIE1);   /* enable timer overflow interrupt    */
  interrupts();             /* enable all interrupts              */
}

//Button
bool lastState;
bool buttonPressed(uint8_t pin)
{
  bool current = !digitalRead(pin);
  if (current && !lastState)
  {
    lastState = current;
    return true;
  }
  lastState = current;
  return false;
}

void clear_display()
{
  for (uint8_t address = 0; address < 4; address++)
    lc.clearDisplay(address);
}

void plot (uint8_t x, uint8_t y, bool value)
{
  uint8_t address;                                      /* select which matrix depending on the x coord  */
  if (x >= 0  && x <= 7)  {
    address = 3;
  }
  if (x >= 8  && x <= 15) {
    address = 2;
    x = x - 8;
  }
  if (x >= 16 && x <= 23) {
    address = 1;
    x = x - 16;
  }
  if (x >= 24 && x <= 31) {
    address = 0;
    x = x - 24;
  }

  if (value == 1)
    lc.setLed(address, y, x, true);
  else
    lc.setLed(address, y, x, false);
}

void print_tiny_char(uint8_t x, uint8_t y, char c)
{
  uint8_t dots;
  if (c >= 'A' && c <= 'Z' || (c >= 'a' && c <= 'z') )  c &= 0x1F;              /* A-Z maps to 1-26  */
  else if (c >= '0' && c <= '9')                        c  = (c - '0') + 32;
  else if (c == ' ' )                                   c  =  0;                /* space             */
  else if (c == '.' )                                   c  = 27;                /* full stop         */
  else if (c == ':' )                                   c  = 28;                /* colon             */
  else if (c == '\'')                                   c  = 29;                /* single quote mark */
  else if (c == '!' )                                   c  = 30;                /* single quote mark */
  else if (c == '?' )                                   c  = 31;                /* question mark     */

  for (uint8_t col = 0; col < 3; col++)
  {
    dots = pgm_read_byte_near(&mytinyfont[c][col]);
    for (uint8_t row = 0; row < 5; row++)
    {
      if (dots & (16 >> row))     plot(x + col, y + row, 1);
      else                      plot(x + col, y + row, 0);
    }
  }
}

void clocks()
{
  print_tiny_char( 2, 1,  hour / 10 + '0'); //print hour
  print_tiny_char( 6, 1,  hour % 10 + '0');
  print_tiny_char(12, 1, minute / 10 + '0'); //print minute
  print_tiny_char(16, 1, minute % 10 + '0');
  print_tiny_char(22, 1, second / 10 + '0'); //print second
  print_tiny_char(26, 1, second % 10 + '0');

  // blink : for each minute
  if (second == 0)
  {
    plot(10, 2, 0);
    plot(10, 4, 0);
    plot(20, 2, 0);
    plot(20, 4, 0);
  }
  else
  {
    plot(10, 2, 1);
    plot(10, 4, 1);
    plot(20, 2, 1);
    plot(20, 4, 1);
  }

  // display second count line
  if (second < 30)
  {
    plot(second + 1, 7, 1);
  }
  else
  {
    plot(60 - second, 7, 0);
  }
}

ISR(TIMER1_OVF_vect)        // interrupt service routine
{
  TCNT1 = 3036;             /* preload timern */
  second++;
  //Serial.println(second++);
  if (second > 59) {
    second = 0; minute++;
  }
  if (minute > 59) {
    minute = 0; hour++;
  }
  if (hour > 23) {
    hour = 0;
  }
}

void loop() {
  
  //Serial.println(digitalRead(B1));
  //Serial.println(digitalRead(B2));
  //Serial.println(digitalRead(B3));
  
  clocks();
  delay(100);

 //Button
  if (buttonPressed(B1)) // Mode
  {
    mode++;
    mode %= 3;
    Serial.println(mode);
    if (mode == Stopwatch)
    {
      stopwatchSec = 0;
      stopwatchRunning = true;
    }
  }
  if (buttonPressed(B2))
  {
    if (mode == Normal)
    {
      EEPROM.update(0, second);
      EEPROM.update(1, minute);
      EEPROM.update(2, hour);
    }
    else if (mode == SetTimeHour)
    {
      hour++;
    }
    else if (mode == SetTimeMin)
    {
      minute++;
    }
    else if (mode == Stopwatch)
    {
      stopwatchRunning = !stopwatchRunning;
    }
  }
  if (buttonPressed(B3)) // Down
  {
    if (mode == Normal)
    {
      EEPROM.update(0, second);
      EEPROM.update(1, minute);
      EEPROM.update(2, hour);
    }
    else if (mode == SetTimeHour)
    {
      hour--;
    }
    else if (mode == SetTimeMin)
    {
      minute--;
    }
    else if (mode == Stopwatch)
    {
      stopwatchRunning = !stopwatchRunning;
    }
  }
  
  //LDR
  int sensorValue = analogRead(A5);
  val = analogRead(analogPin);
  //Serial.print("val = ");
  //Serial.println(val);

   //Set Intensity with LDR
  if (val < 500)
  {
    for (uint8_t address = 0; address < 4; address++)
      lc.setIntensity(address, 8);
  }
  else
  {
    for (uint8_t address = 0; address < 4; address++)
      lc.setIntensity(address, 1);
  }
}
