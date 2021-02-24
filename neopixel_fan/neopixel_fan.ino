#include <Adafruit_NeoPixel.h>
#include "Arduino.h"


// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library
// This sketch shows use of the "new" operator with Adafruit_NeoPixel.
// It's helpful if you don't know NeoPixel settings at compile time or
// just want to store this settings in EEPROM or a file on an SD card.





word VentPin = 3;
word FanInd = 5;
word LightInd=6;
word LightBttn=12;
word FanBttn=11;

int GPIO_fan=0;
int GPIO_light=0;
int lastLight=0;
 int fanState=1;
 int lightState=1;
 int lastFan=0;
 int gpioLastFan=0;
 int gpioLastLight=0;
 int gpioFanState=0;
 int gpioLightState=0;
// Which pin on the Arduino is connected to the NeoPixels?
int pin         =  7; // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
int numPixels   = 30; // Popular NeoPixel ring size

// NeoPixel color format & data rate. See the strandtest example for
// information on possible values.


int pixelFormat = NEO_GRB + NEO_KHZ800;

// Rather than declaring the whole NeoPixel object here, we just create
// a pointer for one, which we'll then allocate later...
Adafruit_NeoPixel *pixels;

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
 pinMode(VentPin, OUTPUT);
 pinMode(FanInd, OUTPUT);
 pinMode(LightInd, OUTPUT);
 pinMode(LightBttn, INPUT_PULLUP);
 pinMode(FanBttn, INPUT_PULLUP);


    Serial.begin(9600);
    while (!Serial);
    Serial.println("Debuging...");
 

  pwm25kHzBegin();
  // END of Trinket-specific code.
  
  // Right about here is where we could read 'pin', 'numPixels' and/or
  // 'pixelFormat' from EEPROM or a file on SD or whatever. This is a simple
  // example and doesn't do that -- those variables are just set to fixed
  // values at the top of this code -- but this is where it would happen.

  // Then create a new NeoPixel object dynamically with these values:
  pixels = new Adafruit_NeoPixel(numPixels, pin, pixelFormat);

  // Going forward from here, code works almost identically to any other
  // NeoPixel example, but instead of the dot operator on function calls
  // (e.g. pixels.begin()), we instead use pointer indirection (->) like so:
  pixels->begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  // You'll see more of this in the loop() function below.
  pixels->clear();
  pixels->show();
}

void loop() {
   // Set all pixel colors to 'off'


fanState= digitalRead(FanBttn);
lightState= digitalRead(LightBttn);
GPIO_fan=digitalRead(A5);
GPIO_light=digitalRead(A4);
if (GPIO_fan!=gpioLastFan){
  gpioFanState=1;
  gpioLastFan=GPIO_fan;
}
if (GPIO_light!=gpioLastLight){
  gpioLightState=1;
  gpioLastLight=GPIO_light;
}
Serial.println(GPIO_fan);
Serial.println(GPIO_light);
  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
if ((fanState==0 && lastFan==0)  || (lastFan==0 && gpioFanState==1 && GPIO_fan==1)){ //button is PRESSED but previous state was off so turn on
  delay (100);
    digitalWrite(FanInd, HIGH);
    pwmDuty(79); //max is 79
    delay(1000);
    lastFan=1;
    gpioFanState=0;
}
else if ((fanState==0 && lastFan==1) || (gpioFanState==1 && lastFan==1  && GPIO_fan==0)){ //button is PRESSED but previous state was on so turn off
  delay (100);
    digitalWrite(FanInd, LOW);
    pwmDuty(0);
    delay(1000);
    lastFan=0;
    gpioFanState=0;
  }
  


if ((lightState==0 && lastLight==0) || (GPIO_light==1 && lastLight==0  && gpioLightState==1)){
    delay (100);
    if (digitalRead(LightInd)==LOW){
      digitalWrite(LightInd, HIGH);
       for(int i=0; i<numPixels; i++) { // For each pixel...
          // pixels->Color() takes RGB values, from 0,0,0 up to 255,255,255
          // Here we're using a moderately bright green color:
          //pixels->setPixelColor(i, pixels->Color(0, 150, 0));
           pixels->setPixelColor(i, pixels->Color(255,255,255));
          //pixels->show();   // Send the updated pixel colors to the hardware.
          // delay(DELAYVAL); // Pause before next pass through loop
       }
       pixels->show();
       lastLight=1;
       delay(1000);
       gpioLightState=0;
}
}
    
else if ((lightState==0 && lastLight==1) || (GPIO_light==0 && lastLight==1 && gpioLightState==1)){
      digitalWrite(LightInd, LOW);
       for(int i=0; i<numPixels; i++) { // For each pixel...
          // pixels->Color() takes RGB values, from 0,0,0 up to 255,255,255
          // Here we're using a moderately bright green color:
          //pixels->setPixelColor(i, pixels->Color(0, 150, 0));
           pixels->setPixelColor(i, pixels->Color(0,0,0));
          //pixels->show();   // Send the updated pixel colors to the hardware.
          // delay(DELAYVAL); // Pause before next pass through loop
       }
       pixels->show();
       lastLight=0;
       delay(1000);
    }

  
}

  
 

void pwm25kHzBegin() {
  TCCR2A = 0;                               // TC2 Control Register A
  TCCR2B = 0;                               // TC2 Control Register B
  TIMSK2 = 0;                               // TC2 Interrupt Mask Register
  TIFR2 = 0;                                // TC2 Interrupt Flag Register
  TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);  // OC2B cleared/set on match when up/down counting, fast PWM
  TCCR2B |= (1 << WGM22) | (1 << CS21);     // prescaler 8
  OCR2A = 79;                               // TOP overflow value (Hz)
  OCR2B = 0;
  //Example: pwmDuty(79) //range = 0-79 = 1.25-100%)
}

void pwmDuty(byte ocrb) {
  OCR2B = ocrb;                             // PWM Width (duty)
}
