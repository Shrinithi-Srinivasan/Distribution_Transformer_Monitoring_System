#include <SoftwareSerial.h>
#include <dht.h>
#include "NewPing.h"
#include <LiquidCrystal_I2C.h>

// Pin definitions
#define DHT_PIN 8
#define TRIGGER_PIN 10
#define ECHO_PIN 9
#define ANALOG_IN_PIN A0

// Create SoftwareSerial object to communicate with NodeMCU
SoftwareSerial mySerial(5, 6);  // RX, TX
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

// DHT and ultrasonic sensor objects
dht DHT;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, 400); // 400 cm max distance

// Voltage sensor variables
float R1 = 30000.0;
float R2 = 7500.0;
float ref_voltage = 5.0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);  // Start software serial at 9600 baud rate for communication with NodeMCU
 lcd.init();
  lcd.clear();         
  lcd.backlight();
  DHT.read11(DHT_PIN); // Initialize DHT sensor
}

void loop() {
  // Collect sensor data
  DHT.read11(DHT_PIN);
  float temp = DHT.temperature;
  float humid = DHT.humidity;
  unsigned int distance = sonar.ping_cm();
  float current = readCurrent();
  float voltage = readVoltage();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("current = ");
  lcd.println(current);  
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,0);
  
  lcd.print("Volt= ");
  lcd.println(readVoltage(), 2);
  lcd.print(" V  ");
  // Short delay
  delay(2000);

  //LCD DISPLY
  lcd.clear();
  lcd.setCursor(0,0);
	lcd.print("Temp = ");
	lcd.print(temp);
	lcd.print(" ");
	lcd.print((char)176);//shows degrees character
	lcd.print("C  ");

  lcd.setCursor(0,1);
	lcd.print("Humidity = ");
	lcd.print(humid);
	lcd.println(" % ");
	lcd.println("");

	delay(2000);
//LCD LEVEL
lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("level = ");
	lcd.print(sonar.ping_cm());
	lcd.println(" cm    ");
	delay(2000);

  

  // Send sensor data to NodeMCU
  mySerial.print(temp);
  mySerial.print(",");
  mySerial.print(humid);
  mySerial.print(",");
  mySerial.print(distance);
  mySerial.print(",");
  mySerial.print(current);
  mySerial.print(",");
  mySerial.println(voltage);  // Send the data as a comma-separated string

  delay(2000);  // Send data every 2 seconds
}

float readCurrent() {
  float average = 0;
  for (int i = 0; i < 1000; i++) {
    average += (0.0264 * analogRead(A1) - 13.51) / 1000;
    delay(1);
  }
   
  return average;
}

float readVoltage() {
  int adcValue = analogRead(ANALOG_IN_PIN);
  float adcVoltage = (adcValue * ref_voltage) / 1024.0;
  return adcVoltage * (R1 + R2) / R2;

   
}
