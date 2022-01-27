#include <SPI.h>
#include <Wire.h>
#include <Servo.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

# define Start_Byte 0x7E
# define Version_Byte 0xFF
# define Command_Length 0x06
# define End_Byte 0xEF
# define Acknowledge 0x00 //Returns info with command 0x41 [0x01: info, 0x00: no info]

# define ACTIVATED LOW

//Button inputs
#define buttonPlus 13
#define buttonMinus 11
#define buttonOk 12

#define vibratePin 2

// Knoppen
const int BUTTON1 = 13;//Initialize Pin12 with Button +
const int BUTTON2 = 11;//Initialize Pin11 with Button enter
const int BUTTON3 = 12; //Initialize Pin7 with Button -

int BUTTONstate1 = 0; // To read the button1 state
int BUTTONstate2 = 0;// To read the button2 state
int BUTTONstate3 = 0;// To read the button3 state

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;

int pos = 0;

//DfPlayer
SoftwareSerial mySoftwareSerial(7, 6); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Servo myservo; 

void setup() {
    
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  myservo.attach(3);

  initPinModes();
  initDFPlayer();
  initOled();
  
  int result = displayMenu();
  Serial.println(result);
  
  int amountOfReplays = (result * 60) / 20;
  Serial.println(amountOfReplays);

  playSleepingExercises(amountOfReplays);
  
}

void playSleepingExercises(int amountOfReplays){

  myDFPlayer.volume(30);
    
  for(int i = 0; i <= amountOfReplays; i++){
    myDFPlayer.play(1);
    Serial.println(i);

    delay(1000);
    
    digitalWrite(vibratePin, HIGH);
    for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
       myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    delay(3000);
    
    digitalWrite(vibratePin, LOW);
    delay(7000);
    
    digitalWrite(vibratePin, HIGH);
    for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    delay(8000);
  }
}

void initDFPlayer(){
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
}

void initOled(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
    }

    display.display();
    delay(2000);

     // Clear the buffer
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
}

void initPinModes(){
  pinMode(BUTTON1, INPUT);//Define Button1 as input pin
  pinMode(BUTTON2, INPUT);//Define Button2 as input pin
  pinMode(BUTTON3, INPUT);//Define Button3 as input pin
  pinMode(vibratePin, OUTPUT);
}

void loop() {
  
  
}

int displayMenu(){

  bool minutesSelected = false;


  while(minutesSelected == false){
    BUTTONstate1 = digitalRead(BUTTON1);//Read button1 state
    if(BUTTONstate1 == 1){
      Serial.println("Yes! 1");
      counter -= 10;
      delay(1000);
    }    
    BUTTONstate2 = digitalRead(BUTTON2);//Read button1 state
      if(BUTTONstate2 == 1){
      Serial.println("Yes! 2");

      display.clearDisplay();
      display.setCursor(0,0);
      display.print("Slaap lekker!");
      display.display();
      
      return counter;
    }
    BUTTONstate3 = digitalRead(BUTTON3);//Read button1 state
      if(BUTTONstate3 == 1){
      Serial.println("Yes! 3");
      counter += 10;
      delay(1000);
    }
    
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Hoeveel minuten?");
  display.setCursor(0,10);
  display.print(counter);
  display.display();
  }

 
  
  // Put in a slight delay to help debounce the reading
  delay(1);
 }

void playFirst()
{
  execute_CMD(0x3F, 0, 0);
  delay(500);
  setVolume(20);
  delay(500);
  execute_CMD(0x11,0,1); 
  delay(500);
}

void setVolume(int volume)
{
  execute_CMD(0x06, 0, volume); // Set the volume (0x00~0x30)
  delay(2000);
}


void execute_CMD(byte CMD, byte Par1, byte Par2)
// Excecute the command and parameters
{
// Calculate the checksum (2 bytes)
word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
// Build the command line
byte Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte};
//Send the command line to the module
for (byte k=0; k<10; k++)
{
mySoftwareSerial.write( Command_line[k]);
}
}
