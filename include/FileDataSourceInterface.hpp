#pragma once

#include <string>
#include <variant>

class FileDataSourceInterface {
public:
    using ConfigType = std::variant<int, float, std::string>;

    virtual std::string htmlTemplate() const = 0;

    virtual std::string css() const = 0;

    virtual std::string config() const = 0;

    virtual void beginSaveConfig() = 0;
    virtual void endSaveConfig() = 0;
    virtual void saveConfig(const ConfigType& v, bool newLine = false) = 0;
};