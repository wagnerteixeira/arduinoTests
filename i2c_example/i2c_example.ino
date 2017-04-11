#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);
 
void setup()
{
 lcd.begin (16,2);
 lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.print("oi Moh !!");
  lcd.setCursor(0,1);
  lcd.print("Mozaum!!!");
  //delay(1000);
  //lcd.setBacklight(LOW);
  //while(1);
}
 
void loop()
{
  lcd.setBacklight(LOW);
  delay(1000);
  lcd.setBacklight(HIGH);
  delay(1000);
}
