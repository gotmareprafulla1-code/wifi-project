/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/

#include <Arduino.h>
#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>

#include "soc/soc_caps.h"
#include "sdkconfig.h"
#if SOC_WIFI_SUPPORTED || CONFIG_ESP_WIFI_REMOTE_ENABLED

#include "esp_wifi_types.h"
#include "WiFiType.h"
#include "WiFiGeneric.h"
#include <WiFiUdp.h>

#endif
#define WIFI_AP_DEFAULT_AUTH_MODE WIFI_AUTH_WPA2_PSK
#define WIFI_AP_DEFAULT_CIPHER    WIFI_CIPHER_TYPE_CCMP  

extern "C" {
#include "lwip/apps/lwiperf.h"
}

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED
#endif

// Set these to your desired credentials.
const char *ssid = "shubh";
const char *password = "gotya2405";

uint8_t myCount;
NetworkServer server(80);

const int localPort = 5000;
const char* stationIP = "192.168.4.2";

WiFiUDP udp;
int data = 1; 

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP);
  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  if (!WiFi.softAP(ssid, password,1,0,1)) 
  {
    log_e("Soft AP creation failed.");
    while (1);
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  udp.begin(localPort);
  Serial.println("Server started");
  myCount = WiFi.softAPgetStationNum();  
  Serial.print(myCount);
}

void loop() 
{
  
    WiFiClient client = server.available();
    myCount = WiFi.softAPgetStationNum();  
    Serial.print(myCount); 

    if(myCount > 0)
    { 
      udp.beginPacket(stationIP, localPort);
      udp.write((uint8_t*)&data, sizeof(data));
      udp.endPacket();

      data++;

      if(data > 11)
      {  
        data = 0;
      }

      int count =0;
      while(count < 4)
      { 
        digitalWrite(LED_BUILTIN, HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN, LOW);
        delay(50);
        count++; 
      } 

      Serial.println(data);  
        
  }
    
 // listen for incoming clients
  else
  {
    client.stop();
    Serial.println("Client Disconnected.");
  }
  // close the connection:
   
  
}
