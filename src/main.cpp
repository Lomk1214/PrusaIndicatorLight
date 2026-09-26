#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "passwords.h"

#ifdef _AVR_
  #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN            4
#define NUMPIXELS      4

JsonDocument printer;

Adafruit_NeoPixel indicator(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

struct PrinterStatus {
  String status;
};

PrinterStatus printer1;
PrinterStatus printer2; 
PrinterStatus printer3;
PrinterStatus XL;

const uint32_t PRINTING = indicator.Color(255, 255, 0);
const uint32_t ERROR    = indicator.Color(255, 0, 0);
const uint32_t FINISHED = indicator.Color(0, 255, 0);
const uint32_t IDLE      = indicator.Color(0, 0, 0);
// const unit32_t starting = indicator.Color();


void setup() {
  Serial.begin(115200);
  indicator.begin(); // This initializes the NeoPixel library.
  indicator.clear(); // This sets all the pixels to 'off'
}

void loop() {
  printer1 = getPrinterStatus(ip1, API_KEY1);
  printer2 = getPrinterStatus(ip2, API_KEY2);
  printer3 = getPrinterStatus(ip3, API_KEY3);
  XL       = getPrinterStatus(ipXL, API_KEYXL);

  indicator.setPixelColor(0, getStatusColor(printer1.status));
  indicator.setPixelColor(1, getStatusColor(printer2.status));
  indicator.setPixelColor(2, getStatusColor(printer3.status));
  indicator.setPixelColor(3, getStatusColor(XL.status));

  delay(5000);
}


PrinterStatus getPrinterStatus(const char* ip, const char* apiKey) {
  PrinterStatus printState;
  HTTPClient http;
  http.begin("http://" + String(ip) + "api/v1/status");
  http.setAuthorization(userName, apiKey);
  int httpCode = http.GET();
  String response = http.getString();

  JsonDocument data;
  deserializeJson(data, response);
  printState.status = data["printer"]["state"].as<String>();

  return printState;
}

uint32_t getStatusColor(String status) {
  if(status = "PRINTING")
    return PRINTING;
  else if(status = "ERROR ")
    return ERROR;
  else if(status = "FINISHED")
    return FINISHED;
  else if(status = "IDLE")
    return IDLE;
  else
    return IDLE;

}