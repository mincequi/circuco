#pragma once

#define DEBUG_ESP_HTTP_SERVER
#include <ESP8266WebServer.h>

#include <Config.hpp>
#include <FileSystem.hpp>
#include <HtmlRenderer.hpp>

class HttpServer {
public:
    HttpServer(ConfigRepository& config, const HtmlRenderer& renderer, const FileDataSource& fileSystem) :
    _renderer(renderer),
    _fileSystem(fileSystem),
    _httpServer(80) {

    _httpServer.on("/", [&]() {
            sendDocument();
        });
        _httpServer.on("/circuco.css", [&]() {
            _httpServer.send(200, "text/css", _fileSystem.css().c_str());
        });

        // from-To
        _httpServer.on("/if", [&]() {
            config.changeFrom(1);
            sendDocument();
        });
        _httpServer.on("/df", [&]() {
            config.changeFrom(-1);
            sendDocument();
        });
        _httpServer.on("/it", [&]() {
            config.incrementTo();
            sendDocument();
        });
        _httpServer.on("/dt", [&]() {
            config.decrementTo();
            sendDocument();
        });

        // duration-interval
        _httpServer.on("/id", [&]() {
            config.incrementDuration();
            sendDocument();
        });
        _httpServer.on("/dd", [&]() {
            config.decrementDuration();
            sendDocument();
        });
        _httpServer.on("/ii", [&]() {
            config.incrementInterval();
            sendDocument();
        });
        _httpServer.on("/di", [&]() {
            config.decrementInterval();
            sendDocument();
        });

        // rate-of-change
        _httpServer.on("/ir", [&]() {
            config.incrementRoc();
            sendDocument();
        });
        _httpServer.on("/dr", [&]() {
            config.decrementRoc();
            sendDocument();
        });

        _httpServer.begin();
    }

    ~HttpServer() {
    }

    void loop() {
        _httpServer.handleClient();
    }

private:
    void sendDocument() {
        _httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
        _httpServer.send(200, "text/html", "");
        for (const auto& t : _renderer.tokens()) {
            _httpServer.sendContent(t.get().c_str());
        }

        _httpServer.sendContent("");
    }

    const HtmlRenderer& _renderer;
    const FileDataSource& _fileSystem;
    ESP8266WebServer _httpServer;
};
