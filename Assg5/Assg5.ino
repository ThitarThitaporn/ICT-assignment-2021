#define LED_W_R 6
#define LED_W_Y 7
#define LED_W_G 8
#define BT_W 3

#define LED_S_R 9
#define LED_S_Y 10
#define LED_S_G 11
#define BT_S 4

#define LED_P_R 12
#define LED_P_G 13
#define BT_P 5

#define goW 0
#define waitW 1
#define goS 2
#define waitS 3
#define bP1 4   //like waitP
#define goP 5
#define bP2 6   //blink
#define bP3 7   //blink
#define bP4 8   //blink
#define bP5 9   //blink

struct State {
  unsigned long ST_Out; // 8-bit pattern to street output
  unsigned long Time; // delay in msunits
  unsigned long Next[10];// next state for inputs 0,1,2,3
  }; 


typedef const struct State SType;
SType FSM[10] = {      
   // 3 2 1
  //  P S W           
  {B10001100, 2000,{goW, goW, waitW, waitW, waitW, waitW, waitW, waitW}},          //1         
  {B10001010, 300, {goS, goS, goS, goS, goP, goP, goS, goS}},                      //2       
  {B10100001, 2000,{goS, waitS, goS, waitS, waitS, waitS, waitS, waitS}},          //3       
  {B10010001, 300, {goW, goW, goW, goW, goP, goP, goP, goP}},                      //4       
  {B00001001, 200, {bP2, bP2, bP2, bP2, bP2, bP2, bP2, bP2}},                      //5 
  {B01001001, 2000,{goP, bP1, bP1, bP1, goP, bP1, bP1, bP1}},                      //6
  {B01001001, 200, {bP3, bP3, bP3, bP3, bP3, bP3, bP3, bP3}},                      //7
  {B00001001, 200, {bP4, bP4, bP4, bP4, bP4, bP4, bP4, bP4}},                      //8
  {B01001001, 200, {bP5, bP5, bP5, bP5, bP5, bP5, bP5, bP5}},                      //9
  {B00001001, 200, {goW, goW, goS, goW, goP, goW, goS, goW}},                      //10
};
unsigned long S = 0; //state start = 0
void setup() {
  Serial.begin(9600);
  pinMode(LED_W_R, OUTPUT);
  pinMode(LED_W_Y, OUTPUT);
  pinMode(LED_W_G, OUTPUT);
  pinMode(BT_W, INPUT);
  
  pinMode(LED_S_R, OUTPUT);
  pinMode(LED_S_Y, OUTPUT);
  pinMode(LED_S_G, OUTPUT);
  pinMode(BT_S, INPUT);
  
  pinMode(LED_P_R, OUTPUT);
  pinMode(LED_P_G, OUTPUT);
  pinMode(BT_P, INPUT);
}
int input=0, input1, input2, input3;
void loop() {
  digitalWrite(LED_P_G, FSM[S].ST_Out & B01000000);
  digitalWrite(LED_P_R, FSM[S].ST_Out & B10000000);
  
  digitalWrite(LED_S_R, FSM[S].ST_Out & B00001000);
  digitalWrite(LED_S_Y, FSM[S].ST_Out & B00010000);
  digitalWrite(LED_S_G, FSM[S].ST_Out & B00100000);
  
  digitalWrite(LED_W_R, FSM[S].ST_Out & B00000001);
  digitalWrite(LED_W_Y, FSM[S].ST_Out & B00000010);
  digitalWrite(LED_W_G, FSM[S].ST_Out & B00000100);
  
  
  delay(FSM[S].Time);
  
  input1 = digitalRead(BT_W);
  input2 = digitalRead(BT_S);
  input3 = digitalRead(BT_P);
  
  input=input3*4+input2*2+input1; //สมการเเปลงบิต
  S = FSM[S].Next[input];
  
  Serial.print(input1);
  Serial.print(input2);
  Serial.println(input3);
} 
