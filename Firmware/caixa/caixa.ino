// Controle de caixa da agua utilizando ESP32
// Autores: Danilo de Maria e Lucas Emanuel da Silveira

//definição das bibliotecas
#include <PubSubClient.h>
#include <WiFi.h>
#include <HTTPClient.h>

// definição dos pinos dos sensores em relação a porcentagem de cada um
#define SENSOR_33 5
#define SENSOR_66 17
#define SENSOR_100 16

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

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t * timer = NULL;

//definição das funções
void conectaWifi();
void conectaMQTT();
void atualizaSensores();
void configuraIO();
void IRAM_ATTR isr();
void enviaBD();
void callback(char* topic, byte* payload, unsigned int length);
void cb_timer();
void startTimer();

//definição das variaveis de controle do sistema
int nivelCaixa = 0;
char tempString[8];
boolean flag = false;
boolean envia = false;
int timeSeconds = 5; // 5 segundos

void setup() {
  Serial.begin(115200);
  configuraIO();
  conectaWifi();
  conectaMQTT();
  atualizaSensores();
  enviaBD();
  startTimer();
}

void loop() {
  client.loop();
  if (flag) {
    atualizaSensores();
    enviaBD();
    flag = false;
  }
  if (envia) {
    Serial.println("Acabou tempo, atualizando");
    atualizaSensores();
    enviaBD();
    envia = false;
    startTimer();
  }

}

void configuraIO() {
  // definição dos sensores
  //pinMode(SENSOR_33, INPUT);
  //pinMode(SENSOR_66, INPUT);
  pinMode(SENSOR_100, INPUT);
  //interrupções
  //attachInterrupt(digitalPinToInterrupt(SENSOR_33), isr, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(SENSOR_66), isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SENSOR_100), isr, CHANGE);

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

void atualizaSensores() {
  Serial.println("Atualizando sensores");
  //if(digitalRead(SENSOR_33) == 1){
  //  nivelCaixa = 33;
  //}else if(digitalRead(SENSOR_66) == 1){
  //  nivelCaixa = 66;
  if (digitalRead(SENSOR_100) == 0) {
    nivelCaixa = 100;
  } else {
    nivelCaixa = 0;
  }
  // conversão para trabalhar no chararray do mqtt
  dtostrf(nivelCaixa, 1, 2, tempString);
  //public no topico esp/nivel
  client.publish("esp/nivel", tempString);
  Serial.println(nivelCaixa);
}

void IRAM_ATTR isr() {
  //detachInterrupt(digitalPinToInterrupt(SENSOR_33));
  //detachInterrupt(digitalPinToInterrupt(SENSOR_66));
  detachInterrupt(digitalPinToInterrupt(SENSOR_100));
  flag = true;
  //attachInterrupt(digitalPinToInterrupt(SENSOR_33), isr, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(SENSOR_66), isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SENSOR_100), isr, CHANGE);
}

void enviaBD() {
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

void cb_timer() {
  static unsigned int counter = 0;

  if (counter == timeSeconds) {
    counter = 0;
    stopTimer();
  }
  counter++;
}

void startTimer() {
  //inicialização do timer. Parametros:
  /* 0 - seleção do timer a ser usado, de 0 a 3.
    80 - prescaler. O clock principal do ESP32 é 80MHz. Dividimos por 80 para ter 1us por tick.
    true - true para contador progressivo, false para regressivo
  */
  timer = timerBegin(0, 80, true);

  /*conecta à interrupção do timer
    - timer é a instância do hw_timer
    - endereço da função a ser chamada pelo timer
    - edge=true gera uma interrupção
  */
  timerAttachInterrupt(timer, &cb_timer, true);

  /* - o timer instanciado no inicio
     - o valor em us para 1s
     - auto-reload. true para repetir o alarme
  */
  timerAlarmWrite(timer, 1000000, true);

  //ativa o alarme
  timerAlarmEnable(timer);
}

void stopTimer() {
  timerEnd(timer);
  envia = 1;
  timer = NULL;
}
