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
double period = 0; // in µS
double frequency = 0; // in Hz
// timestamps for measuring
unsigned long timestamp1 = 0; // first timestamp
unsigned long timestamp2 = 0; // second timestamp
// action controlling
int actioncounter = 0; // counts for timing actions
// frequncy counter
int freqcounter = 0;
boolean highfrequency = true; // default we start with highfrequency measuring mode
double prescaler = 10; // prescaler factor

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
  // set cursor to measure mode indication
  lcd.setCursor(14, 1);
  if (highfrequency == true)
      {
      lcd.print("HF");
      }
    else
      {
      lcd.print("LF");
      }
      
  // measuring handling 
  // first reset counter
  freqcounter = 0;
  // different handling at high or low frequency
  if (highfrequency == true)
    {
    // high frequency measuring mode
    // activate interrupt for high frequency measuring
    attachInterrupt(1, hfcount, FALLING);
    // wait 100mS measuring cycle
    delay (100);
    // deactivate interrupt and stop measuring
    detachInterrupt(1);
    // calculate period from frequency measuring (period in µS)
    // measure cycle time 100000µS divided by counts
    period = 100000 / double(freqcounter);     
    }
  else
    {
    // low frequency measuring in normal mode    
    // wait 100mS measuring cycle
    delay (100);
    // calculate result of period measurement
    period = double(timestamp2 - timestamp1);
    }
  
  // calculate frequency from counter
  frequency = double(freqcounter) * 10;
  
  
  // calculate correct measurements with prescaler use
  frequency = frequency * prescaler;
  period = period / prescaler;
    
  // on lower frequencys, calculate frequency from period if bigger than 999µS
  if (period > 999)
    {
    // in this case, frequnency measuring will be overwritten by calculation
    frequency = 1000000 / period; // calculate the frequency from period
    }
    
  // counts loops for separate periodic actions
  ++actioncounter;
  // every 60 seconds action
  if (actioncounter > 600)
    {
    actioncounter = 0; // reset counter
    sendlogdata(); // send measuring data
    }
  
  // set cursor to second line and display results
  lcd.setCursor(0, 0);
  lcd.print(period,3);
  lcd.print("uS   ");
  lcd.setCursor(0, 1);
  lcd.print(frequency,3);
  lcd.print("Hz  ");
  
  // decide which measuring mode will we usefull
  // don't forget the prescaler 
  if (frequency / prescaler > 25000 && highfrequency == false)
    {
    // at frequencys above 25kHz we will use other measuring mode
    highfrequency = true; // set flag for high frequency measuring mode
    // deactivate interrupt for measuring
    detachInterrupt(1);    
    }
  if (frequency / prescaler < 20000 && highfrequency == true)
    {
    // at frequencys less 20kHz we will use other measuring mode
    highfrequency = false; // reset flag for high frequency measuring mode
    // activate interrupt for measuring
    attachInterrupt(1, measure, FALLING);    
    }
  
  // end of main loop
  }

/************************************
  normal measure interrupt routine
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

/*******************************************
  high frequency measure interrupt routine
*******************************************/    
void hfcount()
  {
  // frequency counter
  ++freqcounter;
  }

    
/************************************
    send data for logging routine
************************************/
void sendlogdata()
  {
  Serial.print("Log:");
  Serial.print(period,3);
  Serial.print(",");
  Serial.print(frequency,3);
  Serial.print("\n");
  }
