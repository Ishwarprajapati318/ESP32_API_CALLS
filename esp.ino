#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

TaskHandle_t connectToWiFiHandler = NULL;
const char *ssid = "xxxxxxxxxxxxxxx";
const char *password = "xxxxxxxxxxxxx";
String serverName = "http://192.168.43.10:4004/testingAPI";

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return "failed";
  }
  Serial.print("Read from file: ");
  char dataa[];
  while(file.available()){
    dataa += file.read();
  }
  file.close();
  return dataa;
}

void connectToWiFi(void *parameters)
{
  for (;;)
  {

    if (WiFi.isConnected())
    {
      Serial.println("wifi connected already");
      vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    else
    {
      Serial.println("Connecting to wifi");
      WiFi.begin(ssid, password);
      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
  }
}

void queryData(String key_name){
      HTTPClient http;
      http.begin(serverName.c_str());
      int httpResponseCode = http.GET();
      if (httpResponseCode == 200) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        writeFile(SD, "/api.txt", payload);
        Serial.println(key_name);
        JSONVar myObject = JSON.parse(payload);
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
      }else{
        JSONVar value = myObject[key_name]; //working
        Serial.println(value);        
        Serial.println(key_name);
       }
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
       char dataa[] = readFile(SD, "/api.txt");
       JSONVar myObject = JSON.parse(dataa);
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
      }else{
        JSONVar value = myObject[key_name]; //working
        Serial.println(value);        
        Serial.println(key_name);
       }
      }
     
      http.end();
}


void setup()
{
  Serial.begin(9600);
   WiFi.begin();
   xTaskCreatePinnedToCore(
       connectToWiFi,         
       "Connect to wifi",     
       3000,                  
       NULL,                  
       1,                     
       &connectToWiFiHandler,
       1                      
   );
}

void loop()
{
    if (Serial.available() > 0) {
    String incomingByte = Serial.readStringUntil('\r');
    queryData(incomingByte);
    }else{
      Serial.println("No data read");
    }
    delay(1000);
}
