/************************************************************************************************************
Frequency measuring programm for experience with programmable light sensor TCS 3200
The Programm doesn't realy measure the frequency, the period lenght is measure contiously and converted to
frequency. 
The fototimer-shield hardware is use and for frequency input the rotary encoder inputs were used.
So the rotary encoder functionaltity was dropped.
*************************************************************************************************************/

// set pin numbers and other constants:
const int batteryPin = 1;  // the number of the analog input used for the battery watching

const int frequencein = 3; // measuring input signal 
const int contrastPin = 5;   // the number of the pwr signal pin for contrast voltage
const char softvers[] = "Frequenzzaehler ";

// include library codes:
#include <LiquidCrystal.h>

// global variables
int frequencestate = HIGH; // measuring input signal 
int contrastValue = 0; // display contrast level
// *** notice, battery not used yet ***
int lowbattery = 611; // level when battery is low Voltage=AnalogIn*0.01014
// measuring and calculate
double period = 0; //
double frequency = 0; //
// timestamps for measuring
unsigned long timestamp1 = 0; // first timestamp
unsigned long timestamp2 = 0; // second timestamp
// action controlling
int actioncounter = 0; // counts for timing actions
// frequncy counter
double freqcounter = 0;


// initialize the library with the numbers of the interface pins
/*  The LCD circruit
 * LCD RS pin to digital pin 13
 * LCD Enable pin to digital pin 12
 * LCD D4 pin to digital pin 11
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 9
 * LCD D7 pin to digital pin 8
 * LCD R/W pin to ground
 * LCD V0 (Pin 3) over RC and OPAMP to PWR Pin5 (Contrast)
*/
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);


// function definitions
void sendlogdata(void); // send data thru serial for logging 


/************************************
       setup all the stuff
************************************/  
void setup()
{
  // initialise serial, results will be send thru serial for logging by computer
  Serial.begin(9600);
  // set display contrast
  pinMode(contrastPin, OUTPUT);
  analogWrite(contrastPin, contrastValue);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16,2);
  // print softwareversion message to the LCD and wait
  lcd.setCursor(0, 0);
  lcd.print(softvers);
  // wait for serial monitor opened and send message!
  while(!Serial); // wait for serial port to connect. Needed for Leonardo only
  Serial.print("Welcome!\n");
  // wait one second
  delay( 1000 );
  // set up the analog reference, not used yet, is for battery watching and analog keyboard
  analogReference(EXTERNAL);
  // measuring input pin initialize
  pinMode(frequencein, INPUT);
  // activate interrupt for measuring
  attachInterrupt(1, measure, FALLING);
  // clear display
  lcd.clear();
  }

/************************************
       main loop begins here
************************************/  
void loop() 
  {
  // read state from measuring input
  frequencestate = digitalRead(frequencein);
  // set cursor to measure signal indication
  lcd.setCursor(15, 0);
  if (frequencestate == LOW)
    {
    // yes, there is a signal
    lcd.print("M");
    }
  else
    {
    // no signal
    lcd.print(" ");
    }  
  // calculate result of period
  period = double(timestamp2 - timestamp1); // double length of the pulse
  // refresh display cycle time 100mS
  // and measuring cycle
  // first reset counter
  freqcounter = 0;
  // wait 100mS
  delay (100);
  // calculate frequency from counter
  frequency = freqcounter / 100;
  // to avoid division by Zero, check first ...
  // calculate frequency from period if bigger than 999µS
  if (period > 999)
    {
    // in this case, frequnency measuring will be overwritten
    frequency = 1000 / period; // calculate the frequency
    }
  // counts loops
  ++actioncounter;
  // every 10 seconds action
  if (actioncounter > 100)
    {
    actioncounter = 0; // reset counter
    sendlogdata(); // send data
    }
  // set cursor to second line and display results
  lcd.setCursor(0, 0);
  lcd.print(period,0);
  lcd.print("uS     ");
  lcd.setCursor(0, 1);
  lcd.print(frequency,3);
  lcd.print("kHz    ");
  }

/************************************
       measure interrupt routine
************************************/    
void measure()
  {
  // shift last time
  timestamp1 = timestamp2; 
  // new time
  timestamp2 = micros();
  // frequency counter
  ++freqcounter;
  }
    
/************************************
    send data for logging routine
************************************/
void sendlogdata()
  {
  Serial.print("Log:");
  Serial.print(period,0);
  Serial.print(",");
  Serial.print(frequency,3);
  Serial.print("\n");
  }
