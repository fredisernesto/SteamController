#include <WiFi.h>
#include <DNSServer.h>
#include "DHT.h"

//Pins definition
#define DHTTYPE           DHT11
#define DHTPIN              2 
#define SteamValvePIN       23
#define WaterValvePIN       22
#define StrovePIN           21
#define WaterLevelPIN       33
#define StroveLevelPIN      32
#define WaterLevelAlarmPIN  19


//Setpoints
#define TempSnow       0
#define HumSnow        20

//Status
bool SteamValve;
bool WaterValve;
bool Strove;
int WaterLevel;
bool StroveLevel;
float Temp;
float Hum;

//
const byte DNS_PORT = 53;
IPAddress apIP(8,8,4,4); // The default android DNS
DNSServer dnsServer;
WiFiServer server(80);
DHT dht(DHTPIN, DHTTYPE);

String responseHTML = ""
  "<!DOCTYPE html><html><head><title>ControlVapor</title></head><body>"
  "<h1>Hello World!</h1><p>Sistema de Control de Vapor "
  "</p></body></html>";

void setup() {
  pinMode(StrovePIN,OUTPUT);
  pinMode(WaterValvePIN,OUTPUT);
  pinMode(SteamValvePIN,OUTPUT);
  pinMode(WaterLevelAlarmPIN, OUTPUT);
  //pinMode(WaterLevelPIN,INPUT);
  pinMode(StroveLevelPIN,INPUT); 
  digitalWrite(StrovePIN,HIGH);
  digitalWrite(WaterValvePIN,LOW);
  digitalWrite(SteamValvePIN,LOW);
  digitalWrite(WaterLevelAlarmPIN, LOW);
  dht.begin();
  Serial.begin(9600);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Unidad_Controladora_Vapor");
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  server.begin();
}

void loop() {
  // Reading sensors
  Temp = dht.readTemperature();
  Hum = dht.readHumidity();
  WaterLevel=analogRead(WaterLevelPIN);
  StroveLevel=digitalRead(StroveLevelPIN);
  
  //  Monitor
  Serial.print(F("Humidity: "));
  Serial.print(Hum);
  Serial.print(F("%  Temperature: "));
  Serial.print(Temp);
  Serial.println(F("°C "));
  Serial.print(F("Water level "));
  Serial.println(WaterLevel);
  Serial.print(F("Strove level "));
  Serial.println(StroveLevel);
  
   //Testing
   
  digitalWrite(StrovePIN,HIGH);
  digitalWrite(WaterValvePIN,HIGH);
  digitalWrite(SteamValvePIN,HIGH);
  delay(1000);
  digitalWrite(StrovePIN,LOW);
  digitalWrite(WaterValvePIN,LOW);
  digitalWrite(SteamValvePIN,LOW);
  delay(1000);


  //Servers
  dnsServer.processNextRequest();
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(responseHTML);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
}
