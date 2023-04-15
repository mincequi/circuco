#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <DeviceManager.hpp>
#include <GpioActuator.hpp>
#include <HtmlRenderer.hpp>
#include <Logger.hpp>
//#include <HttpServer.hpp>
#include <AsyncHttpServer.hpp>
#include <TemperatureSensorDs18b20.hpp>
#include <WifiManager.hpp>

TemperatureSensorDs18b20 sensor;
GpioActuator actuator;
FileDataSource fileDataSource;
ConfigRepository configRepository(fileDataSource);
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
    // Configure time for Germany
    configTime("CET-1CEST,M3.5.0/02,M10.5.0/3", "de.pool.ntp.org");

    // Config output
    Serial.begin(115200);
    LOG("setup");

    // Setup our components
    sensor.setup();
    configRepository.setup();

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
        //std::cout << "Connecting to WiFi" << std::endl;
    }

    //std::cout << "loop" << std::endl;

    // Check if we want to update devices
    static uint _lastDevicePoll = 0;
    static uint _lastSavePoll = 0;
    const auto now = millis();
    digitalWrite(LED_BUILTIN, HIGH);

    if (_lastDevicePoll + configRepository.deviceInterval < now) {
        digitalWrite(LED_BUILTIN, LOW);
        _lastDevicePoll = now;
        deviceManager->loop(now);
        //std::cout << "poll" << std::endl;
    } else if (_lastSavePoll + configRepository.saveInterval < now) {
        _lastSavePoll = now;
        configRepository.loop(now);
    }

  // turn the LED on (HIGH is the voltage level)
  //digitalWrite(LED_BUILTIN, millis()%5000/200 == 0 ? LOW : HIGH);
}
