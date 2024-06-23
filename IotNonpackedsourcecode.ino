#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <DHT.h> // Include the DHT library

#define DHT11_PIN 8 // Define DHT11_PIN here

LiquidCrystal lcd(10, 9, 5, 4, 3, 2);
DHT dht(DHT11_PIN, DHT11); // Initialize DHT object with pin and type

float t = 0;
float h = 0;

#define LDR_PIN A1
#define MQ3_PIN A0

char data = 0;
int ldr_read = 0;
int gas_read = 0;

String apiKey = "[YOUR_THINGSPEAK_API_KEY]"; // Replace with your ThingSpeak API Key
SoftwareSerial ser(12, 13); // RX, TX

void setup() {
  ser.begin(9600);
  lcd.begin(16, 2);

  pinMode(LDR_PIN, INPUT);
  pinMode(MQ3_PIN, INPUT);

  lcd.setCursor(1, 0);
  lcd.print("*SmarT FooD*");
  lcd.setCursor(3, 1);
  lcd.print("MonitorinG");
  Serial.begin(9600);

  ser.println("AT");
  delay(1000);
  ser.println("AT+GMR");
  delay(1000);
  ser.println("AT+CWMODE=3");
  delay(1000);
  ser.println("AT+RST");
  delay(5000);
  ser.println("AT+CIPMUX=1");
  delay(1000);
  String cmd = "AT+CWJAP=\"[vamshi]\",\"[raoraorao]\"";
  ser.println(cmd);
  delay(1000);
  ser.println("AT+CIFSR");
  delay(1000);
}

void loop() {
  lcd.clear();
  ldr_read = analogRead(LDR_PIN);
  lcd.setCursor(0, 0);
  lcd.print("LDR-");
  lcd.setCursor(4, 0);
  lcd.print(ldr_read);

  lcd.setCursor(0, 1);
  lcd.print("MQ3-");
  gas_read = analogRead(MQ3_PIN);
  lcd.setCursor(4, 1);
  lcd.print(gas_read);

  t = dht.readTemperature(); // Read temperature
  lcd.setCursor(8, 0);
  lcd.print("Tem-");
  lcd.setCursor(12, 0);
  lcd.print(t);
  Serial.println(t);

  h = dht.readHumidity(); // Read humidity
  lcd.setCursor(8, 1);
  lcd.print("Hum-");
  lcd.setCursor(12, 1);
  lcd.print(h);
  Serial.println(h);

  esp_8266();
}

void esp_8266() {
  String cmd = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80";
  ser.println(cmd);
  Serial.println(cmd);
  if (ser.find("Error")) {
    Serial.println("AT+CIPSTART error");
    return;
  }

  String postStr = "api_key=" + apiKey;
  postStr += "&field1=" + String(t);
  postStr += "&field2=" + String(h);
  postStr += "&field3=" + String(ldr_read);
  postStr += "&field4=" + String(gas_read);

  cmd = "AT+CIPSEND=";
  cmd += String(postStr.length());
  ser.println(cmd);
  Serial.println(cmd);
  delay(1000);
  ser.println(postStr);
  Serial.println(postStr);

  delay(3000);
}