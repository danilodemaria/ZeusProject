#include <PubSubClient.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define LED_33 5
#define LED_66 6
#define LED_100 7
#define BUZZER 8

//definição de conexão wifi local
const char* ssid = "Java";
const char* password = "univali2019";

//instanciando objetos dos clientes
WiFiClient espClient;

hw_timer_t * timer = NULL;

void conectaWifi();
void configuraIO();
void cb_timer();
void startTimer();
void buscaBD();
void atualizaLED();
void acionaBuzzer();

int timeSeconds = 60; // 60 segundos
int nivelAgua = 0;
boolean envia = false;

void setup() {
  Serial.begin(115200);
  configuraIO();
  conectaWifi();
  buscaBD();
  startTimer();
}

void loop() {
  if (envia) {
    Serial.println("Acabou tempo, atualizando");    
    buscaBD();
    envia = false;
    startTimer();
  }
}

void configuraIO() {
  pinMode(LED_33, OUTPUT);
  pinMode(LED_66, OUTPUT);
  pinMode(LED_100, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(LED_33,LOW);
  digitalWrite(LED_66,LOW);
  digitalWrite(LED_100,LOW);
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

void buscaBD() {
  HTTPClient http;
  String url = "http://192.168.137.1:3000/buscaUltimo";  
  Serial.println("Buscando informações no banco de dados.");
  Serial.println(url);
  http.begin(url);
  int httpCode = http.GET();
  String payload = http.getString();
  Serial.println("Resposta do servidor");
  Serial.println(payload);
  nivelAgua = payload.toInt();
  atualizaLED();
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

void atualizaLED(){
  if(nivelAgua == 33){
    digitalWrite(LED_33,HIGH);
    acionaBuzzer();    
  }else if(nivelAgua == 66){
    digitalWrite(LED_33,HIGH);
    digitalWrite(LED_66,HIGH);
  }else{
    digitalWrite(LED_33,HIGH);
    digitalWrite(LED_66,HIGH);
    digitalWrite(LED_100,HIGH);
  }
}

void acionaBuzzer(){
  digitalWrite(BUZZER,HIGH);
  delay(1500);
  digitalWrite(BUZZER,LOW);
}
