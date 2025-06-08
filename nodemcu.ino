#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

/************************* WiFi Access Point *********************************/
#define WLAN_SSID       ""      // Use double quotes for string literals
#define WLAN_PASS       ""

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  ""         // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/voltage");
Adafruit_MQTT_Publish photocel2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/AmpsRMS0");
Adafruit_MQTT_Publish photocel3 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temp");
Adafruit_MQTT_Publish photocel4 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/PerdistanceMm");

SoftwareSerial s(D4, D3);  // Assuming T4 and T3 are defined correctly

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println(F("Power Monitoring"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());

  s.begin(9600); 
  while (!Serial) continue;
}

void loop() {
  StaticJsonDocument<1000> jsonDocument;
  DeserializationError error = deserializeJson(jsonDocument, s);
  
  if (error) return;
  
  Serial.println("IOT based Transformer Monitoring system");
  serializeJsonPretty(jsonDocument, Serial);

  int voltage = jsonDocument["voltage"];
  int AmpsRMS0 = jsonDocument["AmpsRMS0"];
  int Temp = jsonDocument["Temp"];
  int PerdistanceMm = jsonDocument["PerdistanceMm"];

  Serial.print("voltage: ");
  Serial.println(voltage);
  
  Serial.print("AmpsRMS0: ");
  Serial.println(AmpsRMS0);

  Serial.print("Temp: ");
  Serial.println(Temp);

  Serial.print("PerdistanceMm: ");
  Serial.println(PerdistanceMm);

  MQTT_connect();

  if (!photocell.publish(voltage)) {
    Serial.println(F("Failed to publish voltage"));
  } else {
    Serial.println(F("Published voltage OK!"));
  }

  if (!photocel2.publish(AmpsRMS0)) {
    Serial.println(F("Failed to publish AmpsRMS0"));
  } else {
    Serial.println(F("Published AmpsRMS0 OK!"));
  }

  if (!photocel3.publish(Temp)) {
    Serial.println(F("Failed to publish Temp"));
  } else {
    Serial.println(F("Published Temp OK!"));
  }

  if (!photocel4.publish(PerdistanceMm)) {
    Serial.println(F("Failed to publish PerdistanceMm"));
  } else {
    Serial.println(F("Published PerdistanceMm OK!"));
  }
}

void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.println("Connecting to Adafruit... ");
  uint8_t retries = 3;

  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);

    retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("Adafruit Connected!");
}
