//Include libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);


// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setup(void)
{
  Serial.begin(9600); //Begin serial communication
  Serial.println("Arduino Digital Temperature // Serial Monitor Version"); //Print a message
  sensors.begin();
  lcd.begin (16,2);
  lcd.setBacklight(HIGH);
  //lcd.setCursor(0,0);
  //lcd.print("oi Moh !!");
  //lcd.setCursor(0,1);
  //lcd.print("Mozaum!!!");
  //delay(1000);
  //lcd.setBacklight(LOW);
  //while(1);
}

void loop(void)
{ 
  // Send the command to get temperatures
  sensors.requestTemperatures();  
  Serial.print("Temperature is: ");
  Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  lcd.setCursor(0,0);
  lcd.print("Temp is: ");
  lcd.print(sensors.getTempCByIndex(0));
  //Update value every 1 sec.
  delay(1000);
}

