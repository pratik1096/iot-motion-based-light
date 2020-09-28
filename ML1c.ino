#include <NTPClient.h>
#include <ESP8266WiFi.h> 
#include <WiFiUdp.h> 
#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#else
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#endif
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);
long now = millis();
long lastMeasure = 0;
#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#else
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#endif
const char* ssid = "ssid";
const char* password = "password";
const char* serverName = "0.0.0.0:0000/myfile";
const char* cloudserverName = "0.0.0.0:0000/myfile";
String apiKeyValue = "**********";
String productcode = "**********";
String uniquekey = "*********";
String modelname = "MLOC";
int val = D2;
int light = A0;
int relay = D3;
int mot;
int intensity;
String Status;
String motion;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);
String arr_days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String date;
int hh;
int mm;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(val, INPUT);
  pinMode(light, INPUT);
  pinMode(relay, OUTPUT);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  timeClient.begin();
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void server() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData =  "api_key=" + apiKeyValue + "&product_code=" + productcode + "&model_name=" + modelname + "&unique_key=" + uniquekey + "&Motion=" +  motion + "&Light=" +  intensity
                              +  "&Status=" + Status +  "&date=" + date + "&hh=" + hh + "&mm=" + mm + "" ;
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }

  
}

void cloudserver() {

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(cloudserverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData =   "api_key=" + apiKeyValue + "&product_code=" + productcode + "&model_name=" + modelname + "&unique_key=" + uniquekey + "&Motion=" +  motion + "&Light=" +  intensity
                               +  "&Status=" + Status +  "&date=" + date + "&hh=" + hh + "&mm=" + mm + "" ;
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }

  
}

void loop()
{ timeClient.update();
  hh = timeClient.getHours();
  mm = timeClient.getMinutes();
  //int ss = timeClient.getSeconds();
  // int day = timeClient.getDay();
  date = timeClient.getFormattedDate();
   
  mot = digitalRead(val);
  intensity = analogRead(light);
  Serial.println(mot);
  if (mot == HIGH and intensity < 200) {
    while (mot == HIGH and intensity < 250) {
      mot = digitalRead(val);
      intensity = analogRead(light);
      digitalWrite(relay, LOW);
      Serial.print(intensity);
      Serial.print("  ");
      Serial.print(mot);
      motion = "1";
      Serial.print("  ");
      Serial.println("Lights ON");
      Status = "On";
      delay(10);
      server();
      cloudserver();
      delay(500);
    }
  }
  else if (mot == HIGH and intensity > 200) {
    mot = digitalRead(val);
    intensity = analogRead(light);
    digitalWrite(relay, HIGH);
    Serial.print(intensity);
    Serial.print("  ");
    Serial.print(mot);
    motion = "0";
    Serial.print("  ");
    Serial.println("Lights OFF");
    Status = "Off";
    delay(10);
    server();
    cloudserver();
    delay(500);
  }
  else if (mot == LOW ) {
    mot = digitalRead(val);
    intensity = analogRead(light);
    digitalWrite(relay, HIGH);
    Serial.print(intensity);
    Serial.print("  ");
    Serial.print(mot);
    motion = "0";
    Serial.print("  ");
    Serial.println("Lights OFF");
    Status = "Off";
    delay(10);
    server();
    cloudserver();
    delay(500);
  }
}
