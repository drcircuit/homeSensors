#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "icons.h"
#include "cfg.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const long interval = 1000;   
unsigned long previous = 0;   
int count;
char data[400];
bool locked = true;


DHT dht(DHTPIN, DHTTYPE);

void drawTemp(float tmp, float hum, float idx){
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.clearDisplay();
  display.drawBitmap(100,2, PANDA, 16,16,WHITE);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  char data[10];
  sprintf(data, "%.2f", tmp);
  display.print(data);
  display.print((char)248);
  display.print("C\n");
  sprintf(data, "%.2f Hum", hum);
  display.println(data);
  sprintf(data, "%.2f", idx);
  display.setTextSize(1);
  display.print(data);
  display.print((char)248);
  display.print("C Idx\n");
  if(locked){
    display.print("Locked");
    display.drawBitmap(4, 50,  KEYICON, 16, 8, WHITE);
  } else {
    display.print("Unlocked");
    display.drawBitmap(4, 50,  ULKEYICON, 16, 8, WHITE);
  }
  
  display.display();
}
void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);
  char buff[100];
  EEPROM.get(0,buff);
  int tries = 0;
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(LOCK, OUTPUT);
  pinMode(KEY, INPUT);
  digitalWrite(LOCK, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  count = 0;
  Serial.begin(115200);
  Serial.println(F("DHTxx Test!"));
  Serial.println(buff);
  delay(4000);
  dht.begin();
  delay(10);
  Serial.println("Connecting to ");
  Serial.println(ssid); 
 
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED && tries < maxWifi) 
  {
    delay(500);
    Serial.print(".");
    tries++;
  }

  if(tries == maxWifi){
    Serial.println("NO WIFI :(");
  } else {
    Serial.println("");
    Serial.println("WiFi connected"); 
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
}
void lightUpRed(){
  digitalWrite(RED, HIGH);
  if(locked){
    digitalWrite(GREEN, LOW);  
  }  
}

void lightUpGreen(){
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, HIGH);
}

void unlock(){
  digitalWrite(LOCK, HIGH);
}
void lock(){
  digitalWrite(LOCK, LOW);
}
void loop() {
  // put your main code here, to run repeatedly:
 
  int button = digitalRead(KEY);

  if(button == HIGH){
    locked = false;    
    digitalWrite(GREEN, HIGH);
  } else {
    locked = true;
    digitalWrite(GREEN, LOW);
  }
  unsigned long current = millis();
  
  if(!locked){
    unlock();
  } else {
    lock();
  }
  if (current - previous >= interval) {
    previous = current;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
  
    if(isnan(h)||isnan(t)||isnan(f)){
      Serial.println("Failed to read sensor...");
      return;
    } 
  
    if(t > 19.0){    
      lightUpRed();
    } else {    
      lightUpGreen();
    }
    
    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);
  
    sprintf(data, "\n----------------||--SMIDGE SENSORS--||--------------\nHumidity:\t %.2f\nTemperature:\t %.2f °C \t %.2f °F\nHeat Index:\t %.2f °C\t %.2f °F\n----------------------------------------------------\n", h, t, f, hic, hif);
    Serial.println(data);
    drawTemp(t, h, hic);

  }
}
