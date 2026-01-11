#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#include <FirebaseESP8266.h>
#endif

const int trigPin = 4;
const int echoPin = 2;


volatile int flow_frequency; 
float vol = 0.0,l_minute;
unsigned char flowsensor = 15; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;

#define WIFI_SSID "fuetechmonitor"
#define WIFI_PASSWORD "12345678"
#define API_KEY "AIzaSyBYVwVZUBLXSm7iR5Fp6k-dziJGEuhwExk"
#define DATABASE_URL "megaboth007.firebaseio.com" 
#define USER_EMAIL "pertamina@gmail.com"
#define USER_PASSWORD "12345678"


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


void flow () // Interrupt function to increment flow
{
   flow_frequency++;
}

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(flowsensor, INPUT);
  attachInterrupt(digitalPinToInterrupt(flowsensor), flow, RISING); // Setup Interrupt
  currentTime = millis();
  cloopTime = currentTime;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  Firebase.begin(&config, &auth);

  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

}

void air(){
currentTime = millis();
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000))
   {
    cloopTime = currentTime; // Updates cloopTime
    if(flow_frequency != 0)
    {
        l_minute = (flow_frequency / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
        l_minute = l_minute/60;
        vol = vol +l_minute;
        Serial.print(vol);
        Serial.printf("Set float... %s\n", Firebase.setFloat(fbdo, F("/test/vol"), vol) ? "ok" : fbdo.errorReason().c_str());

        Serial.println(" L");
        flow_frequency = 0;
        
    }
  delay(100);
}
}
void loop() {
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;
  Serial.print("Distance:");
  Serial.print(distance);
  Serial.printf("Set float... %s\n", Firebase.setFloat(fbdo, F("/test/jarak"), distance) ? "ok" : fbdo.errorReason().c_str());

     
  Serial.println(" :");
  delay(100);
  air();
}
