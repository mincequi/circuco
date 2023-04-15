#pragma once

#include <any>

#include <ESPAsyncWebServer.h>

#include <ConfigRepository.hpp>
#include <HtmlRenderer.hpp>

class HttpServer {
public:
    HttpServer(ConfigRepository& config, HtmlRenderer& renderer, const FileDataSource& fileSystem) :
    _renderer(renderer),
    _fileSystem(fileSystem),
    _webServer(80) {

    _webServer.on("/", [&](AsyncWebServerRequest *request) {
            sendDocument(request);
        });
        _webServer.on("/circuco.css", [&](AsyncWebServerRequest *request) {
            request->send(200, "text/css", _fileSystem.css().c_str());
        });

        // from-To
        _webServer.on("/if", [&](AsyncWebServerRequest *request) {
            config.changeFrom(1);
            sendDocument(request);
        });
        _webServer.on("/df", [&](AsyncWebServerRequest *request) {
            config.changeFrom(-1);
            sendDocument(request);
        });
        _webServer.on("/it", [&](AsyncWebServerRequest *request) {
            config.changeTo(1);
            sendDocument(request);
        });
        _webServer.on("/dt", [&](AsyncWebServerRequest *request) {
            config.changeTo(-1);
            sendDocument(request);
        });

        // duration-interval
        _webServer.on("/id", [&](AsyncWebServerRequest *request) {
            config.changeDuration(1);
            sendDocument(request);
        });
        _webServer.on("/dd", [&](AsyncWebServerRequest *request) {
            config.changeDuration(-1);
            sendDocument(request);
        });
        _webServer.on("/ii", [&](AsyncWebServerRequest *request) {
            config.changeInterval(1);
            sendDocument(request);
        });
        _webServer.on("/di", [&](AsyncWebServerRequest *request) {
            config.changeInterval(-1);
            sendDocument(request);
        });

        // rate-of-change
        _webServer.on("/ir", [&](AsyncWebServerRequest *request) {
            config.changeRoc(0.1f);
            sendDocument(request);
        });
        _webServer.on("/dr", [&](AsyncWebServerRequest *request) {
            config.changeRoc(-0.1f);
            sendDocument(request);
        });

        _webServer.begin();
    }

    ~HttpServer() {
    }

private:
    void sendDocument(AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginUserDataResponse("text/html", [&](uint8_t *buffer, size_t maxLen, std::any& userData) -> size_t {
            // If user data is not set yet, we start with index 0 and try again
            if (!userData.has_value()) {
                userData = (size_t)0;
                _renderer.render();
                return RESPONSE_TRY_AGAIN;
            } else if (std::any_cast<size_t>(userData) >= _renderer.tokens().size()) {
                return 0;
            }
            // Write up to "maxLen" bytes into "buffer" and return the amount written.
            // You will be asked for more data until 0 is returned
            size_t i = std::any_cast<size_t>(userData);
            const auto& tokens = _renderer.tokens();
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

    HtmlRenderer& _renderer;
    const FileDataSource& _fileSystem;
    AsyncWebServer _webServer;
};
