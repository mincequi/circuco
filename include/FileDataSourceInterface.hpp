#pragma once

#include <string>

class FileDataSourceInterface {
public:
    virtual std::string htmlTemplate() const = 0;

    virtual std::string css() const = 0;

    virtual std::string config() const = 0;
};
