#pragma once

#include <any>

#include <ESPAsyncWebServer.h>

#include <bl/Config.hpp>

#include "HtmlRendererBase.hpp"

class HttpServer {
public:
    HttpServer(Config& config, HtmlRendererBase& renderer, const FileSystemBase& fileSystem) :
    _htmlRenderer(renderer),
    _fileSystem(fileSystem),
    _webServer(80) {

        _webServer.on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
            sendDocument(request, _htmlRenderer);
        });

        _webServer.on("/addap", HTTP_POST, [&](AsyncWebServerRequest *request) {
            const auto args_ = request->args();
            std::unordered_map<std::string, std::string> args;
            for (size_t i = 0; i < args_; ++i) {
                const auto* key = request->argName(i).c_str();
                const auto* value = request->arg(i).c_str();
                args[key] = value;
            }
            const auto& ssid = args["ssid"];
            const auto& pw = args["pw"];
            if (!ssid.empty() && !pw.empty()) {
                LOG("addap> " << ssid << ": *****");
                config.addAp(ssid, pw);
            }

            request->redirect("/");
        });

        _webServer.on("/rmap", HTTP_POST, [&](AsyncWebServerRequest *request) {
            if (request->args()) {
                const auto* ssid = request->argName((size_t)0).c_str();
                LOG("rmap> " << ssid);
                config.rmAp(ssid);
            }
            request->redirect("/");
        });
        
        _webServer.on("/circuco.css", [&](AsyncWebServerRequest *request) {
            request->send(200, "text/css", _fileSystem.css().c_str());
        });

        // TODO: comment this for release
        // _webServer.on("/circuco.conf", [&](AsyncWebServerRequest *request) {
        //     request->send(200, "text/plain", _fileSystem.config().c_str());
        // });

        // from-To
        _webServer.on("/if", [&](AsyncWebServerRequest *request) {
            config.changeFrom(1);
            sendDocument(request, _htmlRenderer);
        });
        _webServer.on("/df", [&](AsyncWebServerRequest *request) {
            config.changeFrom(-1);
            sendDocument(request, _htmlRenderer);
        });
        _webServer.on("/it", [&](AsyncWebServerRequest *request) {
            config.changeTo(1);
            sendDocument(request, _htmlRenderer);
        });
        _webServer.on("/dt", [&](AsyncWebServerRequest *request) {
            config.changeTo(-1);
            sendDocument(request, _htmlRenderer);
        });

        // duration-interval
        _webServer.on("/id", [&](AsyncWebServerRequest *request) {
            config.changeDuration(1);
            sendDocument(request, _htmlRenderer);
        });
        _webServer.on("/dd", [&](AsyncWebServerRequest *request) {
            config.changeDuration(-1);
            sendDocument(request, _htmlRenderer);
        });
        _webServer.on("/ii", [&](AsyncWebServerRequest *request) {
            config.changeInterval(1);
            sendDocument(request, _htmlRenderer);
        });
        _webServer.on("/di", [&](AsyncWebServerRequest *request) {
            config.changeInterval(-1);
            sendDocument(request, _htmlRenderer);
        });

        // rate-of-change
        _webServer.on("/ir", [&](AsyncWebServerRequest *request) {
            config.changeRoc(0.1f);
            sendDocument(request, _htmlRenderer);
        });
        _webServer.on("/dr", [&](AsyncWebServerRequest *request) {
            config.changeRoc(-0.1f);
            sendDocument(request, _htmlRenderer);
        });

        _webServer.begin();
    }

    ~HttpServer() {
    }

private:
    void sendDocument(AsyncWebServerRequest *request, const HtmlRendererBase& renderer) {
        AsyncWebServerResponse *response = request->beginUserDataResponse("text/html", [&](uint8_t *buffer, size_t maxLen, std::any& userData) -> size_t {
            // If user data is not set yet, we start with index 0 and try again
            if (!userData.has_value()) {
                userData = (size_t)0;
                renderer.render();
                return RESPONSE_TRY_AGAIN;
            } else if (std::any_cast<size_t>(userData) >= renderer.tokens().size()) {
                return 0;
            }
            // Write up to "maxLen" bytes into "buffer" and return the amount written.
            // You will be asked for more data until 0 is returned
            size_t i = std::any_cast<size_t>(userData);
            const auto& tokens = renderer.tokens();
            const size_t tokenSize = tokens.at(i).get().size();
            if (tokenSize > maxLen) {
                std::cout << "token " << i << " with length: " << tokenSize << " exceeds max length: " << maxLen << std::endl;
                return 0;
            }

            size_t bytesWritten = 0;
            while (i < tokens.size() && (bytesWritten + tokens.at(i).get().size()) <= maxLen) {
                const auto& token = tokens.at(i).get();
                std::memcpy(buffer + bytesWritten, token.c_str(), token.size());
                bytesWritten += token.size();
                ++i;
            }

            userData = i;
            return bytesWritten;
        });
        request->send(response);
    }

    HtmlRendererBase& _htmlRenderer;
    const FileSystemBase& _fileSystem;
    AsyncWebServer _webServer;
};
