#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>

#define LED_50 D6
#define LED_100 D7
#define BUZZER D8

//definição de conexão wifi local
const char* ssid = "Java";
const char* password = "univali2019";

void configuraWifi();
void configuraIO();
void buscaBD();
void atualizaLED();
void acionaBuzzer();

int nivelAgua = 0;

void setup() {
  Serial.begin(115200);
  configuraWifi();
  configuraIO();
}

void loop() {
  buscaBD();
  delay(5000);
}

void configuraWifi() {
  Serial.print("Conectando à ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); // Iniciando conexão na rede

  while (WiFi.status() != WL_CONNECTED ) { // Enquanto não conectar fica printando na tela pontos
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado");
}

void configuraIO() {
  pinMode(LED_50, OUTPUT);
  pinMode(LED_100, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(LED_50, LOW);
  digitalWrite(LED_100, LOW);
}

void buscaBD() {
  HTTPClient http;
  String url = "http://192.168.137.1:3000/busca";
  http.begin(url);
  int httpCode = http.GET();
  Serial.println(httpCode);
  String payload = http.getString();
  Serial.println("Resposta do servidor");
  Serial.println(payload);
  nivelAgua = payload.toInt();
  atualizaLED();
  http.end();
}

void atualizaLED() {
  if (nivelAgua < 50 ) {
    digitalWrite(LED_50, LOW);
    digitalWrite(LED_100, LOW);
    acionaBuzzer();
  }
  if (nivelAgua == 50) {
    digitalWrite(LED_50, HIGH);
    digitalWrite(LED_100, LOW);
  }
  if (nivelAgua == 100) {
    digitalWrite(LED_50, HIGH);
    digitalWrite(LED_100, HIGH);
  }
}

void acionaBuzzer() {
  digitalWrite(BUZZER, HIGH);
  delay(1500);
  digitalWrite(BUZZER, LOW);
}
