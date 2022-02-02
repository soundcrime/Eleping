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
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <Time.h>
//#include <U8glib.h>
//#include <U8x8lib.h>
//#include <MUIU8g2.h>
//#include <U8g2lib.h>
#include <ArduinoJson.h>
#include <ESP8266Wifi.h>
#include <ESP8266HTTPClient.h>
#include <TM1637Display.h>
#include <WiFiClientSecure.h>
#include <TimeLib.h>
//#include <Time.h>
#include <Wire.h>
//#include <SPI.h>
//#include <EleringCert.h>

// Define on-board LED pins
#define PIN_LED_built_in_ESP 2
#define PIN_LED_built_in_Node 16

// OLED display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//Adafruit_SSD1306 display(OLED_RESET);

const char* ssid = "";
const char* password = "";

const String hostName = "ELEPing-ESP8266"; //"ELEPing-ESP8266";
// It takes forever if I'm using 11N, so using WIFI_PHY_MODE_11G!
WiFiPhyMode wifiMode = WIFI_PHY_MODE_11G;

String elering_api_price_url = "https://dashboard.elering.ee/api/nps/price";
const char* elering_host = "dashboard.elering.ee";
const uint16_t https_port = 443;

// ISRG Root X1, valid until esmaspäev, 4. juuni 2035 13:04.38
const char cert_Elering_Root_CA[] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)CERT";
X509List cert(cert_Elering_Root_CA);

// Use WiFiClientSecure class to create TLS connection
WiFiClientSecure client;

// Create a display object of type TM1637Display
TM1637Display digits = TM1637Display(D6, D5); // D6 = CLK, D5 = DIO

// Digits - all segments ON
const uint8_t digits_AllON[] = { 0xff, 0xff, 0xff, 0xff };

// Digits: "   ELEPing   "
uint8_t SEG_ELEPing[] = {
  false,                                          //
  false,                                          //
  false,                                          //
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,		      // E
  SEG_D | SEG_E | SEG_F,                          // L
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,          // E
  SEG_A | SEG_B | SEG_F | SEG_G | SEG_E,          // P
  SEG_B,                                          // i'
  SEG_F | SEG_A | SEG_B,                          // n'
  SEG_A | SEG_B | SEG_F | SEG_G | SEG_C | SEG_D,  // g'
  false,
  false,
  false
};

// Digits: "Err "
uint8_t SEG_Err[] = {
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,  // E
  SEG_E | SEG_G,                          // r
  SEG_E | SEG_G,                          // r
  0x00                                    // [space]
};

// Digits: "Sync"
uint8_t SEG_Sync[] = {
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,  // S
  SEG_F | SEG_G | SEG_B | SEG_C | SEG_D,  // y'
  SEG_F | SEG_A | SEG_B,                  // n'
  SEG_A | SEG_F | SEG_G,                  // c'
};

// Timestamp
long timestamp = 0;

// the setup function runs once when you press reset or power the board
void setup() {
#if DEBUG
  Serial.begin(115200);
#endif

  // Set up LED-s
  pinMode(PIN_LED_built_in_ESP, OUTPUT);
  pinMode(PIN_LED_built_in_Node, OUTPUT);

  // Set up digits display (TM1637)
  digits.setBrightness(4);

  // Set up OLED display
  // set I2C pins [SDA = GPIO4 (D2), SCL = GPIO5 (D1)], default clock is 100kHz
  Wire.begin(D2, D1);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // TODO: REPLACE ADAFRUIT LOGO!!!!!

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  //display.display();
  //delay(5000); // Pause for 5 seconds

  // Clear the buffer
  display.clearDisplay();

  // Enable international characters
  display.cp437(true);

  //// Show the display buffer on the screen. You MUST call display() after
  //// drawing commands to make them visible on screen!
  //display.display();
  //delay(2000);

  //testScrollText();

  // Connect to WiFi network
  connectToWiFiAndApi();
}

void testScrollText() {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("ELEPing"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}

// the loop function runs over and over again until power down or reset
void loop() {
  // Switch on-board LED-s
  digitalWrite(PIN_LED_built_in_ESP, HIGH);
  digitalWrite(PIN_LED_built_in_Node, LOW);

  // If not connected to WiFi network - connect!
  if (WiFi.status() != WL_CONNECTED)
  {
    connectToWiFiAndApi();
  }

  // Retrieve current price
  // This must be run before getting all prices for current day
  // Because we will receive current date and time
  retrieveCurrentPrice();

  retrieveDayPrices();

  // Switch on-board LED-s
  digitalWrite(PIN_LED_built_in_ESP, LOW);
  digitalWrite(PIN_LED_built_in_Node, HIGH);

  // Pause for one minute
  //delay(60000);
  // Pause for five minutes
  for (int i = 0; i < 10; i++) {
    //testScrollText();
  }

  //delay(300000);
}

void connectToWiFiAndApi() {
  // Display nothing on digits
  digits.clear();
  displayText(hostName);

  // Scan networks
  //int numberOfNetworks = WiFi.scanNetworks();
  //Serial.print("Number of networks: ");
  //Serial.println(numberOfNetworks);

  //for (int i = 0; i < numberOfNetworks; i++) {
  //  Serial.print("Network name: ");
  //  Serial.println(WiFi.SSID(i));
  //  Serial.print("Signal strength: ");
  //  Serial.println(WiFi.RSSI(i));
  //  Serial.println("-----------------------");
  //}

  // Set up WiFi
  digitalWrite(PIN_LED_built_in_ESP, LOW);

  Serial.println();
  Serial.print("WiFi connecting to '");
  Serial.print(ssid);
  Serial.println("'");

  if (WiFi.SSID() != ssid) {
    Serial.println("WiFi begin.");

    //WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    WiFi.hostname(hostName);
    //WiFi.setHostname(hostName);

    WiFi.setPhyMode(wifiMode);
    WiFi.begin(ssid, password);
    wifi_station_set_auto_connect(true);
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());

    Serial.println("WiFi began.");
  }

  Serial.println("Before connection: ");
  WiFi.printDiag(Serial);

  displayText("Uhendan: " + String(ssid));
  Serial.println();
  Serial.println("Connecting");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    // Display scrolling "ELEPing" on digits during connecting to WiFi
    digits.setSegments(SEG_ELEPing + i);
    if (i == 9)
      i = 0;
    else
      i++;

    delay(500);
    //Serial.print(".");
  }

  Serial.println("After connection: ");
  WiFi.printDiag(Serial);

  displayText("Uhendatud: " + String(ssid));
  digits.clear();

  digitalWrite(PIN_LED_built_in_ESP, HIGH);
  Serial.println();

  Serial.println("WiFi Connected successfully!");
  Serial.print("NodeMCU IP Address: ");
  Serial.println(WiFi.localIP());

  connectToApi();
}

void connectToApi() {
  synchronizeNtpTime();

  Serial.print("Connecting to ");
  Serial.println(elering_host);

  //Serial.printf("Using certificate: %s\n", cert_Elering_Root_CA);
  client.setTrustAnchors(&cert);

  displayText("Uhendan API-ga...");
  if (!client.connect(elering_host, https_port)) {
    displayText("Proovin kohe uuesti...");
    Serial.println("Connection failed");
    /*char* dest;
    int code = client.getLastSSLError(dest, 1U);*/
    Serial.println("Try again in 30 seconds");
    delay(30000);
    connectToApi();
  }

  displayText("Uhendatud API-ga");
}

void synchronizeNtpTime() {
  displayText("Sunkroniseerin aega...");
  digits.setSegments(SEG_Sync);

  // Set time via NTP, as required for x.509 validation
  configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  struct tm timeinfo;

  while (now < 8 * 3600 * 2) {
    delay(500);
    //Serial.print(".");
    now = time(nullptr);
    gmtime_r(&now, &timeinfo);

    /*Serial.print("Now: ");
    Serial.println(asctime(&timeinfo));*/
  }

  //Serial.println("");
  //struct tm timeinfo;
  //gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

  digits.clear();
  display.clearDisplay();
}

void retrieveCurrentPrice() {
  digits.setSegments(SEG_Sync);
  displayText("Laen hetke hinda...");
  String elering_api_price_EE_current_url = elering_api_price_url + "/EE/current";
  Serial.print("Requesting URL: ");
  Serial.println(elering_api_price_EE_current_url);

  HTTPClient httpClient;
  httpClient.begin(client, elering_api_price_EE_current_url);
  int respCode = httpClient.GET();
  Serial.print("Response code: ");
  Serial.println(respCode);

  if (respCode == 200) {
    displayText("Hetke hind saadud!");

    String response = httpClient.getString();
    Serial.println(response);

    // Parsing
    DynamicJsonDocument doc(128);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      digits.setSegments(SEG_Err);
      Serial.println("There was an error while deserializing:");
      Serial.println(error.f_str());
    }
    else {
      bool success = doc["success"];
      Serial.print("success: ");
      Serial.println(success);

      if (!success) {
        httpClient.end();
        return;
      }

      timestamp = doc["data"][0]["timestamp"];
      Serial.print("timestamp: ");
      Serial.println(timestamp);
      float priceMWhFloat = doc["data"][0]["price"];
      Serial.print("priceMWhFloat: ");
      Serial.println(priceMWhFloat);
      int priceMWh = round(priceMWhFloat);
      Serial.print("priceMWh: ");
      Serial.println(priceMWh);

      // Show clock time
      digitsDisplayCurrentPriceTime();

      // Show energy price
      digits.showNumberDec(priceMWh, false);
    }
  }
  else {
    // TODO: Display error codes
    // HTTP error
    Serial.println("Error");
  }
  httpClient.end();
}

void retrieveDayPrices() {
  if (timestamp == 0)
    return;

  displayText("Laen tanaseid hindu...");

  String elering_api_price_day_url = String(elering_api_price_url + "?start=" + startDate() + "&end=" + endDate());

  Serial.print("Requesting URL: ");
  Serial.println(elering_api_price_day_url);

  HTTPClient httpClient;
  httpClient.begin(client, elering_api_price_day_url);
  int respCode = httpClient.GET();
  Serial.print("Response code: ");
  Serial.println(respCode);

  if (respCode == 200) {
    displayText("Tanased hinnad laetud!");

    String response = httpClient.getString();
    Serial.println(response);
    //displayTest(response);

    // Parsing
    //DynamicJsonDocument doc(6144);
    int maxBlock = ESP.getMaxFreeBlockSize() - 512;
    Serial.print("Max block size: ");
    Serial.println(maxBlock);
    // DynamicJsonDocument doc(maxBlock);
    StaticJsonDocument<6144> doc;
    StaticJsonDocument<64> filter;
    filter["data"]["ee"] = true;

    DeserializationError error = deserializeJson(doc, response, DeserializationOption::Filter(filter));
    // Print the result
    /*Serial.println("Pretty: ");
    serializeJsonPretty(doc, Serial);*/

    if (error) {
      digits.setSegments(SEG_Err);
      Serial.println("There was an error while deserializing:");
      Serial.println(error.f_str());
    }
    else {
      /*bool success = doc["success"];
      Serial.print("success: ");
      Serial.println(success);

      if (!success) {
        httpClient.end();
        return;
      }*/

      JsonObject data = doc["data"];

      for (int i = 0; i < 10; i++) {
        JsonArray eeItems = data["ee"].as<JsonArray>();
        int size = eeItems.size();

        // Get min and max price
        // for (JsonObject data_ee_item : data["ee"].as<JsonArray>()) {
        float minPrice = 1000000;
        float maxPrice = 0;
        for (int j = 0; j < size; j++) {
          JsonObject data_ee_item = eeItems[j];
          float data_price = data_ee_item["price"];
          if (data_price < minPrice)
            minPrice = data_price;
          if (data_price > maxPrice)
            maxPrice = data_price;
        }
        /*Serial.print("Min: ");
        Serial.println(minPrice);
        Serial.print("Max: ");
        Serial.println(maxPrice);*/

        // Scroll prices
        // for (JsonObject data_ee_item : data["ee"].as<JsonArray>()) {
        // TODO: Ära üldse hetkehinda eraldi tõmba vaid hakka selle[0] järgi praegust hinda näitama
        for (int j = 0; j < size; j++) {

          JsonObject data_ee_item = eeItems[j];
          long data_timestamp = data_ee_item["timestamp"]; // 1642284000, 1642287600, 1642291200, ...
          float data_price = data_ee_item["price"]; // 118.92, 110.06, 90.25, 87.84, 88.5, 89.39, 90.2, ...

          //if (j == 0) {
          //  int priceMWh = round(data_price);
          //  
          //  // Show clock time
          //  //digitsDisplayCurrentPriceTime();

          //  // Show energy price
          //  digits.showNumberDec(priceMWh, false);
          //}

          displayPrice(2000, data_timestamp, data_price, minPrice, maxPrice, j, size);
        }
        // Display scrolling "<<<"
        /*if (j == size - 1) {
          displayPriceEnd();
        }*/
        // Scroll back
        // TODO: Näita praegust kellaaega
        //digitsDisplayCurrentPriceTime();
        for (int j = size - 1; j >= 0; j--) {
          JsonObject data_ee_item = eeItems[j];
          long data_timestamp = data_ee_item["timestamp"]; // 1642284000, 1642287600, 1642291200, ...
          float data_price = data_ee_item["price"]; // 118.92, 110.06, 90.25, 87.84, 88.5, 89.39, 90.2, ...
          displayPrice(100, data_timestamp, data_price, minPrice, maxPrice, j, size);
        }
      }
    }
  }
  else {
    // TODO: Display error codes
    // HTTP error
    //digits.setSegments(SEG_Err);
    Serial.println("Error");
  }
  httpClient.end();
}

String startDate() {
  int day_utc = day(timestamp);
  int month_utc = month(timestamp);
  int year_utc = year(timestamp);
  int hour_utc = hour(timestamp);
  return String(year_utc) + "-" + formatDtInt(month_utc) + "-" + formatDtInt(day_utc) + "T" + formatDtInt(hour_utc) + "%3A00%3A00.000Z";
}

String endDate() {
  // Add 23 hours in seconds to current time
  long timestampEnd = timestamp + 82800;

  int day_utc = day(timestampEnd);
  int month_utc = month(timestampEnd);
  int year_utc = year(timestampEnd);
  int hour_utc = hour(timestampEnd);

  return String(year_utc) + "-" + formatDtInt(month_utc) + "-" + formatDtInt(day_utc) + "T" + formatDtInt(hour_utc) + "%3A59%3A59.999Z";
}

String formatDtInt(int number)
{
  if (number < 10)
  {
    return "0" + String(number);
  }
  return String(number);
}

String formatLocalHourInt(int number)
{
  if (number < 22)
    number = number + 2;
  else
    number = number - 22;

  return formatDtInt(number);
}

void digitsDisplayCurrentPriceTime() {
  int hour_utc = hour(timestamp);
  int hour_local;
  int minute_local = minute(timestamp);

  // To Estonian time (+2 h)
  if (hour_utc < 22)
    hour_local = hour_utc + 2;
  else
    hour_local = hour_utc - 22;

  // Create time format to display
  int displaytime = (hour_local * 100) + minute_local;

  // Display the current time in 24 hour format with leading zeros and a center colon enabled
  digits.showNumberDecEx(displaytime, 0b11100000, true);

  // Pause for five seconds
  delay(5000);
}

void displayText(String text) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  /*for each (char c in text)
  {
    if (c == 'Ü')
    {
      
    }
  }*/
  display.println(text);
  display.display();
}

void displayPrice(long delayTime, long timestamp, float price, float min, float max, int i, int size) {
  display.clearDisplay();

  // Draw price graph (left)
  int hPrice = round((SCREEN_HEIGHT / (max - min)) * (price - min));
  display.drawLine(0, SCREEN_HEIGHT, 0, SCREEN_HEIGHT - hPrice, SSD1306_WHITE);
  display.drawLine(1, SCREEN_HEIGHT, 1, SCREEN_HEIGHT - hPrice, SSD1306_WHITE);

  // Draw time graph (right)
  /*int hTime = round(((float)SCREEN_HEIGHT / (float)size) * (i + 1));
  display.drawLine(SCREEN_WIDTH - 1, SCREEN_HEIGHT, SCREEN_WIDTH - 1, SCREEN_HEIGHT - hTime, SSD1306_WHITE);
  display.drawLine(SCREEN_WIDTH - 2, SCREEN_HEIGHT, SCREEN_WIDTH - 2, SCREEN_HEIGHT - hTime, SSD1306_WHITE);*/

  // Draw time scrollbar background (right)
  /*for (int i = 1; i <= SCREEN_HEIGHT; i++) {
    if (i % 2 != 0)
      display.writePixel(SCREEN_WIDTH - 1, i, SSD1306_WHITE);
    else
      display.writePixel(SCREEN_WIDTH - 2, i, SSD1306_WHITE);
  }*/

  // Draw time scrollbar (right)
  int scrollbarHeight = 6;
  // From bottom to top
  /*int x1Time = SCREEN_HEIGHT - round((((float)SCREEN_HEIGHT - scrollbarHeight) / (float)size) * (i + 1)) - scrollbarHeight;
  int x2Time = x1Time + scrollbarHeight;*/
  // From top to bottom
  int x2Time = round((((float)SCREEN_HEIGHT - scrollbarHeight) / (float)size) * (i + 1));
  int x1Time = x2Time + scrollbarHeight;
  display.drawLine(SCREEN_WIDTH - 1, x2Time, SCREEN_WIDTH - 1, x1Time, SSD1306_WHITE);
  display.drawLine(SCREEN_WIDTH - 2, x2Time, SCREEN_WIDTH - 2, x1Time, SSD1306_WHITE);

  // Set up text
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);

  // Display time
  int h = hour(timestamp);
  String displaytime = String(formatLocalHourInt(h)) + ":00";

  display.setCursor(34, 0);
  display.println(displaytime);
  
  // Display price
  String priceS = String(price);
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;
  display.getTextBounds(priceS, 0, 0, &x1, &y1, &width, &height);
  display.setCursor(((SCREEN_WIDTH - width) / 2) - 11, 18);
  display.print(priceS);

  // Display EUR/MWh
  int x = display.getCursorX() + 4;
  display.setCursor(x, 17);
  display.setTextSize(1);
  display.print("EUR");
  display.setCursor(x, 25);
  display.print("MWh");

  display.display();
  delay(delayTime);
}

void displayPriceEnd() {
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(70, 7);
  display.println("<<<");
  display.display();
  display.startscrollleft(0, 7);
  delay(2000);
  display.stopscroll();
}

void displayS(String text) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(text);
  display.display();
}

void displayTest(String text) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(32, 0);
  display.println(text);
  display.display();
  display.startscrollleft(0, 1000);
  delay(20000);
}