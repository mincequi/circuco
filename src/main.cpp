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

TemperatureSensorDs18b20 sensor;
GpioActuator actuator;
FileDataSource fileDataSource;
// Since i experienced OOM exception after a while, I moved the following objects on the heap.
// This is was the ESP8266 doc tells me:
// https://arduino-esp8266.readthedocs.io/en/latest/faq/a02-my-esp-crashes.html#other-causes-for-crashes
// "Objects that have large data members, such as large arrays, should also be avoided on the stack,
//  and should be dynamically allocated (consider smart pointers)."
ConfigRepository configRepository(fileDataSource);
std::unique_ptr<WifiManager> wifiManager;
std::unique_ptr<DeviceManager> deviceManager;
std::unique_ptr<HtmlRenderer> htmlRenderer;
std::unique_ptr<HttpServer> httpServer;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
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

    wifiManager = std::make_unique<WifiManager>(configRepository);
    deviceManager = std::make_unique<DeviceManager>(configRepository, sensor, actuator);
    htmlRenderer = std::make_unique<HtmlRenderer>(configRepository, sensor, actuator, *deviceManager, fileDataSource);
    httpServer = std::make_unique<HttpServer>(configRepository, *htmlRenderer, fileDataSource);

    wifiManager->connect();
    htmlRenderer->setup();

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (!wifiManager->isConnected()) {
    wifiManager->connect();
  }

  // Check if we want to update devices
  static uint _millis = 0;
  const auto now = millis();
  digitalWrite(LED_BUILTIN, HIGH);

  if (_millis + configRepository.loopInterval < now) {
    digitalWrite(LED_BUILTIN, LOW);
    _millis = now;
    deviceManager->loop(_millis);
  }

  // turn the LED on (HIGH is the voltage level)
  //digitalWrite(LED_BUILTIN, millis()%5000/200 == 0 ? LOW : HIGH);
}
