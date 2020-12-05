/* Swamp cooler code by Willaim Strotz and D. Min.
 *  For cpe 301 Final progect.
 *  Revision 3.
 *  fixed count duku
 */
 // Includes
 //#include <Wire.h>
 #include <RTClib.h>
 #include <Servo.h>
 #include <LiquidCrystal.h>
 #include <dht.h>
 // Defines
 RTC_DS1307 rtc;
 Servo vent;
 dht DHT;
 #define maxtemp 26
 
 // Declarations
 volatile unsigned char* DDRLed = (unsigned char*) 0x21;
 volatile unsigned char* PortLed = (unsigned char*) 0x22; //LED wired that pa0 is green, pa1 is yellow, pa2 is red
 volatile unsigned char* PinLed = (unsigned char*) 0x20;
 const byte seconds = 0;//initial time is set here
 const byte hours = 0;
 const byte minutes = 0;
 volatile unsigned int Pos = 0;
 volatile unsigned int prev = 0;
 LiquidCrystal lcd(13, 12, 11, 10, 8, 7); //maps the pins to the lcd
 volatile unsigned int State = 2; //state of machine, 1 green go, 2 yellow idle
 unsigned int dispCounter = 0;
 unsigned int CountDuku = 0; //counters for the serial
 unsigned int CountDuku2 = 0;
 unsigned int CountDuku3 = 0;
 unsigned int CountDuku4 = 0;
 unsigned int ct = 0;
 unsigned int ret = 0;
 
 //Protos (done automagically thru ardu apparently) lol guess not.
 unsigned int OnOffStat();
 unsigned int DetectErr();
 void WritePin(unsigned char Reg, unsigned char pin, unsigned char state);
 void GetTime();
 void DispStats();
 
 //Setup
 void setup()
 {
  Serial.begin(9600);
  rtc.begin();//start clock
  vent.attach(9);//attaches servo to analog pin 9
  lcd.begin(16, 2);
  *DDRLed |= 0b00010111; //sets led to outputs and button to an input
 }
 //Main
 void loop()
 {
  dispCounter++;;
  if (dispCounter <= 2000)
  {
    DispStats();
    dispCounter = 0;
  }
  Pos = analogRead(2);
  Pos = map(Pos, 0, 1023, 0, 180);
  vent.write(Pos);
  if ((DetectErr()) == 1 && (CountDuku == 0))
  {
   WritePin(1,2,1); //R on
   WritePin(1,1,0); //Y off
   WritePin(1,0,0); //G off
   WritePin(1,4,0); // motor off
   Serial.print("Low water, refil to resume ");
   GetTime();
   CountDuku++;
  }
  else if (DetectErr() == 0)
  {
   CountDuku = 0;
   switch (OnOffStat()) //essentially 2 seperate paths for the 2 active states above
   {
    case 1: //green go state 
      CountDuku4 = 0;
      WritePin(1,2,0); //R off
      WritePin(1,1,0); //Y off 
      WritePin(1,0,1); //G on
      if (ct > 26)
      {
       WritePin(1,4,1);
       if (CountDuku2 == 0)
       {
        Serial.print("Fan on at ");
        GetTime();
        CountDuku2++; 
        CountDuku3 = 0;
       }
      }
      else if (ct < 26)
      {
       WritePin(1,4,0);
       if (CountDuku3 == 0)
       {
        Serial.print("Fan off at ");
        GetTime();
        CountDuku3++;
       }
      }
    break;

    case 2: //yellow idle state
      CountDuku2 = 0;
      CountDuku3 = 0;
      WritePin(1,2,0); //R off
      WritePin(1,1,1); //Y on
      WritePin(1,0,0); //G off
      WritePin(1,4,0);
      if (CountDuku4 == 0)
      {
       Serial.print("Cooling diabled at ");
       GetTime();
       CountDuku4++;
      }
    break; 
   }
  }
 }
//functions------------
unsigned int OnOffStat()//detects button push, refrences the leds then outputs the machine idle/on state 1 green 2 idle
{
  long t = 0;
  long db = 200;
  unsigned int red = 0;
  red = (*PinLed & 0b00001000);
  if ((*PinLed & 0b00001000)&&(prev == 0)&&(millis() - t > db)) //button press and debug?
  {
     if (*PinLed & 0b00000001)//if last state was on
      {
       ret = 2;
      }
      else
      {
       ret = 1;
      }
     t = millis(); 
  }
 prev = red;
 return ret;
}
//-----------------
unsigned int DetectErr() //function should detect if an error is occuring. 1 if error 0 if none.
{
  if (analogRead(10) < 200)
  {
   return 1;
  }
 return 0;
}
//-----------------
void WritePin(unsigned char Reg, unsigned char pin, unsigned char state)//function is a writing to pin , register a=1 b=2 etc00, pi, then on or off.
{
  switch (Reg)
  {
    case 1: //for port a 
      if(state == 0)
      {
       *PortLed &= ~(0x01 << pin);
      }
      else
      {
       *PortLed |= 0x01 << pin;
      }
      break;   
  }  
}
//-----------------
void GetTime()//prints the real time time and date
{
  DateTime now = rtc.now();
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day());
  Serial.print("/");
  Serial.print(now.year());
  Serial.print("  ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.print(now.second());
  Serial.println();
}
//-----------------
void DispStats() //displays temp and humiditiy on lcd
{
  int chk = DHT.read11(5); // checks for a good read
  if (0 == chk)
  {
  lcd.begin(16,2);
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(ct = DHT.temperature);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(DHT.humidity);
  lcd.print("%"); 
  }
}
//-----------------
