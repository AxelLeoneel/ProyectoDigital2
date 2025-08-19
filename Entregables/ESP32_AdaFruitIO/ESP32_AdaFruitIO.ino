#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Credenciales WiFi
#define WLAN_SSID "Internte gratis"
#define WLAN_PASS "12345678"

// Credenciales Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "LeoGC"
#define AIO_KEY         "aio_sFkU06IgIUC9TLRGHbsTTdiSLI6B"

// Configuración de pines
#define LED1 3 
#define RXD2 16
#define TXD2 17

// Variables
String uartBuffer = "";
unsigned long lastPublish = 0;
const unsigned long publishInterval = 5000;

float tempValue = 0;
float humValue  = 0;
int airValue    = 0;
int rainValue   = 0;
bool newValues  = false;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Suscripción al feed
Adafruit_MQTT_Subscribe led1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led-1");

// Publicación a feeds
Adafruit_MQTT_Publish pubTemp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp-aht10");
Adafruit_MQTT_Publish pubHum  = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/hum-aht10");
Adafruit_MQTT_Publish pubAir  = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/air-mq135");
Adafruit_MQTT_Publish pubRain = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/rain-adc");

void MQTT_Connect() {
  int8_t ret;
  if (mqtt.connected()) return;

  Serial.print("Conectando a MQTT...");
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    mqtt.disconnect();
    delay(500);
  }
  Serial.println("Conectado a MQTT.");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, HIGH);  // LED apagado al inicio

  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado.");
  mqtt.subscribe(&led1);

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // UART2
}

void loop() {
  MQTT_Connect();

  // Leer UART
  while (Serial2.available()) {
    char c = Serial2.read();
    if (c == '\n' || c == '\r') {
      if (uartBuffer.length() > 0) {
        procesarLinea(uartBuffer);
        uartBuffer = "";
      }
    } else {
      uartBuffer += c;
    }
  }

  // Manejar suscripciones
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) {
    if (subscription == &led1) {
      Serial.print("Led 1 feed: ");
      Serial.println((char *)led1.lastread);

      if (!strcmp((char*) led1.lastread, "1")) {
        digitalWrite(LED1, HIGH);
      } else if (!strcmp((char*) led1.lastread, "0")) {
        digitalWrite(LED1, LOW);
      }
    }
  }

  // Publicar a Adafruit IO con intervalo
  if (newValues && millis() - lastPublish > publishInterval) {
    bool ok = pubTemp.publish((double)tempValue) &&
              pubHum.publish((double)humValue) &&
              pubAir.publish((int32_t)airValue) &&
              pubRain.publish((int32_t)rainValue);

    if (ok) {
      Serial.println("Valores enviados a Adafruit IO");
    } else {
      Serial.println("Error al enviar valores");
    }

    lastPublish = millis();
    newValues = false; // esperar nuevos datos
  }
}

// Procesar línea recibida por UART
void procesarLinea(String linea) {
  int idx1 = linea.indexOf(',');
  int idx2 = linea.indexOf(',', idx1 + 1);
  int idx3 = linea.indexOf(',', idx2 + 1);

  if (idx1 == -1 || idx2 == -1 || idx3 == -1) {
    Serial.println("Formato UART inválido.");
    return;
  }

  tempValue = linea.substring(0, idx1).toFloat();
  humValue  = linea.substring(idx1 + 1, idx2).toFloat();
  airValue  = linea.substring(idx2 + 1, idx3).toInt();
  rainValue = linea.substring(idx3 + 1).toInt();

  Serial.print("Temp: "); Serial.println(tempValue);
  Serial.print("Hum: ");  Serial.println(humValue);
  Serial.print("Air: ");  Serial.println(airValue);
  Serial.print("Rain: "); Serial.println(rainValue);

  newValues = true;  // datos listos para enviar
  delay(4000);
}
