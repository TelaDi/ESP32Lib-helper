/**
 * Вспомогательная библиотека для работы с ESP32 от Кириеша
 */

#ifndef __ESP32_CUSTOM_LIB_H__
#define __ESP32_CUSTOM_LIB_H__

#include <Arduino.h>	
#include <ArduinoOTA.h>

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>

//Класс для инициализации "обновления по воздуху"
class OTA_Config {
  public:
	OTA_Config();
	void configure(const char* hostname);
	void handle();
};
//Класс для более простой работы с ШИМ esp32
class PWM {
	public:
	    //канал [0-15], пин
		PWM(uint8_t channel, uint8_t pin);
		//Значение ШИМ [0-4095]
		void analogWrite(uint16_t pwm);
		uint16_t getPWM();
		uint8_t getPin();
		uint8_t getChannel();
	private:
		uint16_t pwm;
		uint8_t  channel;
		uint8_t  pin;
};

class ESP32Lib {
	public:
		ESP32Lib();
		//метод необходимо вызывать в loop для поддержки работоспособности OTA, MQTT, Wi-Fi
		void handle();
		
		void initOTA (const char* hostname);
		//Инициализация mqtt, имя устройства, адрес mqtt-брокера, функция обработки сообщений из топиков
		void initMQTT(const char* hostname, const char* addr, void (*callback)(char*, uint8_t*, unsigned int));
		//Отправка сообщения через mqtt-брокер
		void MQTTPublish(const char* topic, int16_t num);
        void MQTTPublish(const char* topic, int32_t num);
		void MQTTPublish(const char* topic, const char* message);
		//Подписаться на топик s
		void MQTTSubscribe(const char* s);
		
		bool MQTTConnected();
		void MQTTreconnect();
    private:	
		OTA_Config  ota;
		WiFiClient espClient;
		PubSubClient* client;
		unsigned long MQTTlastReconnect = 0 ;
		const char* hostname;
        unsigned long lastAvailable = 0;
};
extern const char* aliveTopic;
#endif

