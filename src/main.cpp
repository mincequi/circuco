//#include <Arduino.h>
//#include <HardwareSerial.h>

#include <Logger.hpp>
#include <bl/DeviceManager.hpp>
#include <bl/WifiManager.hpp>
#include <io/FileSystem.hpp>
#include <io/ActuatorGpio.hpp>
#include <io/SensorTemperatureDs18b20.hpp>
#include <ui/HtmlRenderer.hpp>
//#include <ui/HttpServer.hpp>
#include <ui/HttpServerAsync.hpp>

// These are our stack components
SensorTemperatureDs18b20 sensor;
ActuatorGpio actuator;
FileSystem fileSystem;
Config config(fileSystem);
// Since i experienced OOM exception after a while, I moved the following objects on the heap.
// This is was the ESP8266 doc tells me:
// https://arduino-esp8266.readthedocs.io/en/latest/faq/a02-my-esp-crashes.html#other-causes-for-crashes
// "Objects that have large data members, such as large arrays, should also be avoided on the stack,
//  and should be dynamically allocated (consider smart pointers)."
// These are our heap components
std::unique_ptr<WifiManager> wifiManager;
std::unique_ptr<DeviceManager> deviceManager;
std::unique_ptr<HtmlRenderer> htmlRenderer;
std::unique_ptr<HttpServer> httpServer;

void setup() {
    // Config output
    Serial.begin(115200);
    LOG("");
    LOG("setup");
    pinMode(LED_BUILTIN, OUTPUT);

    // Configure time for Germany
    configTime("CET-1CEST,M3.5.0/02,M10.5.0/3", "de.pool.ntp.org");

    // Setup stack components
    sensor.setup();
    config.setup();

    // Create heap components
    wifiManager = std::make_unique<WifiManager>(config);
    deviceManager = std::make_unique<DeviceManager>(config, sensor, actuator);
    htmlRenderer = std::make_unique<HtmlRenderer>(config, sensor, actuator, *deviceManager, fileSystem);
    httpServer = std::make_unique<HttpServer>(config, *htmlRenderer, fileSystem);

    // Setup heap components
    wifiManager->setup();
    htmlRenderer->setup();
}

void loop() {
    if (!wifiManager->isConnected()) {
        wifiManager->setup();
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
