void setup() {Serial.begin(9600);
}
void loop()
{
while (Serial.available()) // recheck serial is available
{
  char inChar = (char)Serial.read(); 
 new byte;
 Serial.print(inChar);
} 
}
