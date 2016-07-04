#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define pwmPin 7
int preheatSec = 120;
int prevVal = LOW;
long th, tl, h, l, ppm = 0;

void PWM_ISR() {
  long tt = millis();
  int val = digitalRead(pwmPin);
  
  if (val == HIGH) {    
    if (val != prevVal) {
      h = tt;
      tl = h - l;
      prevVal = val;
    }
  }  else {    
    if (val != prevVal) {
      l = tt;
      th = l - h;
      prevVal = val;
      ppm = 2000 * (th - 2) / (th + tl - 4);      
    }
  }
}

void setup()   {     
  Serial.begin(115200);
  pinMode(pwmPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(pwmPin), PWM_ISR, CHANGE);  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  display.setTextColor(WHITE);
}

void displayPreheating(int secLeft) {
  display.setTextSize(2);    
  display.println("PREHEATING");
  display.setTextSize(1);    
  display.println();    
  display.setTextSize(5);          
  display.print(" ");
  display.print(secLeft);  
  display.display();   
}

void displayPPM(long ppm) {
  display.setTextSize(2);    
  display.println("CO2    PPM");    
  display.setTextSize(1);    
  display.println();    
  display.setTextSize(5);    
  if (ppm < 1000) {
    display.print(" ");
  }
  display.print(ppm);  
  display.display();
  Serial.println(ppm);  
}

void loop() {    
    display.clearDisplay(); 
    display.setCursor(0,0); 
    if (preheatSec > 0) {
      displayPreheating(preheatSec);       
      preheatSec--;
    }
    else {      
      displayPPM(ppm);
    }
    delay(1000);  
}
