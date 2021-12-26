#include "ESP32Lib.h"
#include <Arduino.h>	
#include <ArduinoOTA.h>

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>


void ESP32Lib::handle(){
	ArduinoOTA.handle();
	client->loop();
    if (abs(millis() - this->lastAvailable) > 300000) {
    this->lastAvailable = millis();
    int32_t ip = 0;
    for (int i = 0; i < 4; i++) {
      ip |= (WiFi.localIP()[i]) << (8 * i);
    }
    this->MQTTPublish(aliveTopic, ip); 
  }
}


OTA_Config::OTA_Config() {};

void OTA_Config::configure(const char* hostname) {
  // Hostname defaults to esp3232-[MAC]
   ArduinoOTA.setHostname(hostname);

  ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

}

void OTA_Config::handle() {
	ArduinoOTA.handle();

}

ESP32Lib::ESP32Lib() {
}

void ESP32Lib::initMQTT(const char* hostname, const char* addr, void (*callback)(char*, uint8_t*, unsigned int)) {
	this->hostname = hostname;
	client = new PubSubClient(espClient);
	client->setServer(addr, 1883);
	client->setCallback(callback);
	MQTTreconnect();
}
void ESP32Lib::MQTTSubscribe(const char* s) {
	if(!client->connected()) return;
	Serial.print("subcribe to topic ");
	Serial.println(s);
	client->subscribe(s);
}

void ESP32Lib::MQTTreconnect() {
	if(client->connected()) return;
	if(abs(millis() - MQTTlastReconnect) < 5000) {
		return;
	}
	MQTTlastReconnect = millis();
	// Loop until we're reconnected
	Serial.print("Attempting MQTT connection...");
    // Attempt to connect
	if (client->connect(this->hostname)) {
      Serial.println("connected");
	}	
}

bool ESP32Lib::MQTTConnected() {
	return client->connected();
}

void ESP32Lib::MQTTPublish(const char* topic, int32_t num)  {
	String str(num);
	char buff[str.length() + 1];
	str.toCharArray(buff, str.length() + 1);
	MQTTPublish(topic, buff); 
}

void ESP32Lib::MQTTPublish(const char* topic, int16_t num)  {
	String str(num);
	char buff[str.length() + 1];
	str.toCharArray(buff, str.length() + 1);
	MQTTPublish(topic, buff); 
}

void ESP32Lib::MQTTPublish(const char* topic, const char* message) {
	client->publish(topic, message);
}

void ESP32Lib::initOTA(const char* hostname) {
	ota.configure(hostname);
}

PWM::PWM(uint8_t channel, uint8_t pin){
	this->channel = channel;
	this->pin = pin;
	//Канал, частота, битность
	ledcSetup(channel, 1000, 13);
	ledcAttachPin(pin, channel);
}

void PWM::analogWrite(uint16_t pwm) {
	this->pwm = pwm;
	ledcWrite(channel, pwm);
}

uint16_t PWM::getPWM() {
	return pwm;
}
uint8_t PWM::getPin() {
	return pin;
}
uint8_t PWM::getChannel() {
	return channel;
}