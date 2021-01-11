#include "EEPROM.h"
#define EEPROM_SIZE 512

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "heltec.h"
#include <WiFi.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
String token;
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDxTCCAq2gAwIBAgIBADANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx\n" \
"EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT\n" \
"EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp\n" \
"ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5MDkwMTAwMDAwMFoXDTM3MTIzMTIz\n" \
"NTk1OVowgYMxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH\n" \
"EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjExMC8GA1UE\n" \
"AxMoR28gRGFkZHkgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjCCASIw\n" \
"DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL9xYgjx+lk09xvJGKP3gElY6SKD\n" \
"E6bFIEMBO4Tx5oVJnyfq9oQbTqC023CYxzIBsQU+B07u9PpPL1kwIuerGVZr4oAH\n" \
"/PMWdYA5UXvl+TW2dE6pjYIT5LY/qQOD+qK+ihVqf94Lw7YZFAXK6sOoBJQ7Rnwy\n" \
"DfMAZiLIjWltNowRGLfTshxgtDj6AozO091GB94KPutdfMh8+7ArU6SSYmlRJQVh\n" \
"GkSBjCypQ5Yj36w6gZoOKcUcqeldHraenjAKOc7xiID7S13MMuyFYkMlNAJWJwGR\n" \
"tDtwKj9useiciAF9n9T521NtYJ2/LOdYq7hfRvzOxBsDPAnrSTFcaUaz4EcCAwEA\n" \
"AaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYE\n" \
"FDqahQcQZyi27/a9BUFuIMGU2g/eMA0GCSqGSIb3DQEBCwUAA4IBAQCZ21151fmX\n" \
"WWcDYfF+OwYxdS2hII5PZYe096acvNjpL9DbWu7PdIxztDhC2gV7+AJ1uP2lsdeu\n" \
"9tfeE8tTEH6KRtGX+rcuKxGrkLAngPnon1rpN5+r5N9ss4UXnT3ZJE95kTXWXwTr\n" \
"gIOrmgIttRD02JDHBHNA7XIloKmf7J6raBKZV8aPEjoJpL1E/QYVN8Gb5DKj7Tjo\n" \
"2GTzLH4U/ALqn83/B2gX2yKQOC16jdFU8WnjXzPKej17CuPKf1855eJ1usV2GDPO\n" \
"LPAvTK33sefOT6jEm0pUBsV/fdUID+Ic/n4XuKxe9tQWskMJDE32p2u0mYRlynqI\n" \
"4uJEvlz36hz1\n" \
"-----END CERTIFICATE-----\n";

#define stop_width 60
#define stop_height 60
static unsigned char stop_bits[] = {
   0x00, 0x00, 0xfc, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
   0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x1c, 0x00, 0x00,
   0x00, 0xc0, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00,
   0x00, 0x70, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00,
   0x00, 0x30, 0xa0, 0xaa, 0x2a, 0xc0, 0x00, 0x00, 0x00, 0x0c, 0xf8, 0xff,
   0xff, 0x81, 0x03, 0x00, 0x00, 0x0c, 0x0c, 0x00, 0x80, 0x03, 0x02, 0x00,
   0x00, 0x03, 0x0e, 0x00, 0x00, 0x06, 0x06, 0x00, 0x00, 0x03, 0x03, 0x00,
   0x00, 0x04, 0x1c, 0x00, 0xc0, 0x80, 0x01, 0x00, 0x00, 0x00, 0x30, 0x00,
   0xc0, 0xc0, 0x00, 0x00, 0x00, 0x20, 0x30, 0x00, 0x30, 0xe0, 0x00, 0x00,
   0x00, 0x60, 0xe0, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0xc0, 0x80, 0x00,
   0x0c, 0x18, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x06, 0x0c, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x02, 0x06, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06,
   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x83, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x18, 0x0c, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0c,
   0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0c, 0x83, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x30, 0x0c, 0xc3, 0xf0, 0xe1, 0x1f, 0x3e, 0xfc, 0x10, 0x0c,
   0x83, 0x58, 0x43, 0x03, 0x6b, 0xac, 0x31, 0x0c, 0x83, 0x08, 0x02, 0x83,
   0x41, 0x04, 0x11, 0x0c, 0xc3, 0x08, 0x02, 0x81, 0x61, 0x04, 0x31, 0x0c,
   0x83, 0x08, 0x00, 0x83, 0x41, 0x04, 0x11, 0x0c, 0xc3, 0x0c, 0x00, 0x81,
   0x61, 0x04, 0x31, 0x0c, 0x83, 0xf8, 0x01, 0x83, 0x41, 0xfc, 0x11, 0x0c,
   0xc3, 0xf0, 0x03, 0x81, 0x61, 0xfc, 0x30, 0x0c, 0x83, 0x00, 0x02, 0x83,
   0x41, 0x04, 0x10, 0x0c, 0xc3, 0x00, 0x02, 0x81, 0x61, 0x04, 0x30, 0x0c,
   0x83, 0x08, 0x02, 0x83, 0x41, 0x04, 0x10, 0x0c, 0xc3, 0x08, 0x02, 0x81,
   0x61, 0x04, 0x30, 0x0c, 0x83, 0xb8, 0x03, 0x03, 0x77, 0x04, 0x10, 0x0c,
   0xc3, 0xf0, 0x01, 0x01, 0x3e, 0x04, 0x30, 0x0c, 0x83, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x10, 0x0c, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x0c,
   0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0c, 0x83, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x18, 0x0c, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0c,
   0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x0c, 0x0c, 0x00, 0x00,
   0x00, 0x00, 0x03, 0x06, 0x0c, 0x18, 0x00, 0x00, 0x00, 0x80, 0x81, 0x01,
   0x38, 0x30, 0x00, 0x00, 0x00, 0xe0, 0x80, 0x01, 0x20, 0xe0, 0x00, 0x00,
   0x00, 0x60, 0xe0, 0x00, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00,
   0x80, 0x80, 0x01, 0x00, 0x00, 0x18, 0x18, 0x00, 0x80, 0x03, 0x00, 0x00,
   0x00, 0x00, 0x18, 0x00, 0x00, 0x06, 0x08, 0x61, 0x0c, 0x01, 0x06, 0x00,
   0x00, 0x0c, 0x08, 0x63, 0x0c, 0x01, 0x07, 0x00, 0x00, 0x18, 0x08, 0x61,
   0x88, 0x81, 0x01, 0x00, 0x00, 0x30, 0x08, 0x01, 0x00, 0x80, 0x00, 0x00,
   0x00, 0x60, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00,
   0x00, 0x30, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00,
   0x00, 0x80, 0x03, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff,
   0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0x03, 0x00, 0x00 };

const int ledPin = 22;
const int modeAddr = 0;
const int wifiAddr = 10;

int modeIdx;

void stopLogo(){
  Heltec.display -> clear();
  Heltec.display -> drawXbm(0,5,stop_width,stop_height,(const unsigned char *)stop_bits);
  Heltec.display -> display();
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.print("Value : ");
        Serial.println(value.c_str());
        writeString(wifiAddr, value.c_str());
      }
    }

    void writeString(int add, String data) {
      int _size = data.length();
      for (int i = 0; i < _size; i++) {
        EEPROM.write(add + i, data[i]);
      }
      EEPROM.write(add + _size, '\0');
      EEPROM.commit();
    }
};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  if (!EEPROM.begin(EEPROM_SIZE)) {
    delay(1000);
  }

  modeIdx = EEPROM.read(modeAddr);
  Serial.print("modeIdx : ");
  Serial.println(modeIdx);

  EEPROM.write(modeAddr, modeIdx != 0 ? 0 : 1);
  EEPROM.commit();

  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/);
  Heltec.display->clear();

  if (modeIdx != 0) {
    //BLE MODE
    digitalWrite(ledPin, true);
    Serial.println("BLE MODE");
    bleTask();
  } else {
    //WIFI MODE
    digitalWrite(ledPin, false);
    Serial.println("WIFI MODE");
    wifiTask();
  }

}

void bleTask() {
  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void wifiTask() {
  String receivedData;
  receivedData = read_String(wifiAddr);

  if (receivedData.length() > 0) {
    String wifiName = getValue(receivedData, ',', 0);
    String wifiPassword = getValue(receivedData, ',', 1);
    if (wifiName.length() > 0 && wifiPassword.length() > 0) {
      token = getValue(receivedData, ',', 2);
      Serial.print("WifiName : ");
      Serial.println(wifiName);

      Serial.print("wifiPassword : ");
      Serial.println(wifiPassword);

      WiFi.begin(wifiName.c_str(), wifiPassword.c_str());
      Serial.print("Connecting to Wifi");
      while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
      }
    }
  }
}

String read_String(int add) {
  char data[512];
  int len = 0;
  unsigned char k;
  k = EEPROM.read(add);
  while (k != '\0' && len < 500) {
    k = EEPROM.read(add + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loop() {
  bool busy = false;
    if (modeIdx != 0) {
      delay(2000);
      Heltec.display->clear();
      Heltec.display -> drawString(0, 0, "BLE MODE");
      Heltec.display -> display();
      
      delay(2000);
      Heltec.display->clear();      
    } else {
      delay(60000);
      WiFiClientSecure *client = new WiFiClientSecure;
      if(client) {
        client -> setCACert(rootCACertificate);
        {
          // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
          HTTPClient https;
      
          Serial.print("[HTTPS] begin...\n");
          
          if (https.begin(*client, "https://webexapis.com/v1/people/me")) {  // HTTPS
            https.setReuse(true);
            https.addHeader("Authorization", "Bearer " + token);
            Serial.print("[HTTPS] GET...\n");
            // start connection and send HTTP header
            int httpCode = https.GET();
      
            // httpCode will be negative on error
            if (httpCode > 0) {
              // HTTP header has been send and Server response header has been handled
              Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
              String payload = https.getString();
              if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {                
                Serial.println(payload);
                String status;
                if (payload.indexOf("\"status\":\"active\"")>-1) {status = "Status active!"; busy = false;}
                if (payload.indexOf("\"status\":\"call\"")>-1) {status = "Status CALL!"; busy = true;}
                if (payload.indexOf("\"status\":\"DoNotDisturb\"")>-1) {status = "Status DND!"; busy = true;}
                if (payload.indexOf("\"status\":\"inactive\"")>-1) {status = "Status inactive!"; busy = false;}
                if (payload.indexOf("\"status\":\"meeting\"")>-1) {status = "Status meeting!"; busy = true;}
                if (payload.indexOf("\"status\":\"OutOfOffice\"")>-1) {status = "Status OOO!"; busy = false;}
                if (payload.indexOf("\"status\":\"pending\"")>-1) {status = "Status pending!"; busy = false;}
                if (payload.indexOf("\"status\":\"presenting\"")>-1) {status = "Status presenting!"; busy = true;}
                if (payload.indexOf("\"status\":\"unknown\"")>-1) {status = "Status unknown!"; busy = false;}
                if (busy) {
                  Serial.print(status);
                  Heltec.display -> clear();
                  Heltec.display -> display();
                  stopLogo();
                } else {
                    Heltec.display -> clear();
                    //Heltec.display -> drawString(0, 10, status);
                    Serial.print("Status: " + status + "\n");  
                    Heltec.display -> display();
                } 
              } else if (httpCode == 401) {
                Serial.println("access token refresh!");
                Serial.println(payload);
                String receivedData;
                receivedData = read_String(wifiAddr);
                Serial.println(receivedData);
                if (receivedData.length() > 0) {
                  String accessToken = getValue(receivedData, ',', 2);
                  String clientId = getValue(receivedData, ',', 3);
                  String clientSecret = getValue(receivedData, ',', 4);
                  String refreshToken = getValue(receivedData, ',', 5);

                  String request = String("{\"grant_type\":\"refresh_token\", \"client_id\": \"" + clientId + "\",\"client_secret\": \"" + clientSecret + "\", \"refresh_token\": \"" + refreshToken + "\" }");

                  Serial.println(request);
                  if (https.begin(*client, "https://webexapis.com/v1/access_token")) {
                     https.addHeader("Content-Type", "application/json"); 
                     httpCode = https.POST(request);
                     Serial.println(httpCode);
                     
                     if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                        String payloadRefresh = https.getString();
                        Serial.println(payloadRefresh);
                        const size_t capacity = JSON_OBJECT_SIZE(4) + 310;
                        DynamicJsonDocument doc(capacity);

                        deserializeJson(doc, payloadRefresh);

                        const char* access_token = doc["access_token"];
                        Serial.println(access_token);
                        token = access_token;
                     }
                  }
                }  
              }
                                 
            } else {
              Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
            }
      
            https.end();
          } else {
            Serial.printf("[HTTPS] Unable to connect\n");
          }
    
        }
      
        delete client;
      } else {
        Serial.println("Unable to create client");
      }
    }
}
