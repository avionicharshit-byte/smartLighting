#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <DHT.h>

#define DHTPIN 32     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "led"
float h ;
float t;
 
DHT dht(DHTPIN, DHTTYPE);

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

const int pirPin = 27;  // PIR sensor pin
const int pirPin2 = 33;  // PIR sensor pin
const int ldrPin = 26; // LDR pin
const int led1Pin = 13; // LED 1 pin
const int led2Pin = 12; // LED 2 pin
const int led3Pin = 14; // LED 3 pin
const int led4Pin = 25; // LED 4 pin
const int led5Pin = 36; // LED 4 pin
const int led6Pin = 39; // LED 4 pin
  // Define the pin where the LED is connected
int pirState = LOW;
int pirState2 = LOW;
int ldrValue = 0;
unsigned long lastMotionTime = 0;
const unsigned long motionTimeout = 5000;

void connectAWS() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(messageHandler);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage() {
 StaticJsonDocument<200> doc;
  doc["pirvalue"] = pirState;
  doc["pirvalue2"]= pirState2;
  doc["ldrvalue"] = ldrValue;
  doc["humidity"] = h;
  doc["temperature"] = t;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
void dimlights()
{
  analogWrite(led1Pin,40);
  analogWrite(led2Pin,40);
  analogWrite(led3Pin,40);
  analogWrite(led4Pin,40);
  analogWrite(led5Pin,40);
  analogWrite(led6Pin,40);
}
void highlights(){
  analogWrite(led1Pin,255);
  analogWrite(led3Pin,255);
  analogWrite(led5Pin,255);
  analogWrite(led2Pin,255);
  analogWrite(led4Pin,255);
  analogWrite(led6Pin,255);
}

void turnoffLeds() {
 analogWrite(led1Pin,0);
  analogWrite(led2Pin,0);
  analogWrite(led3Pin,0);
  analogWrite(led4Pin,0);
  analogWrite(led5Pin,0);
  analogWrite(led6Pin,0);
}


void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("incoming: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const int value = doc["message"]; // Assuming the message is either 0 or 1

  if (value == 11) {
    analogWrite(led3Pin, 255);}
    else if(value==10){
      analogWrite(led3Pin,0);
    }
    if(value==11){
  analogWrite(led4Pin, HIGH);
    }
    else if(value==10){
      analogWrite(led4Pin,LOW);
    }

   /* if(value==51){
  digitalWrite(led5Pin, HIGH);
    }
    else if(value==50){
      digitalWrite(led5Pin,LOW);
    }
    if(value==61){
  digitalWrite(led6Pin, HIGH);
    }
    else if(value==60){
      dgitalWrite(led6Pin,LOW);
    }
    if(value==71){
  digitalWrite(led7Pin, HIGH);
    }
    else if(value==70){
      digitalWrite(led7Pin,LOW);
    }
    if(value==81){
  digitalWrite(led8Pin, HIGH);
    }
    else if(value==80){
      dgitalWrite(led8Pin,LOW);
    }
    if(value==91){
  digitalWrite(led9Pin, HIGH);
    }
    else if(value==90){
      dgitalWrite(led9Pin,LOW);
    }
    if(value==101){
  digitalWrite(led10Pin, HIGH);
    }
    else if(value==100){
      dgitalWrite(led10Pin,LOW);
    }
    if(value==?){
  digitalWrite(led2Pin, HIGH);
    }
    else if(value==20){
      dgitalWrite(led2Pin,LOW);
    }*/
}

void setup() {
  Serial.begin(115200);
  connectAWS();

  // Initialize LED pin
  pinMode(pirPin, INPUT);
  pinMode(pirPin2, INPUT);
  pinMode(ldrPin, INPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(led4Pin,OUTPUT);
  pinMode(led5Pin,OUTPUT);
  pinMode(led6Pin,OUTPUT);

  // Ensure LED starts in OFF state
  
}

void loop() {
  pirState = digitalRead(pirPin);
  pirState2=digitalRead(pirPin2);
  ldrValue = digitalRead(ldrPin);

  if (ldrValue ==1) { // LDR senses darkness
    dimlights();
    if (pirState == HIGH) { // PIR detects motion
    highlights();
    delay(1000);
    //count++;
   }
 
    if(pirState==LOW)
    {
      dimlights();
    }
    if (pirState2 == HIGH) { // PIR detects motion
    highlights();
    delay(1000);
  
   }
    
  } 

  if(ldrValue==0)
  {
    turnoffLeds();
  }


  

   h = dht.readHumidity();
  t = dht.readTemperature();
 
 
  if (isnan(h) || isnan(t) )  // Check if any reads failed and exit early (to try again).
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
 
  Serial.print("PIR State: ");
  Serial.println(pirState);
   Serial.print("PIR State2: ");
  Serial.println(pirState2);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));

   delay(1000);// Add a short delay to avoid rapid sensor readings
  /*pirState = digitalRead(pirPin);
  ldrValue = digitalRead(ldrPin);

  if (ldrValue ==1) { // LDR senses darkness
    turnOnLeds();
    delay(5000);
  turnoffLeds();}
    else if (pirState == HIGH) { // PIR detects motion
    turnOnLeds();
    lastMotionTime = millis(); // Update last motion time
  } else if (millis() - lastMotionTime > motionTimeout) { // Check if no motion for 10 minutes
    turnoffLeds();
  }


  Serial.print("PIR State: ");
  Serial.println(pirState);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

   delay(1000);// Add a short delay to avoid rapid sensor readings*/
  publishMessage();
  client.loop();
  delay(1000);
}