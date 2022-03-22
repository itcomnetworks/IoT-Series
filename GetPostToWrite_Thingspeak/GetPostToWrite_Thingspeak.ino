#include <ESP8266WiFi.h>
#include "DHT.h"

// You need to edit with your network credentials.
#define WIFI_NAME "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Initialize the client library
WiFiClient client;

// You need to edit with your ThingSpeak information.
#define THING_SPEAK_ADDRESS "api.thingspeak.com"
String writeAPIKey="YOUR_WRITE_API_KEY";

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
  if ((millis() - lastTime) > delayTime) {
    HTTPPost();
    lastTime = millis();
  }
}

void HTTPGet() {
  if (client.connect(THING_SPEAK_ADDRESS, 80)) {
      Serial.println("Server connected");

      // Read DHT sensor
      float h = dht.readHumidity();    // Read humidity as %
      float t = dht.readTemperature(); // Read temperature as Celsius (the default)
      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }

      // Use HTTP GET request
      Serial.print("Requesting URL: ");
      // Build the GET url: http://api.thingspeak.com/update?api_key=RBDRQF9W9SDPJA6T&field1=0
      String getUrl = "/update?api_key=" + writeAPIKey + "&field1=" + String(t) + "&field2=" + String(h);
      Serial.println(getUrl);

      // Make a HTTP request:
      client.println("GET " + getUrl + " HTTP/1.1");
      client.println("Host: api.thingspeak.com");
      client.println("Connection: close");
      client.println();
      
    }

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000){
      Serial.println(">>> Client Timeout !");
      client.stop(); return;
      }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()){
    String line = client.readStringUntil('\r'); Serial.print(line);
  }
  
  Serial.println("\nClosing connection\n");
}

void HTTPPost() {
  if (client.connect(THING_SPEAK_ADDRESS, 80)) {
    Serial.println("Server connected");

    // Read DHT sensor
    float h = dht.readHumidity();    // Read humidity as %
    float t = dht.readTemperature(); // Read temperature as Celsius (the default)
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    Serial.println("Sending POST request:");
    // String postData= "api_key=" + writeAPIKey + "&field1=" + String(t) + "&field2=" + String(h);
    String postData = "{\"api_key\":\"" + writeAPIKey + "\",\"field1\":\"" + String(t) + "\",\"field2\":\"" + String(h) + "\"}";
    // client.println("POST /update HTTP/1.1");
    client.println("POST /update.json HTTP/1.1");
    client.println( "Host: api.thingspeak.com" );
    client.println( "Connection: close" );
    // client.println( "Content-Type: application/x-www-form-urlencoded" );
    client.println( "Content-Type: application/json" );
    client.println( "Content-Length: " + String( postData.length() ) );
    client.println();
    client.println( postData );
    Serial.println( postData );

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000){
        Serial.println(">>> Client Timeout !");
        client.stop(); return;
        }
    }
    
    // Read all the lines of the reply from server and print them to Serial
    while (client.available()){
      String line = client.readStringUntil('\r'); Serial.print(line);
    }
    
    Serial.println("\nClosing connection\n");
  }
}
