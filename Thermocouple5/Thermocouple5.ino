
// ***** INCLUDES *****
#include "MAX31855.h"  // Local version
#include <Streaming.h>
#include <SoftwareSerial.h>
#include <serLCD.h>

// ***** PIN DEFINITIONS *****
// Current settings for Arduino Mega ADK
const  unsigned  char thermocoupleSO = 41;  // SO
const  unsigned  char thermocoupleCS = 39;  // CS
const  unsigned  char thermocoupleCLK = 37; // SCK

const  unsigned  char mA = 43;
const  unsigned  char mB = 45;
const  unsigned  char mC = 47;
const  unsigned  char mD = 49;

const  unsigned  char ctrlPin = 35;
const  unsigned  char btn1Pin = 33;

const  unsigned  char lcdpin = 31;
// ***** PIN DEFINITION END

// ***** Max number of channels *****
// Counts 0..maxcount-1
const int maxcount = 16;

int reg = 0;
int oldreg = 0;

double tempTC, tempCJC;
bool faultOpen, faultShortGND, faultShortVCC, x;
bool temp_unit = 0;  // 0 = Celsius, 1 = Fahrenheit

unsigned long time;
unsigned long now;
const long delaymillis = 500;
double templimit = 30.0;

//Relay parameters
bool alwayson;
bool onstatus = false;
bool checkedAll = false;
bool goodtemp = true;

// PINS: SCK-SCK-5, SS-CS-6, MISO-SO-7
MAX31855 temp(thermocoupleCLK, thermocoupleCS, thermocoupleSO);
serLCD lcd(lcdpin);

void writeNum(int num) {
   if ((num & (1<<0)) > 0) {
     digitalWrite(mA, HIGH);
   } else {
     digitalWrite(mA, LOW);
   }
   if ((num & (1<<1)) > 0) {
     digitalWrite(mB, HIGH);
   } else {
     digitalWrite(mB, LOW);
   }
   if ((num & (1<<2)) > 0) {
     digitalWrite(mC, HIGH);
   } else {
     digitalWrite(mC, LOW);
   }
   if ((num & (1<<3)) > 0) {
     digitalWrite(mD, HIGH);
   } else {
     digitalWrite(mD, LOW);
   }
}



// Print the temperature on the LCD
void showTemp(int num, double hot) {
    char numpad[2];
    sprintf(numpad, "%02d", num);
    lcd.clearLine(2);
    lcd << "T" << numpad << " = " << _FLOAT(hot,2) << " C";
}

// Print the temperature on the LCD
void showRelay() {
    char status[10];
    if (onstatus) {
        if (alwayson) {
           strcpy(status, "always-on");
        } else {
           strcpy(status, "on");
        }
    } else {
       strcpy(status, "OFF");
    }
    lcd.clearLine(1);
    lcd << "Relay: " << status;
}

void  setup()
{
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  lcd.setBrightness(20);
  pinMode(mA, OUTPUT);
  pinMode(mB, OUTPUT);
  pinMode(mC, OUTPUT);
  pinMode(mD, OUTPUT);
  digitalWrite(mA, LOW);
  digitalWrite(mB, LOW);
  digitalWrite(mC, LOW);
  digitalWrite(mD, LOW);

  pinMode(btn1Pin, INPUT);
  alwayson = digitalRead(btn1Pin);
  if (alwayson == HIGH) {
    onstatus = true;
  }
  showRelay();

  pinMode(ctrlPin, OUTPUT);
  digitalWrite(ctrlPin, onstatus);
  
  reg = 0;
  
  lcd.clear();
}

void  loop()
{
  time = millis();
  // Read the temperature
  x = temp.readMAX31855(&tempTC, &tempCJC, &faultOpen, &faultShortGND, &faultShortVCC, temp_unit);

  // Set the next channel number
  reg++;
  if (reg >= maxcount) {
    reg = 0;
    checkedAll = true;  // have we seen every channel at least once?
  }
  writeNum(reg);  // set new connection
  now = millis();  // set reference point for delay
  
  ///// do other stuff
  // send data
  // Append "T" in the front to signify thermocouple, thus we can distinquish it from other Arduinos. Ie. acts like an ID.
  Serial << "T," << oldreg << "," << tempTC << "," << tempCJC << "," << time << "," << faultOpen << faultShortGND << faultShortVCC << endl;
  showTemp(oldreg, tempTC);
  oldreg = reg;
  if (tempTC > templimit) {
       goodtemp = false; 
  }
  
  if ((checkedAll && goodtemp) || (alwayson)) {
       onstatus = true;
  } else {
       onstatus = false; 
  }
  
  bool btn1 = digitalRead(btn1Pin);
  if (btn1 != alwayson) {
     alwayson = btn1;
     if (alwayson) {
        onstatus = alwayson;
     } else {
        // start checking them all again
        onstatus = false;
        checkedAll = false;
        goodtemp = true;
        reg = 0;
     }
  }
  showRelay();
  digitalWrite(ctrlPin, onstatus);

  // wait until enought time elapsed    
  while ((millis() - now) < delaymillis) {
      // just wait....
  }

}
