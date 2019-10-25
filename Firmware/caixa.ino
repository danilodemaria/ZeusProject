#include <PubSubClient.h>
#include <WiFi.h>
#define SENSOR_01 36

const char* ssid = "Java";
const char* password = "univali2019";
const char* mqttServer = "soldier.cloudmqtt.com";
const int mqttPort = 13470;
const char* mqttUser = "ivytoyoe";
const char* mqttPassword = "EK32RC1rh_bK";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(SENSOR_01, INPUT);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting to WiFi:");
    Serial.println(ssid);
  }
  Serial.println("Connected to the WiFi network");
  Serial.println("");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT…");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword )) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  client.subscribe("esp/nivelmessage");
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  if (payload[0] == '1') {
    Serial.println("\nEnviando dados do sensor");
    client.publish("esp/nivel", "85");
  }

  Serial.println();
  Serial.println(" — — — — — — — — — — — -");

}

void loop() {
  delay(980);
  client.loop();
  char tempString[8];
  int aux = digitalRead(SENSOR_01);
  dtostrf(aux, 1, 2, tempString);
  client.publish("esp/nivel", tempString);



}
