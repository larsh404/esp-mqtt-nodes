/**
 * ESP MQTT sensornode example.
 *
 * Example ESP node that reads sensors and publishes the data to
 * MQTT.
 *
 * Usage:
 *
 * 1. Update all <------------> parts with your sensor specific code.
 * 2. Update/extend settings.h
 * 3. Create a credentials.h with all passwords. See settings.h for
 *    corresponding comments. Do not commit this header file.
 */

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>

#include "settings.h"
#include "credentials.h"

ESP8266WiFiMulti wifiMulti;

WiFiClientSecure wclient;
PubSubClient client(mqttHost, mqttPort, wclient);


// Define sensors here. For example:
// DHT dht11(D3, DHT11);
// <------------>

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println();

    // Initialize sensors. For example:
    // dht11.begin();
    // <------------>

    // Add wifi networks. wifiMulti supports multiple networks.
    // and retries to connect to all of them until it succeeds.
    wifiMulti.addAP(ssid0, ssid0Password);
    wifiMulti.addAP(ssid1, ssid1Password);

    // Wait till connected.
    while(wifiMulti.run() != WL_CONNECTED){
      Serial.println("WiFi connection failed, retrying.");
      delay(500);
    }

    // Initialize OTA updates. Must `begin` after wifiMulti.run.
    // OTA does not work reliable with all ESP boards (WiMos works well).
    // Restart of Arduino IDE might be required before node appears.
    ArduinoOTA.setHostname(sensorHostname);
    ArduinoOTA.setPassword(otaPassword);
    ArduinoOTA.begin();
}


bool measureAndPublish() {
    // Read sensors and publish to MQTT
    // Wifi and client are connected when this function is called.
    // Return false when reading failed, to retry faster then the normal
    // update cycle.

    // <------------>

    // Example function:

    // float t = dht11.readTemperature();

    // if (isnan(t)) {
    //     Serial.print("[DHT11] no temperature or humidity\n");
    //     return false;
    // }

    // Serial.print("[DHT11]");
    // Serial.println(t);

    // // Send to MQTT as float with 1 decimal.
    // client.publish(mqttSensorTopic, String(t, 1).c_str());

    // return true;
}

unsigned long lastUpdate = 0;

void loop() {
    if(wifiMulti.run() != WL_CONNECTED) {
        Serial.println("WiFi not connected!");
        delay(1000);
        return;
    }
    // else connected

    ArduinoOTA.handle();

    if (!client.connected()) {
        Serial.println("Connecting to MQTT server");
        if (client.connect(sensorHostname, mqttUser, mqttPassword)) {
          Serial.println("Connected to MQTT server, checking cert");
          if (wclient.verify(mqttFingerprint, mqttHost)) {
            Serial.println("certificate matches");
          } else {
            Serial.println("certificate doesn't match");
            client.disconnect();
            delay(10000);
            return;
          }
        } else {
          Serial.println("Could not connect to MQTT server");
          delay(1000);
          return;
        }
    }

    if (client.connected()) {
        client.loop();
        if (millis() - lastUpdate >= updateInterval) {
            if (measureAndPublish()) {
                lastUpdate = millis();
            } else {
                // retry earlier on error
                lastUpdate = millis() - updateInterval + retyInterval;
            }
        }
    }
}

