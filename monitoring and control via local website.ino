#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//#include <WiFiClient.h>
WiFiClient client;

// WiFi Parameters
String host = "192.168.43.189";
char ssid[] = "Redmi123";
char password [] = "aryanicosa";
//timer
unsigned long timePrevious = 0;
const long interval = 1000;
//control
#define led D5
//monitor
#define potPin A0
int pot = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(led, OUTPUT);

  Serial.println("connecting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("");
  Serial.print("Connected to Wifi with IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - timePrevious >= interval) {
    timePrevious = millis();

    // Check WiFi Status and GET data from page
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;  //Object of class HTTPClient
      http.begin("http://" + host + "/tes/json.php");
      int httpCode = http.GET();
      //Check the returning code
      if (httpCode > 0) {
        // Get the request response payload
        //String payload = http.getString();

        // TODO: Parsing
        const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 20;
        DynamicJsonDocument doc(capacity);

        String json = http.getString();
        deserializeJson(doc, json);

        const char* id = doc[0]["id"]; // "1"
        const char* nilai = doc[0]["nilai"]; // "1"

        // Output to serial monitor
        Serial.print("id    : ");
        Serial.println(id);
        Serial.print("nilai : ");
        Serial.println(nilai);

        if (doc[0]["nilai"] == "1") {
          Serial.println("NYALA");
          digitalWrite(led, HIGH);
        } else {
          Serial.println("PADAM");
          digitalWrite(led, LOW);
        }
      }

      pot = analogRead(potPin);
      Serial.println(pot);
      if (client.connect(host, 80)) {
        String link = "/tes/terimaDataSensor.php";
        String myData = "sensor=" + String(pot);
        int contentLength = myData.length();
        //client.print(String("GET ") + myData + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
        client.print(String("POST ") + link + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "Content-Type: application/x-www-form-urlencoded" + "\r\n" +
                          "Content-Length: " + contentLength + "\r\n\r\n" +
                          myData);
      } else {
        Serial.println("data tidak terkirim");
      }

      http.end();   //Close connection
    }
  }

}
