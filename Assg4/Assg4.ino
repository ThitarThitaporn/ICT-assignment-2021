#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define WIDTH 128
#define HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 OLED(OLED_RESET);

void setup()
{
    
    OLED.begin(SSD1306_SWITCHCAPVCC, 0x3c);
    randomSeed(analogRead(A0));
}

//speaker and sound
int speakerPin = 3;
int numTones = 10;
int tones[] = {261,277,};

//Game
#define radius 3
int x = 64, y = 16;
int dx = 0, dy = 2;

#define barWidth 20
#define barHi 3
int barX, barY = HEIGHT - barHi;

int gameOver = false;

void loop()
{
    if (!gameOver)
    {
        // resistance = analogRead(A1)
        barX = map(analogRead(A1), 0, 1023, 0, 108);
        // Update
        x += dx;
        y += dy;
        if (x > WIDTH - radius)
        {
            dx = -dx;
            x = WIDTH - radius;
        }
        else if (x < radius)
        {
            dx = -dx;
            x = radius;
        }
        if (y < radius)
        {
            dy = -dy;
            y = radius;
        }

        /*
        ด้านซ้ายปอง < ด้านขวาบาร์ &&
        ด้านขวาปอง > ด้านซ้ายบาร์ &&
        ด้านล่างปอง > ด้านบนบาร์
        ** ตำแหน่งปองนับจากตรงกลาง
        ** ตำแหน่งบาร์นับจากซ้ายบน
        */

        if ((x - radius < barX + barWidth) && (x + radius > barX) && (y + radius > barY))
        {
            dy = -dy;
            dx = random(-2, 3); // สุ่มค่า -2 ถึง 2
        }

        if (y > HEIGHT)
        {
            gameOver = true;
        }
        OLED.clearDisplay();
        // Draw
        OLED.fillCircle(x, y, radius, WHITE);
        OLED.fillRect(barX, barY, barWidth, barHi, WHITE);
    }
    else
    {
        for(int i=0; i<numTones;i++)
        {
            tone(speakerPin,tones[i]);
        }        
        OLED.clearDisplay();
        // Say game over
        OLED.setTextColor(WHITE,BLACK); 
        OLED.setCursor(34,16); 
        OLED.setTextSize(1); 
        OLED.println("Game Over!!");
    }
    noTone(speakerPin);
    OLED.display();
}
