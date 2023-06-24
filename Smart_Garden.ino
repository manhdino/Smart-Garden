#include <LiquidCrystal.h>
#include <WiFiClient.h>
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>    // tải thêm trong Sketch ở Arduino IDE
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6tvHbOv0-"
#define BLYNK_TEMPLATE_NAME "Smart Garden"
#define BLYNK_AUTH_TOKEN "ruOPsNkbbRstK-aoo5Dwj6d-N9RyL-oB"


char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "DCE-KTMT";
char pass[] = "ktmt-dce";
BlynkTimer timer;
//SDA 21
//SCL 22

#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE    DHT11     // DHT 11
#define SOILMOISTURE_PIN A0 // 36
#define RELAY_PIN 26
DHT dht(DHTPIN, DHTTYPE);
int h = 0;
float t = 0;
int water_pump_auto=0;
int water_pump_manual=0;

float readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
  }
  else {
    Serial.println(t);
    Blynk.virtualWrite(V0, t);
    return t;
  }
}

float readDHTHumidity() {
  int hum = analogRead(SOILMOISTURE_PIN);
  h = map(hum,0,4095,100,0);

  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  else {
    Serial.println(h);
    Blynk.virtualWrite(V1, h);
    return h;
  }
}

void sendSensor(){
   int hum =  readDHTHumidity();
   float temp =  readDHTTemperature();
   OLED(temp,hum);
}


void setup(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Debug console
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN,LOW);
  timer.setInterval(500L, sendSensor);
}
void Mode(){
  if(water_pump_auto == 1 && water_pump_manual == 0){ //auto
   if (h <=42 && t < 35){            
      digitalWrite(RELAY_PIN, HIGH);
    }else if ( h >42) {               
      digitalWrite(RELAY_PIN, LOW);
    }
  }
  else if(water_pump_manual == 1){ //manual
     digitalWrite(RELAY_PIN,HIGH);
  }
  else{
    digitalWrite(RELAY_PIN,LOW);
  }
}
BLYNK_WRITE(V2){          //đọc gtri nút từ app blynk
  water_pump_auto =  param.asInt();
  Serial.println(water_pump_auto);
}
BLYNK_WRITE(V3){          //đọc gtri nút từ app blynk
  water_pump_manual =  param.asInt();
  Serial.println(water_pump_manual);
}


void OLED(float t,int h){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.print("Hum:");
  display.print(h);
  display.println(F("%"));
   display.println();
  display.print("Temp:");
  display.println(t);
  display.display();
}
void loop(){
  Blynk.run();
  timer.run();
  Mode();
}