#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <TM1637Display.h>
#include <PubSubClient.h>
#include "letreiro.h"

#define DEBUG 1 //debug [ON]

// wi-fi connection
const char* ssid = "[NETWORK NAME]";
const char* password = "[NETWORK PASSWORD]";

// mqtt [USING LAR-ECT MQTT]

const char* mqttServer = "[MQTT]";
const int port = 0000; // MQTT PORT
const char* mqttUser = "[MQTT USER]";
const char* mqttPassword = "[MQTT PASSWORD]";

const char* mqttTopic = "RACE/LAP";
const char* aliveMessage = "TRACK OK";

const int avliveDelay 5000

WiFiClient espClient;
PubSubClient client(espClient);

void connectToWiFi();

void connectToMQTT();

void reconnect();

void displayTime(unsigned long time);

void stopTimer();

void resetTimer();

void toggleStandby();

void sendTimeToMQTT(unsigned long time);

#define clk 0// Define CLK pin
#define dio 0// Define DIO pin

TM1637Display display(clk, dio);

#define pir 0// definir pino do do PIR
bool pirstatus = 0;

#define start 0// definir pino do botão start
bool racestart = 0;
#define reset 0// definir pino do botão reset
bool restart = 0;

#define standby = 0;

unsigned long startTime;
unsigned long currentTime;
bool running = false;
bool standbyMode = false;

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);

  connectToWiFi();
  connectToMQTT();

  pinMode(start, INPUT_PULLUP);
  pinMode(reset, INPUT_PULLUP);

  pinMode(pir, INPUT);

  pinMode(statusled, OUTPUT);

  display.setBrightness(0x0f);
  display.standbyMessage(display, "ura na ggcom");

  attachInterrupt(digitalPinToInterrupt(start), startTimer, FALLING);
  attachInterrupt(digitalPinToInterrupt(reset), resetTimer, FALLING);  
  attachInterrupt(digitalPinToInterrupt(pir), stopTimer, RISING);
  attachInterrupt(digitalPinToInterrupt(standby), toggleStandby, FALLING);

}

void loop() {
  if (!client.connected()){
    reconnect();
  }

  if(DEBUG){
    Serial.println("Enviando mensagem 'estou vivo'");
    }
  client.publish(mqttTopic, aliveMessage);

}

void connectToWiFi() {
  Serial.println();
  Serial.print("Conectando ao WiFi");
  
  // Conecta-se à rede Wi-Fi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    if(DEBUG){
      Serial.print("Conexão WiFi falhou");
    }
  }
  if(DEBUG){
    Serial.println("Conectado ao WiFi");
  }

}

void connectToMQTT() {
  if(DEBUG){
    Serial.println("Conectando ao MQTT");
  }

  client.setServer(mqttServer, port);

  while (!client.connected()){
    Serial.print(".");
    
    if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
      if(DEBUG){
        Serial.println("Conectado ao MQTT");
      }
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      if(DEBUG){
        Serial.print("Conexão MQTT falhou");
      }
      delay(5000);
    }
  }
}

void reconnect() {
  while (!client.connected()){
    connectToMQTT();
  }
}

void displayTime(unsigned long time) {
  int seconds = (time / 1000) % 60;
  int minutes = (time / (1000 * 60)) % 60;
  display.showNumberDecEx(minutes * 100 + seconds, 0x40, true);
}

void startTimer() {
  if (!running && !standbyMode) {
    startTime = millis();
    running = true;
    Serial.println("Cronômetro iniciado");
  }
}

void stopTimer() {
  if (running) {
    running = false;
    currentTime = millis() - startTime;
    if(DEBUG){
      Serial.println("Cronômetro parado");
      Serial.print("Tempo: ");
      Serial.println(currentTime);
    }
    sendTimeToMQTT(currentTime);
  }
}

void resetTimer() {
  if (!standbyMode) {
    running = false;
    display.showNumberDec(0);
    Serial.println("Cronômetro resetado");
  }
}

void toggleStandby() {
  standbyMode = !standbyMode;
  if (standbyMode) {
    displayStandbyMessage(display, standbyMessage);
    Serial.println("Entrando no modo standby");
  } else {
    display.clear();
    Serial.println("Saindo do modo standby");
  }
}

void sendTimeToMQTT(unsigned long time) {
  if (!client.connected()) {
    reconnect();
  }

  String timeStr = String(time);
  if (client.publish(MQTT_TOPIC, timeStr.c_str())) {
    Serial.println("Tempo enviado ao MQTT");
  } else {
    Serial.println("Falha ao enviar tempo ao MQTT");
  }
}
