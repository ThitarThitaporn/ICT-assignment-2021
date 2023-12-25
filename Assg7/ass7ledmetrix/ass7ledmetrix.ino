//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
//#include <TimerOne.h>
#include <EEPROM.h>
#define OLED_RESET -1
Adafruit_SSD1306 OLED(OLED_RESET);
#define BA 2
#define BB 3
#define BC 4
#define Normal 0
#define SetTime 1
#define Stopwatch 2
int mode;
uint8_t second;
uint8_t minute;
uint8_t hour;
int stopwatchSec;
bool stopwatchRunning = true;

void setup()
{
    Serial.begin(9600);
    OLED.begin(SSD1306_SWITCHCAPVCC, 0x3c);
    OLED.setTextColor(WHITE);

    pinMode(BA, INPUT_PULLUP);
    pinMode(BB, INPUT_PULLUP);
    pinMode(BC, INPUT_PULLUP);

    Timer1.initialize(); // 1 s
    Timer1.attachInterrupt(countTime);

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
}
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

void loop()
{
    if (buttonPressed(BA)) // Mode
    {
        mode++;
        mode %= 3;
        if (mode == Stopwatch)
        {
            stopwatchSec = 0;
            stopwatchRunning = true;
        }
    }
    if (buttonPressed(BB)) // Up Hour
    {
        if (mode == Normal)
        {
            EEPROM.update(0, second);
            EEPROM.update(1, minute);
            EEPROM.update(2, hour);
        }
        else if (mode == SetTime)
        {
            hour++;
            hour %= 24;
        }
        else if (mode == Stopwatch)
        {
            stopwatchRunning = !stopwatchRunning;
        }
    }
    if (buttonPressed(BC)) // Up minute
    {
        if (mode == Normal)
        {
            EEPROM.update(0, second);
            EEPROM.update(1, minute);
            EEPROM.update(2, hour);
        }
        else if (mode == SetTime)
        {
            minute++;
            minute %= 60;
        }
        else if (mode == Stopwatch)
        {
            stopwatchRunning = !stopwatchRunning;
        }
    }
    OLED.clearDisplay();
    // Draw
    OLED.display();
}

void countTime()
{
    second++;
    if (second == 60)
    {
        second = 0;
        minute++;
    }
    if (minute == 60)
    {
        minute = 0;
        hour++;
    }
    hour %= 24;

    if (stopwatchRunning)
    {
        stopwatchSec++;
    }
    
}
