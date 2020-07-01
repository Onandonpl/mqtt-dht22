
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//DHT SETTINGS
#include "DHT.h"
#define DHTTYPE DHT22
// DHT SensorPIN SETTING
#define DHTPin 4  

// network.
const char* ssid = "";
const char* password = "";
 const char* mqtt_server = "test.mosquitto.org";
long previousMillis = 0;
long interval = 60000;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;



void setup_wifi() {
   delay(100);
  //  WiFi
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect() {
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    //if  MQTT broker has clientID,username and password
    //if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      client.subscribe("esp/greenhouse/DHT");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} 


// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);                
long now = millis();
long lastMeasure = 0;
//////////////////////////
void setup() {
  Serial.begin(115200);
  setup_wifi();
   dht.begin();
  client.setServer(mqtt_server, 1883);

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  now = millis();
  // Publishes new temperature and humidity every 10 seconds
  if (now - lastMeasure > 10000) {
    lastMeasure = now;
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
//    if (isnan(h) || isnan(t) || isnan(f)) {
//      Serial.println("Failed to read from DHT sensor!");
//      return;
//    }

    // Computes temperature values in Celsius
    float hic = dht.computeHeatIndex(t, h, false);
    static char temperatureTemp[7];
    dtostrf(hic, 6, 2, temperatureTemp);
    
    // Uncomment to compute temperature values in Fahrenheit 
    // float hif = dht.computeHeatIndex(f, h);
    // static char temperatureTemp[7];
    // dtostrf(hic, 6, 2, temperatureTemp);
    
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);

    // Publishes Temperature and Humidity values
    client.publish("esp/greenhouse/DHT/temmperature", temperatureTemp);
    client.publish("esp/greenhouse/DHT/humidity", humidityTemp);
    
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t Heat index: ");
    Serial.print(hic);
    Serial.println(" *C ");
   
   


}}
