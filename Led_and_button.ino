/* Swamp cooler code by Willaim Strotz and D. Min.
 *  For cpe 301 Final progect.
 *  Revision 1.
 *  hello i am the spagetti man and this just exists for teahing myself github.
 */
 // Defines
 // Declarations
 volatile unsigned char* DDRLed = (unsigned char*) 0x21;
 volatile unsigned char* PortLed = (unsigned char*) 0x22; //LED wired that pa0 is green, pa1 is yellow, pa2 is red
 volatile unsigned char* PinLed = (unsigned char*) 0x20;
 
 //Protos (done automagically thru ardu apparently) lol guess not.
 unsigned int OnOffStat();
 unsigned int DetectErr();
 void WritePin(unsigned char Reg, unsigned char pin, unsigned char state);
 
 //Setup
 void setup()
 {
  Serial.begin(9600);
  *DDRLed |= 0b00000111; //sets led to outputs and button to an input
  volatile unsigned int State = 2; //state of machine, 1 green go, 2 yellow idle
 }
 //Main
 void loop()
 {
  Serial.println("fcukme");
  if (DetectErr() == 1)
  {
   WritePin(1,2,1); //R on
   WritePin(1,1,0); //Y off
   WritePin(1,0,0); //G off
   for (volatile unsigned int i = 0; i < 1000; i++); //wasting time
  }
  else if (DetectErr() == 0)
  {
   switch (OnOffStat()) //essentially 2 seperate paths for the 2 active states above
   {
    case 1: //green go state 
      WritePin(1,2,0); //R off
      WritePin(1,1,0); //Y off 
      WritePin(1,0,1); //G on
    break;

    case 2: //yellow idle state
      WritePin(1,2,0); //R off
      WritePin(1,1,1); //Y on
      WritePin(1,0,0); //G off
    break; 
   }
  }
 }
//functions
unsigned int OnOffStat()//detects button push, refrences the leds then outputs the machine idle/on state 1 green 2 idle
{
  if (*PinLed & 0b00001000) //button press?
  {
    for (volatile unsigned int i = 0; i < 1000; i++); //noise debounce
    while (*PinLed & 0b00001000); //debounce
    if (*PinLed & 0b00000001)//if last state was on
    {
      return 2;
    }
    else if (*PinLed & 0b00000010)//if last state was idle
    {
      return 1;
    }
  }
}
//-----------------
unsigned int DetectErr() //function should detect if an error is occuring. 1 if error 0 if none.
{
  return 0; //NOT FIANL, FOR TEST PURPOSES.
}
//-----------------
void WritePin(unsigned char Reg, unsigned char pin, unsigned char state)//function is a writing to pin , register a=1 b=2 etc00, pi, then on or off.
{
  switch (Reg)
  {
    case 1:
      if(state == 0)
      {
        *PortLed &= ~(0x01 << pin);
      }
      else
      {
      *PortLed |= 0x01 << pin;
      }
  } 
}
