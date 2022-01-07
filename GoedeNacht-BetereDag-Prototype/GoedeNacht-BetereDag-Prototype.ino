#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "SoftwareSerial.h"


// Rotary Encoder Inputs
#define S2 3
#define S1 4
#define KEY 5

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16


int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;


//SoftwareSerial mySerial(6, 7);

void setup() {

  // Setup Serial Monitor
  Serial.begin(9600);

  // Set encoder pins as inputs
  pinMode(S2,INPUT);
  pinMode(S1,INPUT);
  pinMode(KEY, INPUT_PULLUP);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Read the initial state of CLK
  lastStateCLK = digitalRead(S2);
  
  display.display();
  delay(2000); // Pause for 2 seconds
  
  // Clear the buffer
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  int result = displayMenu();
  Serial.println(result);

  int amountOfReplays = (result * 60) / 20;
  Serial.println(amountOfReplays);
}

void loop() {
 
}

int displayMenu(){
 
  bool minutesSelected = false;
  while(minutesSelected == false){
     
    // Read the current state of CLK
    currentStateCLK = digitalRead(S2);

    // If last and current state of CLK are different, then pulse occurred
    // React to only 1 state change to avoid double count
    if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){

      // If the DT state is different than the CLK state then
      // the encoder is rotating CCW so decrement
      if (digitalRead(S1) != currentStateCLK) {
        counter -= 10;
        currentDir ="CCW";
      } else {
        // Encoder is rotating CW so increment
        counter += 10;
        currentDir ="CW";
      }

      Serial.print("Direction: ");
      Serial.print(currentDir);
      Serial.print(" | Counter: ");
      Serial.println(counter);
  }

  // Remember last CLK state
  lastStateCLK = currentStateCLK;

  // Read the button state
  int btnState = digitalRead(KEY);

  //If we detect LOW signal, button is pressed
  if (btnState == LOW) {
    //if 50ms have passed since last LOW pulse, it means that the
    //button has been pressed, released and pressed again
    if (millis() - lastButtonPress > 50) {
      Serial.println("Button pressed!");

      display.clearDisplay();
      display.setCursor(0,0);
      display.print("Slaap lekker!");
      display.display();
      
      return counter;
    }

    Serial.println(S1);
    
    // Remember last button press event
    lastButtonPress = millis();
  }

  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Hoeveel minuten?");
  display.setCursor(0,10);
  display.print(counter);
  display.display();
  
  // Put in a slight delay to help debounce the reading
  delay(1);
    
 }
}
