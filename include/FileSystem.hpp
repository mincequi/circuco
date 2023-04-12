#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <regex>
#include <string>
#include <LittleFS.h>

class FileSystem {
public:
    FileSystem() {
        if (!LittleFS.begin()) {
            std::cerr << "FileSystem initialization failed." << std::endl;
        };
    }

    ~FileSystem() {
    }

    std::string htmlTemplate() const {
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

    std::string css() const {
        return loadFile("circuco.css");
    }

private:
    static std::string loadFile(const std::string& fileName) {
        File file = LittleFS.open(fileName.c_str(), "r");
        if (!file) return {};

        return file.readString().c_str();
    }
};

#endif // FILESYSTEM_HPP
