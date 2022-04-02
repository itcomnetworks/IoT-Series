#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

// You need to edit with your network credentials.
#define WIFI_NAME "YOUR_WIFI_NAME"//"YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"//"YOUR_WIFI_PASSWORD"

// You need to edit with your ThingSpeak information.
String writeAPIKey="YOUR_WRITE_API_KEY";//"YOUR_WRITE_API_KEY"

// DHT information
#define DHTPIN D2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastTime = 0;
unsigned long delayTime = 20000; // set a period of sending data.

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay(2000);

  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  // Begin DHT Sensor
  dht.begin();
  
  delay(2000);
}

void loop() {
    //Send an HTTP POST request every 20 seconds
    if ((millis() - lastTime) > delayTime) {
        httpGETRequest();
        lastTime = millis();
    }
}

void httpGETRequest() {    
    // Read DHT sensor
    float h = dht.readHumidity();    // Read humidity in %
    float t = dht.readTemperature(); // Read temperature as Celsius (the default)
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.println(t);

    WiFiClient client;
    HTTPClient http;
    
    // Build URL GET path.
    String urlPath = "http://api.thingspeak.com/update?api_key=" + writeAPIKey + "&field1=" + String(t) + "&field2=" + String(h);

    http.begin(client, urlPath);
    
    // Send HTTP GET request
    Serial.println("Sending HTTP GET request");
    int httpResponseCode = http.GET();
  
    if (httpResponseCode>0) {
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

void httpPostRequest() {
    // Read DHT sensor
    float h = dht.readHumidity();    // Read humidity as %
    float t = dht.readTemperature(); // Read temperature as Celsius (the default)
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.println(t);
  
    WiFiClient client;
    HTTPClient http;
    http.begin(client, "http://api.thingspeak.com/update.json");
    /*
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Data to send with HTTP POST
    String httpRequestData = "api_key=" + writeAPIKey + "&field1=" + String(t) + "&field2=" + String(h);
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
    */
    
    // If you need an HTTP request with a content type: application/json, use the following:
    http.addHeader("Content-Type", "application/json");
    // JSON data to send with HTTP POST
    String httpRequestData = "{\"api_key\":\"" + writeAPIKey + "\",\"field1\":\"" + String(t) + "\",\"field2\":\"" + String(h) + "\"}";
    // Send HTTP POST request
    Serial.println("Sending HTTP POST request");
    int httpResponseCode = http.POST(httpRequestData);
     
    String payload = "{}"; 
  
    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
        Serial.println(payload);
    }
    else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
}
