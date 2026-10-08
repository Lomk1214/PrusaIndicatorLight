#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <passwords.h>


#ifdef _AVR_
  #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN            4
#define NUMPIXELS      4
bool canConnect;
JsonDocument printer;

Adafruit_NeoPixel indicator(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

struct PrinterStatus { 
  String status;
};

PrinterStatus printer1;
PrinterStatus printer2; 
PrinterStatus printer3;
PrinterStatus XL;

const uint32_t RED = indicator.Color(255, 0, 0);   // Red
const uint32_t YELLOW = indicator.Color(255, 150, 0); // Yellow
const uint32_t GREEN = indicator.Color(0, 255, 0);   // Green

bool IPAddresses;

uint32_t setUps[] = {
    RED, YELLOW, GREEN
};

const uint32_t PRINTING = indicator.Color(255, 150, 0); // Yellow
const uint32_t ERROR    = indicator.Color(255, 0, 0);   // Red
const uint32_t FINISHED = indicator.Color(0, 255, 0);   // Green
const uint32_t IDLE     = indicator.Color(0, 0, 0);    // Off
const uint32_t PRINTBUSY     = indicator.Color(0, 0, 255);    // Blue
const uint32_t ATTENTION     = indicator.Color(250, 40, 0);    // Orange



PrinterStatus getPrinterStatus(const char* ip, const char* apiKey) {

  PrinterStatus printState;
  HTTPClient http;
  String url = "http://" + String(ip) + "/api/v1/status";

  Serial.print("Requesting: ");
  Serial.println(url);
  http.begin(url);
  
  http.addHeader("X-Api-Key", apiKey);
  int httpCode = http.GET();

//   Serial.print("HTTP Code: ");
//   Serial.println(httpCode);

  String response = http.getString();

//   Serial.println("Response:");
//   Serial.println(response);

  JsonDocument data;

  DeserializationError error = deserializeJson(data, response);

//   if (error) {
//     Serial.print("JSON Error: ");
//     Serial.println(error.c_str());
//   }

  printState.status = data["printer"]["state"].as<String>();

//   Serial.print("Parsed state: ");
//   Serial.println(printState.status);

  http.end();

  return printState;
}




uint32_t getStatusColor(String status) {
  if(status == "PRINTING")
    return PRINTING;
  else if(status == "ERROR")
    return ERROR;
  else if(status == "FINISHED")
    return FINISHED;
  else if(status == "IDLE")
    return IDLE;
  else if(status == "BUSY" || "READY")
    return PRINTBUSY;
  else if(status == "ATTENTION")
    return ATTENTION;
  else
    return IDLE;

}

void setup() {
//   Serial.begin(115200);

  
  WiFi.begin("BYUI_Visitor", "");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }
  
//   Serial.println("");
//   Serial.println("WiFi connected!");
//   Serial.print("IP address: ");
//   Serial.println(WiFi.localIP());

  indicator.begin(); // This initializes the NeoPixel library.
  indicator.clear(); // This sets all the pixels to 'off'
  indicator.show(); // turn off lights
  for(int i = 0; i < 3; i++) {
    indicator.fill(setUps[i]);
    indicator.show();
    delay(1000);
    indicator.fill(IDLE);
    indicator.show();
    delay(500);
  }

  

//   indicator.setPixelColor(0, 1, 2, 3)
}

void loop() {
//   Serial.println("Printer 1 status wait: ");
  printer1 = getPrinterStatus(ip1, API_KEY1);
  printer2 = getPrinterStatus(ip2, API_KEY2);
  printer3 = getPrinterStatus(ip3, API_KEY3);
  XL       = getPrinterStatus(ipXL, API_KEYXL);

//   Serial.println(printer1.status);
//   Serial.println(printer2.status);
//   Serial.println(printer3.status);
//   Serial.println(XL.status);

  indicator.setPixelColor(0, getStatusColor(printer1.status));
//   Serial.println(getStatusColor(printer1.status));
  indicator.setPixelColor(1, getStatusColor(printer2.status));
//   Serial.println(getStatusColor(printer2.status));
  indicator.setPixelColor(2, getStatusColor(printer3.status));
//   Serial.println(getStatusColor(printer3.status));
  indicator.setPixelColor(3, getStatusColor(XL.status));
//   Serial.println(getStatusColor(XL.status));

  indicator.show();
  delay(5000);
}


// #include <Arduino.h>
// #include <WiFi.h>
// #include <ESPmDNS.h>

// const char* WIFI_SSID = "BYUI_Visitor";
// const char* WIFI_PASSWORD = "";

// const char* PRINTER_HOSTNAME = "prusa-mk4-1";

// void setup() {

//     Serial.begin(115200);

//     Serial.println("Connecting to WiFi...");

//     WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }

//     Serial.println();
//     Serial.println("WiFi connected!");

//     Serial.print("ESP32 IP: ");
//     Serial.println(WiFi.localIP());

//     Serial.println();
//     Serial.print("Looking for: ");
//     Serial.print(PRINTER_HOSTNAME);
//     Serial.println(".local");

//     if (!MDNS.begin("esp32-test")) {
//         Serial.println("mDNS initialization failed!");
//         return;
//     }

//     IPAddress printerIP = MDNS.queryHost(PRINTER_HOSTNAME);

//     if (printerIP == INADDR_NONE) {

//         Serial.println("Could not find printer.");

//     } else {

//         Serial.print("Printer found!");
//         Serial.print(" IP address: ");
//         Serial.println(printerIP);
//     }
// }

// void loop() {
// }

//curl --url 'https://connect.prusa3d.com/app/printers/2e42c40c-3261-4a5d-9577-8c29b1775cfd' \
  -H 'accept: */*' \
  -H 'accept-language: en-US,en;q=0.9' \
  -H 'authorization: Bearer eyJhbGciOiJSUzI1NiIsImtpZCI6IkhTSU53OXQzalhZd0lGaUcxNWVleW1BNlJscFFwVW5veTFrOG0wTW4yM0EiLCJ0eXAiOiJKV1QifQ.eyJqdGkiOiIxNmQyM2ExMTk4Y2Q0YjM5YWMzZWE3NmFiZmYyY2ViNCIsInN1YiI6IjE0MTQyNjciLCJleHAiOjE3OTA0NDk4MTAuMzM2NDgyLCJzaWQiOiIyMjZjNDdjMi03OGMxLTRhNjEtOWY0ZC02ZTRmZGE5Y2Q3NWIiLCJhcHAiOiJjb25uZWN0IiwidHlwZSI6ImFjY2VzcyIsInNjb3BlIjoiYmFzaWNfaW5mbyB1c2VyX29wZXJhdGlvbnMgZW1haWxfbGlzdHMgb3BlbmlkIGNvbm5lY3QiLCJjb25uZWN0X2lkIjoiMjQ1MzMifQ.kGAlf-NJX7-4VMD_r9rnHPNVRLTKdoGXn8N0P51rywVouIONyd_UfRwn3WxmJOVQEIKUUdv3quwkM37nJ2eX56aT3z8xaEvCLH3aUDlgtJGeyKVJmXjquiTMr1oGCo_UcMN7rZ6D5pUCVtmQELND4UzxUamSwmPMRZ7lw5rwgoNNOeVdljINXqB04t-d8FaHSt2AI6iqrKqyoqVofSCFFLlZPc9ujc5OTASXiQe3K6QdtNrMCSd8Sa7IhVX1yj-FVpBVvcpiB2V5Kqt3eE72W4APqUQbcMJzIvk0fKSslj09Lj_I5G5nxSKxniZGYe6PNVDjpAjgOcK8ujeVobRf1w' \
  -H 'baggage: sentry-environment=production,sentry-release=connect-app%402026.7,sentry-public_key=195e1113e2b89386c82b64b4707ed58e,sentry-trace_id=864e2cb9f95243d1921492f4df6eadd0,sentry-org_id=4511750817513472,sentry-sampled=false,sentry-sample_rand=0.5228365466190371,sentry-sample_rate=0.01' \
  -H 'cache-control: no-cache' \
  -b 'cookieyes-consent=consentid:clIyTFp4VGZObXdIdEo2M2hWRTdrcGFoUjdFdjdSVDk,consent:no,action:,necessary:yes,functional:yes,analytics:yes,performance:yes,advertisement:yes,other:yes; _gcl_au=1.1.1048552361.1790216034; _ga=GA1.1.1270798332.1790216034; single_ga=GA1.1.1270798332.1790216034; _twpid=tw.1790216033854.68643710204882366; GlobalE_Analytics=%7B%22merchantId%22%3A%221883%22%2C%22shopperCountryCode%22%3A%22US%22%2C%22cdn%22%3A%22https%3A%2F%2Fwebservices.global-e.com%2F%22%2C%22clientId%22%3A%2214647ca1-cb28-4ce6-a36d-abf318f34ec2%22%2C%22sessionId%22%3A%22efeec5c3-ba1b-4a29-b2db-c43760282d96%22%2C%22sessionIdExpiry%22%3A1790217833928%2C%22configurations%22%3A%7B%22eventSendingStrategy%22%3A0%7D%2C%22featureToggles%22%3A%7B%22FT_3DA%22%3Afalse%2C%22FT_3DA_UTM_SOURCE_LIST%22%3A%5B%5D%2C%22FT_3DA_STORAGE_LIFETIME%22%3A4320%2C%22FT_BF_GOOGLE_ADS%22%3Afalse%2C%22FT_BF_GOOGLE_ADS_LIFETIME%22%3A30%2C%22isOperatedByGlobalE%22%3Atrue%2C%22isPiiDataEnabled%22%3Atrue%2C%22isEventSendingDelayed%22%3Afalse%7D%2C%22lockBrowsingStartOnSessionId%22%3A%22efeec5c3-ba1b-4a29-b2db-c43760282d96%22%2C%22dataUpdatedAt%22%3A1790216033928%2C%22environment%22%3A%22PRODUCTION%22%7D; _sleek_session=%7B%22init%22%3A%222026-09-24T02%3A13%3A53.991Z%22%7D; PAPVisitorId=NTiYJAmDU77StQIvNWjsiVh9m0h7lQOi; _lb_id=7688919316074234000; _lb_ccc=1; _fbp=fb.1.1790216033986.557928484108709501.AQYAAQMC; __hstc=134567790.bccc3f2e17614fd9e9549534a992ec13.1790216035346.1790216035346.1790216035346.1; hubspotutk=bccc3f2e17614fd9e9549534a992ec13; udid=01a0d130-ddda-7075-b2df-3ad300f1fe22@1790216035834; single_ga_2C54J71TLD=GS2.1.s1790216033$o1$g1$t1790217048$j60$l0$h0; _pin_unauth=dWlkPU5tSXlaRFZtTlRFdE9HSmpOUzAwWW1ZeExUaGhNR1F0WW1Fek9EazFOMlZoWVRJdw; _clck=tsjwig%5E2%5Eg9s%5E0%5E2458; lang=en; single_ga_4LGCJS9C33=GS2.1.s1790442559$o1$g0$t1790442609$j10$l0$h0; auth.access_token=eyJhbGciOiJSUzI1NiIsImtpZCI6IkhTSU53OXQzalhZd0lGaUcxNWVleW1BNlJscFFwVW5veTFrOG0wTW4yM0EiLCJ0eXAiOiJKV1QifQ.eyJqdGkiOiIxNmQyM2ExMTk4Y2Q0YjM5YWMzZWE3NmFiZmYyY2ViNCIsInN1YiI6IjE0MTQyNjciLCJleHAiOjE3OTA0NDk4MTAuMzM2NDgyLCJzaWQiOiIyMjZjNDdjMi03OGMxLTRhNjEtOWY0ZC02ZTRmZGE5Y2Q3NWIiLCJhcHAiOiJjb25uZWN0IiwidHlwZSI6ImFjY2VzcyIsInNjb3BlIjoiYmFzaWNfaW5mbyB1c2VyX29wZXJhdGlvbnMgZW1haWxfbGlzdHMgb3BlbmlkIGNvbm5lY3QiLCJjb25uZWN0X2lkIjoiMjQ1MzMifQ.kGAlf-NJX7-4VMD_r9rnHPNVRLTKdoGXn8N0P51rywVouIONyd_UfRwn3WxmJOVQEIKUUdv3quwkM37nJ2eX56aT3z8xaEvCLH3aUDlgtJGeyKVJmXjquiTMr1oGCo_UcMN7rZ6D5pUCVtmQELND4UzxUamSwmPMRZ7lw5rwgoNNOeVdljINXqB04t-d8FaHSt2AI6iqrKqyoqVofSCFFLlZPc9ujc5OTASXiQe3K6QdtNrMCSd8Sa7IhVX1yj-FVpBVvcpiB2V5Kqt3eE72W4APqUQbcMJzIvk0fKSslj09Lj_I5G5nxSKxniZGYe6PNVDjpAjgOcK8ujeVobRf1w; _sleek_product=%7B%22token%22%3A%223546171296880f91d4c400786d4333f46cefea680%22%2C%22user_data%22%3A%7B%22user_id%22%3A4315034%2C%22admin_id%22%3A0%2C%22sso%22%3Atrue%2C%22anonymous%22%3Afalse%2C%22data_name%22%3A%22mac_lab%22%2C%22data_full_name%22%3A%22%22%2C%22data_mail%22%3A%22mckaymaclab%40byui.edu%22%2C%22data_img%22%3A%22https%3A%2F%2Fstorage.sleekplan.com%2Fstatic%2Fimage%2Fuser.png%22%2C%22segments%22%3A%5B%5D%2C%22notify%22%3A1%2C%22notify_settings%22%3A%7B%22mention%22%3Atrue%2C%22changelog%22%3Afalse%2C%22subscribed%22%3Atrue%7D%7D%7D; _rdt_uuid=1790216033858.4a1a8ac7-5e0a-4570-b508-aaecb3b08b20; _uetsid=e9ac58c0b9cc11f1b8750decc019f3ec; _uetvid=97390f60b7bd11f1b99d6d3f9a2f0e9d; _clsk=1cz4xh5%5E1790443237244%5E12%5E1%5Ez.clarity.ms%2Fcollect; sid=id=2497357347414073099|t=1790216037.272|te=1790443239.154|c=982E9CCDB08867A2A7C2B4B9DC4853D3; _twsid=1790442516911-311455947.2.1790443685398; single_ga_QZM3V1NYG1=GS2.1.s1790442516$o1$g1$t1790443686$j59$l0$h0; _ga_3HK7B7RT5V=GS2.1.s1790442516$o2$g1$t1790443686$j59$l0$h0' \
  -H 'pragma: no-cache' \
  -H 'priority: u=1, i' \
  -H 'referer: https://connect.prusa3d.com/printer/2e42c40c-3261-4a5d-9577-8c29b1775cfd/dashboard' \
  -H 'sec-ch-ua: "Chromium";v="154", "Google Chrome";v="154", "Not A(Brand";v="99"' \
  -H 'sec-ch-ua-mobile: ?0' \
  -H 'sec-ch-ua-platform: "macOS"' \
  -H 'sec-fetch-dest: empty' \
  -H 'sec-fetch-mode: cors' \
  -H 'sec-fetch-site: same-origin' \
  -H 'sentry-trace: 864e2cb9f95243d1921492f4df6eadd0-957003dcedeb2049-0' \
  -H 'user-agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/154.0.0.0 Safari/537.36'