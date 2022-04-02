#include "EspMQTTClient.h"
#include "mqtt_secrets.h"
#include "DHT.h"

EspMQTTClient client(
  SECRET_WIFI_NAME,
  SECRET_WIFI_PASSWORD,
  "mqtt3.thingspeak.com",  // MQTT Broker server ip
  SECRET_MQTT_USERNAME,   // Can be omitted if not needed
  SECRET_MQTT_PASSWORD,   // Can be omitted if not needed
  SECRET_MQTT_CLIENT_ID      // Client name that uniquely identify your device
);

// DHT information
#define DHTPIN D2
#define DHTTYPE DHT22       // or set DHT11 type if you use DHT11
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastTime = 0;
unsigned long delayTime = 20000; // set a period of sending data.

void setup() {
  Serial.begin(115200);

  // Begin DHT
  dht.begin();
}

void onConnectionEstablished() {
  Serial.println("MQTT Client is connected to Thingspeak!");
}

void publishData() {
  if (client.isConnected() && (millis() - lastTime > delayTime)) {
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
  
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    Serial.print(F("\nTemperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.println(F("%  "));

    Serial.println(F("\nPublising data to Thingspeak"));
    
    String MY_TOPIC = "channels/" + String(CHANNEL_ID) + "/publish";  // build your topic: channels/<channelID>/publish
    String payload = "field1=" + String(t) + "&field2=" + String(h);  // build your payload: field1=<value1>&field2=<value2>
    client.publish(MY_TOPIC, payload);

    Serial.println("Data published");
    lastTime = millis();
  }
}

void loop() {
  client.loop();
  publishData();
} 
