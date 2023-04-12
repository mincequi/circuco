#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <iostream>

#include <DeviceManager.hpp>
#include <GpioActuator.hpp>
#include <HtmlRenderer.hpp>
//#include <HttpServer.hpp>
#include <AsyncHttpServer.hpp>
#include <TemperatureSensorDs18b20.hpp>
#include <WifiManager.hpp>

WifiManager wifiManager;
Config config;
TemperatureSensorDs18b20 sensor;
GpioActuator actuator;
FileSystem fileSystem;
// Since i experienced OOM exception after a while, I moved the following objects on the heap.
// This is was the ESP8266 doc tells me:
// https://arduino-esp8266.readthedocs.io/en/latest/faq/a02-my-esp-crashes.html#other-causes-for-crashes
// "Objects that have large data members, such as large arrays, should also be avoided on the stack,
//  and should be dynamically allocated (consider smart pointers)."
std::unique_ptr<DeviceManager> deviceManager;
std::unique_ptr<HtmlRenderer> htmlRenderer;
std::unique_ptr<HttpServer> httpServer;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    wifiManager.connect();
    sensor.setup();

    configTime("CET-1CEST,M3.5.0/02,M10.5.0/3", "de.pool.ntp.org");

    /*
    std::cout << "Connecting to WiFi: " << config.wifiSsid() << std::endl;
    WiFi.begin(config.wifiSsid().c_str(), config.wifiPassword().c_str());
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    std::cout << std::endl << "IP: " << WiFi.localIP().toString().c_str() << std::endl;
    */

    deviceManager = std::make_unique<DeviceManager>(config, sensor, actuator);
    htmlRenderer = std::make_unique<HtmlRenderer>(config, sensor, actuator, *deviceManager, fileSystem);
    httpServer = std::make_unique<HttpServer>(config, *htmlRenderer, fileSystem);

    htmlRenderer->setup();

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (!wifiManager.isConnected()) {
    wifiManager.connect();
  }

  // Check if we want to update devices
  static uint _millis = 0;
  const auto now = millis();
  digitalWrite(LED_BUILTIN, HIGH);

  if (_millis + config.loopInterval < now) {
    digitalWrite(LED_BUILTIN, LOW);
    _millis = now;
    deviceManager->loop(_millis);
  }

  // turn the LED on (HIGH is the voltage level)
  //digitalWrite(LED_BUILTIN, millis()%5000/200 == 0 ? LOW : HIGH);
}
