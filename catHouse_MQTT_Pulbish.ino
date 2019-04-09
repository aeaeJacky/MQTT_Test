// ESP8266
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "DHTesp.h"

String apiKey = "2IRFKWM93CCLMWA7";
const char* server = "api.thingspeak.com";
WiFiClient client;

// MQTT Publish
const char* SubscribeToTemp = "office/catHouse1/temperature"; //For publish temperature /location/Cat House #/Measure item
const char* SubscribeToHum = "office/catHouse2/humidity"; //For publish humidity
//const char* SubscribeToPot = "esp/1/pot";

// WiFi & MQTT Server
const char* ssid = "XXX";
const char* password = "XXX";
const char* mqtt_server = "192.168.XXX.XXX";

WiFiClient catHouseClient;
PubSubClient pubClient(catHouseClient);

long lastTimeRecorded = 0; //lastMsg
char msg[50];

String humMessage;
String tempMessage;
//String potMessage;


String lastHumMessage("0");
//String lastPotMessage("0");

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

float humidity = 0;
float temperature = 0;
//int potValue = 0;
//const int AnalogIn  = A0;

DHTesp dht;

void setup() {
  Serial.begin(115200);
  
  // WiFi
  setup_wifi();
  
  // MQTT
  Serial.print("Connecting to MQTT...");
  // connecting to the mqtt server
  pubClient.setServer(mqtt_server, 1883);
  pubClient.setCallback(callback);
  Serial.println("done!");

  //For DHT
  String thisBoard= ARDUINO_BOARD;
  Serial.println(thisBoard);

  // Autodetect is not working reliable, don't use the following line
  // dht.setup(17);
  // use this instead: 
  dht.setup(D2, DHTesp::DHT22); // Connect DHT sensor to GPIO D3

}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String color("#");
  
  Serial.println();

//    if((char)payload[0] == '#'){
//     // setting color
//     setNeoColor(color);
//  }

}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  
  if (!pubClient.connected()) {
    delay(100);
    reconnect();
  }
  pubClient.loop();

  humidity = dht.getHumidity();
  temperature = dht.getTemperature();
  
  long now = millis();
  if (now - lastTimeRecorded > 120000 || lastHumMessage.indexOf(humMessage) < 0) {

    lastTimeRecorded = now;
    
    humMessage = humidity;
 
    Serial.print("Publish Humidity Value: ");
    Serial.println(humMessage);
    char msgH[7];
    humMessage.toCharArray(msgH, humMessage.length());
    
    Serial.print("Publishing Humidity...");
    pubClient.publish(SubscribeToHum, msgH);
    Serial.println("Humidity Done!!!");

    tempMessage = temperature;

    Serial.print("Publish Temperature Value: ");
    Serial.println(tempMessage);
    char msgT[7];
    tempMessage.toCharArray(msgT, tempMessage.length());
    
    Serial.print("Publishing Temperature...");
    pubClient.publish(SubscribeToTemp, msgT);
    Serial.println("Temperature Done!!!");
    
    lastHumMessage = humMessage;

//    potMessage = potValue;
 
//    Serial.print("Publish Potentiometer Value: ");
//    Serial.println(potMessage);
//    char msgPot[7];
//    potMessage.toCharArray(msgPot, potMessage.length());
//    
//    Serial.print("Publishing Potentiometer Value...");
//    pubClient.publish(SubscribeToPot, msgPot);
//    Serial.println("Potentiometer Value Done!!!");
//
//    lastPotMessage = potMessage;
//      
//  Serial.println(potValue);

     if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
        {  
              
               String postStr = apiKey;
               postStr +="&field1=";
               postStr += String(temperature);
               postStr +="&field2=";
               postStr += String(humidity);
               postStr += "\r\n\r\n";

               client.print("POST /update HTTP/1.1\n");
               client.print("Host: api.thingspeak.com\n");
               client.print("Connection: close\n");
               client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
               client.print("Content-Type: application/x-www-form-urlencoded\n");
               client.print("Content-Length: ");
               client.print(postStr.length());
               client.print("\n\n");
               client.print(postStr);

               Serial.print("Temperature: ");
               Serial.print(temperature);
               Serial.print(" degrees Celcius, Humidity: ");
               Serial.print(humidity);
               Serial.println("%. Sent to Thingspeak.");

          }

  }
  
  delay(10);
}

void reconnect() {
  // Loop until we're reconnected
  while (!pubClient.connected()) {
    // Attempt to connect
    if (pubClient.connect("ESP_Client")) {
      Serial.println("ESP_Client connected");
      // Once connected, publish an announcement...
//      pubClient.publish(SubscribeToTemp, "Restart");
      pubClient.publish(SubscribeToHum, "Restart");
      // ... and resubscribe
      //pubClient.subscribe(SubscribeFrom);
    } else {
      Serial.print("failed, rc=");
      Serial.print(pubClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    } 
  }
}

