#pragma once

#include <ESP8266WebServer.h>

#include <Config.hpp>
#include <FileDataSource.hpp>
#include <ui/HtmlRenderer.hpp>

class HttpServer {
public:
    HttpServer(Config& config, const HtmlRenderer& renderer, const FileDataSource& fileSystem) :
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
            config.changeTo(1);
            sendDocument();
        });
        _httpServer.on("/dt", [&]() {
            config.changeTo(-1);
            sendDocument();
        });

        // duration-interval
        _httpServer.on("/id", [&]() {
            config.changeDuration(1);
            sendDocument();
        });
        _httpServer.on("/dd", [&]() {
            config.changeDuration(-1);
            sendDocument();
        });
        _httpServer.on("/ii", [&]() {
            config.changeInterval(1);
            sendDocument();
        });
        _httpServer.on("/di", [&]() {
            config.changeInterval(-1);
            sendDocument();
        });

        // rate-of-change
        _httpServer.on("/ir", [&]() {
            config.changeRoc(0.1);
            sendDocument();
        });
        _httpServer.on("/dr", [&]() {
            config.changeRoc(-0.1);
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