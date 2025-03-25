#define BLYNK_TEMPLATE_ID "TMPL2dltCiNJb"
#define BLYNK_TEMPLATE_NAME "Sensor"

#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>



char auth[] = "N14v8b4V2mngMfSQI3PNS4yh27FTPvSI";  // Lo copias desde tu proyecto en la app
char ssid[] = "Miravalles";
char pass[] = "*M1rAv@l_L3$#";


#define FLOW_PIN 13
volatile int pulseCount = 0; 
float flowRate;  
unsigned long oldTime = 0;
void IRAM_ATTR pulseCounter() {  
    pulseCount++;  // Incrementa cada vez que el sensor detecta un pulso
}

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN,DHTTYPE);
float read_temp = 0;

#define WATER_TEMP_PIN  17 // ESP32 pin GPIO17 connected to DS18B20 sensor's DATA pin
OneWire oneWire(WATER_TEMP_PIN);
DallasTemperature DS18B20(&oneWire);
float read_water_temp = 0;

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


void setup()
{
  Blynk.begin(auth,ssid,pass);

  pinMode(FLOW_PIN, INPUT_PULLUP);
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  DS18B20.begin();
  dht.begin();
}


void loop(){ 
Blynk.run();

lcd.setCursor(0,0);
DS18B20.requestTemperatures();
read_water_temp = DS18B20.getTempCByIndex(0);
lcd.print("Water temp: ");
lcd.print(read_water_temp);
delay(2000);

lcd.setCursor(0,1);
read_temp = dht.readTemperature();
lcd.print("Temp: ");
lcd.print(read_temp);
delay(2000);
lcd.clear();

Blynk.virtualWrite(V0,read_temp);

  if (millis() - oldTime > 1000) {  // Actualización cada segundo
        detachInterrupt(FLOW_PIN);
        
        // Cálculo del caudal en L/min (según la hoja de datos del YF-S201)
        flowRate = (pulseCount / 7.5);  
        lcd.setCursor(0,0);
        lcd.print("Flujo: ");
        lcd.print(flowRate);
        lcd.print(" L/min");

        pulseCount = 0;  // Reiniciar contador
        oldTime = millis();
        
        attachInterrupt(digitalPinToInterrupt(FLOW_PIN), pulseCounter, FALLING);
    }
    delay(2000);
}



