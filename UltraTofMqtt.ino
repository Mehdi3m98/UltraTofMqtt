#include <ESP8266WiFi.h>
#include <Wire.h>
#include <NewPing.h>
#include <PubSubClient.h>

#define TRIGGER_PIN 4
#define ECHO_PIN 5
#define MAX_DISTANCE 200
#define REPS 20

const char* mqttServer = "192.168.0.189";
const int mqttPort = 1883;
const char* TraTopic = "DIS//DATA";
const char* ComTopic = "DIS//GET";
int avg_distance ;
unsigned long LTime = 0;

WiFiClient wifiClient;
PubSubClient client(mqttServer, mqttPort, wifiClient);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void Wifi() {
    WiFi.mode(WIFI_STA);
  Serial.println("Scanning Wi-Fi networks...");
  int numNetworks = WiFi.scanNetworks();
  if (numNetworks == 0) {
    Serial.println("No networks found.");
  } else {
    Serial.print(numNetworks);
    Serial.println(" networks found:");
    for (int i = 0; i < numNetworks; i++) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(" dBm)");
      Serial.println("");
      delay(10);
    }
  }
  // Connect to Wi-Fi network
  Serial.println();
  Serial.print("Enter SSID: ");
  while (!Serial.available());
  String ssid = Serial.readStringUntil('\n');
  ssid.trim();

  Serial.print("Enter password: ");
  while (!Serial.available());
  String password = Serial.readStringUntil('\n');
  password.trim();

  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  // Print Wi-Fi network details
  Serial.println("");
   if (!client.connected()) {
    reconnect();
  }
}

//void distance(){
// int meansonar = 0;
//  for (int i = 0; i < REPS; i++) {
 //   meansonar += sonar.ping_cm();
 //   delay(50); // Wait for 50ms before next measurement
 // }
 //int avg_distance = meansonar / REPS;
 // Serial.println(avg_distance);
//}

void setup() {
  Serial.begin(115200);
  Wifi();
  client.setCallback(callback);
}

void loop() {
  //distance();
  Mqtt();
  
}

void Mqtt() {
  unsigned long CTime = millis();
       int meansonar = 0;
  for (int i = 0; i < REPS; i++) {
    meansonar += sonar.ping_cm();
    delay(50); // Wait for 50ms before next measurement
  }
 int avg_distance = meansonar / REPS;
  Serial.println(avg_distance); 
  if (CTime - LTime >= 10000) {
  client.publish(TraTopic,String(avg_distance).c_str());    
    LTime = CTime;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Message received");
  String command = "";
  for (int i = 0; i < length; i++) {
    command += (char)payload[i];
  }
  Serial.println(topic);
  Serial.println("Command: " + command);
  if(!strcmp(command.c_str(),"on")){
  String x="Distance is:"+ String(avg_distance);
    client.publish(ComTopic, x.c_str());
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    //if (client.connect("ESP8266", mqttUser, mqttPassword))
    if (client.connect("ESP8266")) {
      Serial.println("Connected to MQTT");
      client.subscribe(TraTopic);
      client.subscribe(ComTopic);
    } else {
      Serial.print("Failed to connect to MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");   
    }
  }
 }