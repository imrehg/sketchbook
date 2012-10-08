
// ***** INCLUDES *****
#include "MAX31855.h"  // Local version
#include <SoftwareSerial.h>
#include <serLCD.h>


// ***** PIN DEFINITIONS *****
// const  unsigned  char thermocoupleSO = 6;
// const  unsigned  char thermocoupleCS = 5;
// const  unsigned  char thermocoupleCLK = 4;
const  unsigned  char thermocoupleSO = 41;  // SO
const  unsigned  char thermocoupleCS = 43;  // CS
const  unsigned  char thermocoupleCLK = 45; // SCK

const  unsigned  char mEN = 46;
const  unsigned  char mA0 = 47;
const  unsigned  char mA1 = 48;
const  unsigned  char mA2 = 49;

int reg = 0;

const int lcdpin = 35;

double tempTC, tempCJC;
bool faultOpen, faultShortGND, faultShortVCC, x;
bool temp_unit = 0;  // 0 = Celsius, 1 = Fahrenheit

// MAX31855  MAX31855(thermocoupleSO, thermocoupleCS, thermocoupleCLK);

// SCK-SCK-5, SS-CS-6, MISO-SO-7
MAX31855 temp(thermocoupleCLK, thermocoupleCS, thermocoupleSO);
serLCD lcd(lcdpin);

double  cold, hot;
unsigned long time;

void writeNum(int num) {
   if ((num & (1<<0)) > 0) {
     digitalWrite(mA0, HIGH);
   } else {
     digitalWrite(mA0, LOW);
   }
   if ((num & (1<<1)) > 0) {
     digitalWrite(mA1, HIGH);
   } else {
     digitalWrite(mA1, LOW);
   }
   if ((num & (1<<2)) > 0) {
     digitalWrite(mA2, HIGH);
   } else {
     digitalWrite(mA2, LOW);
   }
}


void  setup()
{
  Serial.begin(115200);
//  lcd.setBrightness(25);
  pinMode(mEN, OUTPUT);
  pinMode(mA0, OUTPUT);
  pinMode(mA1, OUTPUT);
  pinMode(mA2, OUTPUT);
  digitalWrite(mEN, HIGH);
  digitalWrite(mA0, LOW);
  digitalWrite(mA1, LOW);
  digitalWrite(mA2, LOW);

  reg = 0;
}

void  loop()
{
  
  time = millis();
//  // Retrieve thermocouple temperature in Degree Celsius
//  hot = MAX31855.readThermocouple(CELSIUS);  
//  // Retrieve cold junction temperature in Degree Celsius
//  cold = MAX31855.readJunction(CELSIUS);
//  Serial.print(hot);
//  Serial.print(",");
//  Serial.print(cold);
//  Serial.print(",");
//  Serial.print(time);
//  Serial.print(",");
//  Serial.print(reg);
//  Serial.println();
    x = temp.readMAX31855(&tempTC, &tempCJC, &faultOpen, &faultShortGND, &faultShortVCC, temp_unit);
//    Serial.print(tempTC);
//    Serial.print("\t");
//    Serial.print(tempCJC);
//    Serial.print("\t");
//    Serial.print(faultOpen);
//    Serial.print(faultShortGND);
//    Serial.println(faultShortVCC); 
    tempTC = 111;

    Serial.print(tempTC);
    Serial.print(",");
    Serial.print(tempCJC);
    Serial.print(",");
    Serial.print(time);
    Serial.print(",");
    Serial.print(reg);
    Serial.print(",");
    Serial.print(faultOpen);
    Serial.print(faultShortGND);
    Serial.println(faultShortVCC);

  lcd.clear();
  lcd.selectLine(1);
  lcd.print("Hot :");
  lcd.print(tempTC);
  lcd.print(" C");
  lcd.selectLine(2);
  lcd.print("Cold: ");
  lcd.print(tempCJC);
  lcd.print(" C");
  reg++;
  if (reg >= 2) {
    reg = 0;
  }
  writeNum(reg);
  delay(75);
}
