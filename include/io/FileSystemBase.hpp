#pragma once

#include <string>
#include <variant>

class FileSystemBase {
public:
    using ConfigType = std::variant<int, float, std::string>;

    virtual std::string mainHtmlTemplate() const = 0;
    virtual std::string setupHtmlTemplate() const = 0;
    virtual std::string css() const = 0;
    virtual std::string config() const = 0;

    virtual void beginWriteConfig() {};
    virtual void endWriteConfig() {};
    virtual void writeConfig(const ConfigType& v, bool newLine = false) {};
};
