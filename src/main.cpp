#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <Logger.hpp>
#include <WifiManager.hpp>
#include <ui/HtmlRenderer.hpp>

#include <io/DeviceManager.hpp>
#include <io/GpioActuator.hpp>
#include <io/TemperatureSensorDs18b20.hpp>
//#include <ui/HttpServer.hpp>
#include <ui/HttpServerAsync.hpp>

TemperatureSensorDs18b20 sensor;
GpioActuator actuator;
FileDataSource fileDataSource;
Config config(fileDataSource);
// Since i experienced OOM exception after a while, I moved the following objects on the heap.
// This is was the ESP8266 doc tells me:
// https://arduino-esp8266.readthedocs.io/en/latest/faq/a02-my-esp-crashes.html#other-causes-for-crashes
// "Objects that have large data members, such as large arrays, should also be avoided on the stack,
//  and should be dynamically allocated (consider smart pointers)."
std::unique_ptr<WifiManager> wifiManager;
std::unique_ptr<DeviceManager> deviceManager;
std::unique_ptr<HtmlRenderer> htmlRenderer;
std::unique_ptr<HttpServer> httpServer;

void setup() {
    // Config output
    Serial.begin(115200);
    LOG("setup");
    pinMode(LED_BUILTIN, OUTPUT);

    // Configure time for Germany
    configTime("CET-1CEST,M3.5.0/02,M10.5.0/3", "de.pool.ntp.org");

    wifiManager = std::make_unique<WifiManager>(config);
    deviceManager = std::make_unique<DeviceManager>(config, sensor, actuator);
    htmlRenderer = std::make_unique<HtmlRenderer>(config, sensor, actuator, *deviceManager, fileDataSource);
    httpServer = std::make_unique<HttpServer>(config, *htmlRenderer, fileDataSource);

    // Setup our components
    sensor.setup();
    config.setup();
    wifiManager->connect();
    htmlRenderer->setup();
}

void loop() {
    if (!wifiManager->isConnected()) {
        wifiManager->connect();
    }

    // Check if we want to update devices
    static uint _lastDevicePoll = 0;
    static uint _lastSavePoll = 0;
    const auto now = millis();

    // TODO: comment this for WIFFI pump
    digitalWrite(LED_BUILTIN, HIGH);

    if (_lastDevicePoll + config.deviceInterval < now) {
        // TODO: comment this for WIFFI pump
        digitalWrite(LED_BUILTIN, LOW);
        _lastDevicePoll = now;
        deviceManager->loop(now);
    } else if (_lastSavePoll + config.saveInterval < now) {
        _lastSavePoll = now;
        config.loop(now);
    }

  // turn the LED on (HIGH is the voltage level)
  //digitalWrite(LED_BUILTIN, millis()%5000/200 == 0 ? LOW : HIGH);
}
