/*
  Program to Read a Temp and Humidity Sensor and Display Values on an LCD
*/
#include <Wire.h>
#include <time.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27, change if LCD I2C address is different
Adafruit_BME280 bme;                 // BME Sensor


/*
Equates pins to names
*/
int moistureSensor = A3;  //Pin A3 (analog pin 3)
int hallEffectSensor = 7; //Pin 7 for Hall Effect Sensor Reading
int solenoid = 5;         //Pin 5 for Solenoid Control
int speaker = 6;          //Pin 4 Speaker Control




/* 
Moisture Sensor Variables
*/
int moistureVal = 0;


/*
Variables for water timing
*/
int waterTimer = 0;


void setup() {
  
  /*
  Serial Monitor 
  */
  Serial.begin(9600);


  /*
  Input and Output Pin Enables
  */
  pinMode(hallEffectSensor, INPUT);  //Pin 7 is input for Hall Effect Sensor
  pinMode(moistureSensor, INPUT); //Pin A3 is input for Moisture Sensor

  pinMode(solenoid, OUTPUT); //Pin 5 = Output for Solenoid Control
  pinMode(speaker, OUTPUT); //Pin 6 = Output for Speaker


  /*
  LCD initialization
  */
  lcd.init();     // initialize the lcd
  lcd.backlight();  // Turn on the LCD screen backlight


  /*
  Enable Interrupts
  */
  cli();
  PCICR |= 0b00000100;    // turn on port D
  PCMSK2 |= 0b10000000; //Enable interrupts on pin 7
  sei();


  /*
  Check if the BME sensor is detected
  */
  unsigned status;
  status = bme.begin(0x76); //Change this value for a different BME address
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor");
  }

}


/*
Function to generate a 1khz square wave, beeping the speaker
*/
void playSpeaker() {
  //1kHz square wave
  for (int i = 0; i < 128; i++) {
    digitalWrite(speaker, HIGH);
    delayMicroseconds(500);
    digitalWrite(speaker, LOW);
    delayMicroseconds(500);
  }

}


/*
Function to print the "Water me" to the LCD when water is low
*/
void waterMe(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Please give me");  
  lcd.setCursor(0,1);
  lcd.print("water, am thirst");
}


/*
Function to open and close the solenoid, holding open for x time
*/
void waterPlant() {
  if (moistureVal >= 430){
    digitalWrite(solenoid,HIGH);
    delay(10000);                 //Time in ms to hold the solenoid open
    digitalWrite(solenoid,LOW);
  }


}


/*

void rotaryEncoder(){
  //I have no Idea what is going on with this.....
}

*/



/*
ISR to watch Pin7 (Hall Effect Sensor) and activate when the pin changes

ISR will only execute any code when the pin is low, meaning that the water level is low, exiting when changed back to high
*/

void speakerInterrupt(){
  
  while(digitalRead(hallEffectSensor) == LOW){
    waterMe();      //Send a message to the LCD
    delay(3000);
    playSpeaker(); 
  }
  return;
}


void loop() {

  /*
  Print Measured Data to LCD
  */
  float tempF = ((bme.readTemperature() * 9 / 5) + 22);  //Read in the temperature
  float humidity = bme.readHumidity();                   //Read in the humidity
  
  lcd.clear();

  lcd.setCursor(0, 0);      //Print the Temperature (in F) on the top line
  lcd.print("Temp : ");
  lcd.print(tempF, 2);
  lcd.print("F");

  lcd.setCursor(0, 1);      //Print the Humidity on the bottome line
  lcd.print("Humidity: ");
  lcd.print(humidity, 2);
  lcd.print("%");
  
  delay(3000); //Hold LCD display for 3 seconds

  if (digitalRead(hallEffectSensor) == LOW) {
    speakerInterrupt();
  }
  
  /*
  Print the moisture value from the capacitive moisture sensor
  Displaying if the soil is Dry, Wet, or "Water"
  */
  moistureVal = analogRead(moistureSensor);

  if (moistureVal >= 430){
    lcd.clear();
    lcd.setCursor(0, 0);          //Set LCD cursor back to top left
    lcd.print("Soil == Dry!");    //Print the level of "moisture" in the soil
    lcd.setCursor(0, 1);
    lcd.print("Moisture: ");
    lcd.print(moistureVal);       //Print the moisture value (in decimal)    
  } else if (moistureVal < 430 & moistureVal >=350){
    lcd.clear();
    lcd.setCursor(0, 0);          //Set LCD cursor back to top left
    lcd.print("Soil == Wet!");    //Print the level of "moisture" in the soil
    lcd.setCursor(0, 1);
    lcd.print("Moisture: ");
    lcd.print(moistureVal);       //Print the moisture value (in decimal)    
  } else if (moistureVal <350 & moistureVal >= 260) {
    lcd.clear();
    lcd.setCursor(0, 0);          //Set LCD cursor back to top left
    lcd.print("Soil == WATER!");  //Print the level of "moisture" in the soil
    lcd.setCursor(0, 1);
    lcd.print("Moisture: ");
    lcd.print(moistureVal);       //Print the moisture value (in decimal)    
  } else{
    lcd.clear();
    lcd.setCursor(0, 0);          //Set LCD cursor back to top left
    lcd.print("Soil == Fucked");  //Print the level of "moisture" in the soil
    lcd.print("Moisture: ");
    lcd.print(moistureVal);       //Print the moisture value (in decimal)    
  }
    
  delay(3000); //Hold LCD display for 3 seconds



  if (digitalRead(hallEffectSensor) == LOW) {
    speakerInterrupt();
  }


  /*
  Water the plant every 5 iterations of code
  */
  waterTimer += 1;
  //If the timer ...... open the solenoid?
  if (waterTimer == 5) {
    waterPlant();
    waterTimer = 0;
  }
  


}
