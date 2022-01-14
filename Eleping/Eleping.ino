/*
 Name:		Eleping.ino
 Created:	1/12/2022 8:42:09 PM
 Author:	Silver Põld
*/

//#include <WiFiUdp.h>
//#include <WiFiServerSecureBearSSL.h>
//#include <WiFiServerSecure.h>
//#include <WiFiServer.h>
//#include <WiFiClientSecureBearSSL.h>
//#include <WiFiClientSecure.h>
//#include <WiFiClient.h>
//#include <ESP8266WiFiType.h>
//#include <ESP8266WiFiSTA.h>
//#include <ESP8266WiFiScan.h>
//#include <ESP8266WiFiMulti.h>
//#include <ESP8266WiFiGratuitous.h>
//#include <ESP8266WiFiGeneric.h>
//#include <ESP8266WiFiAP.h>
//#include <CertStoreBearSSL.h>
//#include <BearSSLHelpers.h>
//#include <ArduinoWiFiServer.h>
#include <ArduinoJson.h>
#include <ESP8266Wifi.h>
#include <ESP8266HTTPClient.h>
#include <TM1637Display.h>

// Define on-board LED pins
#define LED_built_in_ESP 2
#define LED_built_in_Node 16

// Define TM1637 7 Segment 4 digits connections pins
#define Digits_CLK 3
#define Digits_DIO 4

const char* ssid = "MeieKodu";
const char* password = "JaamaTaga";

// Create a display object of type TM1637Display
TM1637Display digits = TM1637Display(Digits_CLK, Digits_DIO);

// Digits - all segments ON
const uint8_t digits_AllON[] = { 0xff, 0xff, 0xff, 0xff };

// Digits - all segments OFF
const uint8_t digits_AllOFF[] = { 0x00, 0x00, 0x00, 0x00 };

// Digits: "   ELEPing"
uint8_t SEG_ELEPing[] = {
  SEG_D,                                          // _ // TODO: Replace '_' with space { 0x00 };
  SEG_D,                                          // _
  SEG_D,                                          // _
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,		  // E
  SEG_D | SEG_E | SEG_F,                          // L
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,          // E
  SEG_A | SEG_B | SEG_F | SEG_G,                  // P
  SEG_C,                                          // i
  SEG_C | SEG_E | SEG_G,                          // n
  SEG_A | SEG_B | SEG_F | SEG_G | SEG_C | SEG_D,  // g
};

// Digits: "Err"
uint8_t SEG_Err[] = {
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,		  // E
  SEG_E | SEG_G,                                  // r
  SEG_E | SEG_G,                                  // r
  0x00                                            // [space]
};

// the setup function runs once when you press reset or power the board
void setup() {
  // Set up LED-s
  pinMode(LED_built_in_ESP, OUTPUT);
  pinMode(LED_built_in_Node, OUTPUT);

  // Set up WiFi
  digitalWrite(LED_built_in_ESP, LOW);

  Serial.begin(115200);
  Serial.println();
  Serial.print("WiFi connecting to '");
  Serial.print(ssid);
  Serial.println("'");

  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    // Display scrolling "   ELEPing" on digits during connecting to WiFi
    digits.setSegments(SEG_ELEPing + i);
    if (i == 10)
      i = 0;
    else
      i++;

    delay(500);
    Serial.print(".");
  }

  digitalWrite(LED_built_in_ESP, HIGH);
  Serial.println();

  Serial.println("WiFi Connected successfully!");
  Serial.print("NodeMCU IP Address: ");
  Serial.println(WiFi.localIP());

  // Display nothing on digits
  digits.setSegments(digits_AllOFF);
}

// the loop function runs over and over again until power down or reset
void loop() {
  if (WiFi.status() != WL_CONNECTED)
  {
    digits.setSegments(digits_AllON);
    delay(500);
  }

  HTTPClient http;
  http.begin("https://dashboard.elering.ee/api/nps/price/EE/current");
  int httpCode = http.GET();

  if (httpCode == 200) {
    // Parsing
    const size_t bufferSize = JSON_OBJECT_SIZE(1) + 50;
    DynamicJsonDocument jsonDocument(bufferSize);
    DeserializationError error = deserializeJson(jsonDocument, http.getString());
    
    if (error) {
      digits.setSegments(SEG_Err);
      Serial.println("There was an error while deserializing");
    }
    else {
      JsonObject root = jsonDocument.as<JsonObject>();
      // Parameters
      int Success = root["success"]; // 1
      Serial.print("success: ");
      Serial.println(Success);

      long timestamp = root["data"]["timestamp"];
      float priceMWhFloat = root["data"]["price"];
      int priceMWh = round(priceMWhFloat);
      //int price = (int)priceFloat * 1000.0; // bar now = 12345
      //float foo = (float)price / 1000.0;   // foo now = 12.345 or something very close but that's how FP is and always has been.
      digits.showNumberDec(priceMWh);
      // Pause for a minute
      delay(60000);
    }
  }

  http.end(); //Close connection


  // TODO: Display rounded data/price on digits



  // Blink LED-s
  digitalWrite(LED_built_in_ESP, HIGH);
  digitalWrite(LED_built_in_Node, LOW);
  delay(1000);
  digitalWrite(LED_built_in_ESP, LOW);
  digitalWrite(LED_built_in_Node, HIGH);
  delay(1000);
}
