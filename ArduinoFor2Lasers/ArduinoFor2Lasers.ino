/*  ArduinoFor2Lasers.ino rev 5/30/2020 Tom
* Monitor two inputs from two Laser machines Lily and Daniel at Technology for Humankind
* One input indicates the laser is firing. The other input indicates the lid is raised
* Output to a set of Neopixel Lights the status of the laser according to the inputs.
* If the laser is on the set of lights for that machine will be BLUE. 
* If the laser is off and the lid is down the set of lights for that machine will be GREEN. 
* If the laser is on and the lid is raised the set of lights for that machine will be RED. 
* If the Green stays on for over 1 Minute the GREEN lights will begin FLASHING. 
* If the FLASHING continues for over 5 minutes an alarm output will sound a bell
*/
#define Debug 0
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN            6
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      10
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const long LaserTimeout = 60000; // One Minute timeout interval
const long AlarmTimeout = 300000; // Five Minute Alarm timeout interval
int bellCount;
int delayval = 50; // delay for half a second
int avg0 = 0;
int avgLaser0 = 0;
int avgLaser1 = 0;
float avg1;
bool isOpen0 = true;
bool isOpen1 = true;
bool lightOff0 = 0; 
bool lightOff1 = 0; 
bool Blink0 = 0;
bool Blink1 = 0;

unsigned long endTime0 = 0;        // will store last time LED not green
unsigned long endTime1 = 0;        // will store last time LED not green
unsigned long endTimeBell = 0;        // will store last time LED not green

void setup() {
  pinMode(A0, INPUT_PULLUP);  // laser power 0
  pinMode(A1, INPUT_PULLUP);  // laser power 1
  pinMode(A2, INPUT_PULLUP);  
  pinMode(2, INPUT_PULLUP);  // lid open 0
  pinMode(3, INPUT_PULLUP);  // lid open 1
  pinMode(4, INPUT_PULLUP);  
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);    // builtin LED
  Serial.begin(9600);
  Serial.println("Hello");
  strip.begin();
  strip.show();
}

void loop() {
  unsigned long currentMillis = millis();
  // Photocell 0 ---------------------
  //if(Debug)Serial.println(analogRead(A0));
  //if(Debug){Serial.print(avgLaser0,DEC);Serial.print(" . ");}
  int valLaser0 = (analogRead(A0) >= 40 ? 1000 : 0);
  avgLaser0 = ((19*avgLaser0) + valLaser0) / 20;
  lightOff0 = (avgLaser0 < 980);

  // Photocell 1 ---------------------
  //if(Debug)Serial.println(analogRead(A1));
  if(Debug){Serial.println(avgLaser1,DEC);Serial.print(" . ");}
  int valLaser1 = (analogRead(A1) >= 40 ? 1000 : 0);
  avgLaser1 = ((19*avgLaser1) + valLaser1) / 20;
  lightOff1 = (avgLaser1 < 980);

  //Tilt Sensor 0 --------------
  //if(Debug){Serial.print(avg0,DEC);Serial.print(" . ");}
  int val0 = digitalRead(2)*1000;
  avg0 = ((9*avg0)+val0)/10;
  isOpen0 = (avg0 > 100);
  
  //Tilt Sensor 1 --------------
  //if(Debug){Serial.print(avg1,DEC);Serial.print(" . ");}
  int val1 = digitalRead(3)*1000;
  avg1 = ((9*avg1)+val1)/10;
  isOpen1 = (avg1 > 100);

  //Laser0 Timoout Flash -------------------
  //if(Debug){Serial.print(lightOff0,DEC);Serial.print(" ");}
  if(lightOff0 || isOpen0){
    endTime0 = millis() + LaserTimeout;
    endTimeBell = millis() + AlarmTimeout;
    Blink0 = 0;
  }else if(currentMillis > endTime0){Blink0 = !Blink0;}
  
  //Laser1 Timoout Flash -------------------
  if(Debug){Serial.print(lightOff1,DEC);Serial.print(" ");}
  if(lightOff1 || isOpen1){
    endTime1 = millis() + LaserTimeout;
    endTimeBell = millis() + AlarmTimeout;
    Blink1 = 0;
  }else if(currentMillis > endTime1){Blink1 = !Blink1;}

  //Alarm Timeout --------------
  if(currentMillis > endTimeBell){
    bellCount = (bellCount + 1) % 10;
    digitalWrite(13,HIGH);
    digitalWrite(12,(bellCount == 2 ? 1 : 0));
    } else {
      digitalWrite(13,LOW);
      digitalWrite(12,LOW);
    }
  
  delay(120);  // loop delay
  
  //if(Debug)Serial.print(!isOpen1*8 + !lightOff1*4 + !isOpen0*2 + !lightOff0,BIN);
  if(Debug)Serial.println(".");
  if(!Debug)Serial.write(!isOpen1*8 + !lightOff1*4 + !isOpen0*2 + !lightOff0);
  int Brightness = 128;
  int RED0 = !lightOff0 * isOpen0 * Brightness;
  int GRN0 = !lightOff0 * !isOpen0 * !Blink0 * Brightness;
  int BLU0 = lightOff0 * Brightness;
  int RED1 = !lightOff1 * isOpen1 * Brightness;
  int GRN1 = !lightOff1 * !isOpen1 * !Blink1 * Brightness;
  int BLU1 = lightOff1 * Brightness;
  colorWipe(strip.Color(RED0, GRN0, BLU0),strip.Color(RED1, GRN1, BLU1), 30);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c,uint32_t d, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels()/2; i++) {
      strip.setPixelColor(i, c);
      strip.setPixelColor(i+5, d);
  }
  delay(wait);
  strip.show();
 }
