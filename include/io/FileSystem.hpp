#pragma once

#include <iostream>
#include <regex>
#include <string>
#include <LittleFS.h>
#include <Util.hpp>

#include "FileSystemBase.hpp"

class FileSystem : public FileSystemBase {
public:
    FileSystem() {
        if (!LittleFS.begin()) {
            std::cerr << "FileSystem initialization failed." << std::endl;
        };
    }

    ~FileSystem() {
    }

    std::string htmlTemplate() const override {
        auto _htmlTemplate = loadFile("circuco.html");
        std::cout << "HTML template minified from " << _htmlTemplate.size();
        // TODO: this causes an OOM
        //const std::regex rx1("<!--\\.+-->");
        const std::regex rx2("\\s+<");
        const std::regex rx3(">\\s+");
        //_htmlTemplate = std::regex_replace(_htmlTemplate, rx1, "");
        _htmlTemplate = std::regex_replace(_htmlTemplate, rx2, "<");
        _htmlTemplate = std::regex_replace(_htmlTemplate, rx3, ">");
        std::cout << " to " << _htmlTemplate.size() << " bytes" << std::endl;
        _htmlTemplate.shrink_to_fit();

        return _htmlTemplate;
    }

    std::string css() const override {
        return loadFile("circuco.css");
    }

    std::string config() const override {
        return loadFile("circuco.conf");
    }

    void beginWriteConfig() override {
        _configFile = LittleFS.open("circuco.conf", "w");
    }

    void endWriteConfig() override {
        _configFile.close();
    }

    void writeConfig(const ConfigType& v, bool newLine = false) override {
        if (!_configFile) return;

        std::visit(overloaded {
            [&](int arg) {
                if (newLine)
                    _configFile.println(arg);
                else
                    _configFile.print(arg);
            },
            [&](float arg) {
                if (newLine)
                    _configFile.println(arg);
                else
                    _configFile.print(arg);
            },
            [&](const std::string& arg) {
                if (newLine)
                    _configFile.println(arg.c_str());
                else
                    _configFile.print(arg.c_str());
            }
        }, v);
    }

private:
    static std::string loadFile(const std::string& fileName) {
        File file = LittleFS.open(fileName.c_str(), "r");
        if (!file) return {};

        return file.readString().c_str();
    }

    File _configFile;
};
