// Controle de caixa da agua utilizando ESP32
// Autores: Danilo de Maria e Lucas Emanuel da Silveira

//definição das bibliotecas
#include <PubSubClient.h>
#include <WiFi.h>
#include <HTTPClient.h>

// definição dos pinos dos sensores em relação a porcentagem de cada um
#define SENSOR_25 17
#define SENSOR_50 35
#define SENSOR_75 34
#define SENSOR_100 33

//definição de conexão wifi local
const char* ssid = "Java";
const char* password = "univali2019";
//definição do servidor mqtt externo
const char* mqttServer = "soldier.cloudmqtt.com";
const int mqttPort = 13470;
const char* mqttUser = "ivytoyoe";
const char* mqttPassword = "EK32RC1rh_bK";

//instanciando objetos dos clientes
WiFiClient espClient;
PubSubClient client(espClient);

//definição das funções
void conectaWifi();
void conectaMQTT();
void atualizaSensores();
void configuraIO();
void IRAM_ATTR isr();
void enviaBD();
void callback(char* topic, byte* payload, unsigned int length);

//definição das variaveis de controle do sistema
int nivelCaixa = 0;
char tempString[8];
boolean flag = false;

void setup() {
  Serial.begin(115200);  
  configuraIO();
  conectaWifi();
  conectaMQTT();
  atualizaSensores();
  enviaBD();
}

void loop() { 
  client.loop(); 
  if(flag){
    atualizaSensores();
    enviaBD();
    flag = false; 
  }
}

void configuraIO(){
  // definição dos sensores
  pinMode(SENSOR_25, INPUT);
  pinMode(SENSOR_50, INPUT);
  pinMode(SENSOR_75, INPUT);
  pinMode(SENSOR_100, INPUT);
  //interrupções
  attachInterrupt(digitalPinToInterrupt(SENSOR_25), isr, CHANGE);
  //attachInterrupt(SENSOR_50, isr, CHANGE);
  //attachInterrupt(SENSOR_75, isr, CHANGE);
  //attachInterrupt(SENSOR_100, isr, CHANGE);
}

void conectaWifi() {
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Conectando a WiFi:");
    Serial.println(ssid);
  }
  Serial.println("Conectado com sucesso!");
  Serial.println("");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void conectaMQTT() {
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  // tentando conexão
  while (!client.connected()) {
    Serial.println("Conectando ao MQTT…");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword )) {
      Serial.println("Conectado.");
    } else {
      Serial.print("Erro no estado: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  // inscrevendo no topic nivelmessage para retornar atualizações da placa
  client.subscribe("esp/nivelmessage");
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Chegou mensagem no topico: ");
  Serial.println(topic);

  Serial.print("Messagem:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  if (payload[0] == '1') {
    Serial.println("\nEnviando dados do sensor");
    atualizaSensores();   
    Serial.println("Nivel do sensor:" );
    Serial.println(nivelCaixa); 
  }
  Serial.println();
  Serial.println(" — — — — — — — — — — — -");
}

void atualizaSensores(){
  if(digitalRead(SENSOR_25) == 1){
    nivelCaixa = 25;
  }else if(digitalRead(SENSOR_50) == 1){
    nivelCaixa = 50;
  }else if(digitalRead(SENSOR_75) == 1){
    nivelCaixa = 75;
  }else if(digitalRead(SENSOR_100) == 1){
    nivelCaixa = 100;
  }else{
    nivelCaixa = 0;
  }
  // conversão para trabalhar no chararray do mqtt
  dtostrf(nivelCaixa, 1, 2, tempString);
  //public no topico esp/nivel
  client.publish("esp/nivel", tempString);
  sprintf(nivelCaixa);
}

void IRAM_ATTR isr() {
  
  detachInterrupt(digitalPinToInterrupt(SENSOR_25));
  flag = true;
  attachInterrupt(digitalPinToInterrupt(SENSOR_25), isr, CHANGE);  
}

void enviaBD(){
  HTTPClient http;
  String url = "http://192.168.137.1:3000/insere?nivel=";
  url = url + String(nivelCaixa);
  Serial.println("Enviando dados para banco de dados.");
  Serial.println(url);
  http.begin(url);
  int httpCode = http.GET();
  String payload = http.getString();
  Serial.println("Resposta do servidor");
  Serial.println(payload);
  http.end();  
}
