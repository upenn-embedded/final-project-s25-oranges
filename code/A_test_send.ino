#include "SPI.h"
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

// This is the receiver on the robot, doesn work properly sending from feather to atmega

typedef struct data_packet {
  int16_t xa;
  int16_t ya;
} data_packet;

data_packet data;

void data_received(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));

  Serial.print("Received X: ");
  Serial.print(data.xa);
  Serial.print(" | Y: ");
  Serial.println(data.ya);
}


uint8_t rxData;

void setup() {
  Serial.begin(115200);
  SPI.begin();

  WiFi.mode(WIFI_STA);

  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH); 

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(data_received);
}

uint8_t firstByte(int16_t var) {
  return (uint8_t)(var >> 8 & 0xFF);
}

uint8_t lastByte(int16_t var) {
  return (uint8_t)(var & 0xFF);
}

void loop() {
    // Prepare bytes to send
    uint8_t xl = firstByte(data.xa);
    uint8_t xh = lastByte(data.xa);
    uint8_t yl = firstByte(data.ya);
    uint8_t yh = lastByte(data.ya);
  
    // Print the raw values and their byte breakdown
    Serial.print("Preparing to send: XA = ");
    Serial.print(data.xa);
    Serial.print(" (");
    Serial.print(xl);
    Serial.print(", ");
    Serial.print(xh);
    Serial.print("), YA = ");
    Serial.print(data.ya);
    Serial.print(" (");
    Serial.print(yl);
    Serial.print(", ");
    Serial.print(yh);
    Serial.println(")");
  
    // Send via SPI
    digitalWrite(SS, LOW);
    SPI.transfer(xl);
    SPI.transfer(xh);
    SPI.transfer(yl);
    SPI.transfer(yh);
    digitalWrite(SS, HIGH);
  
    delay(100);
  }
  

