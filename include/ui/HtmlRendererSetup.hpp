#pragma once

#include <bl/Config.hpp>

#include "HtmlRendererBase.hpp"

class HtmlRendererSetup : public HtmlRendererBase {
public:
    HtmlRendererSetup(const std::string& templ,
                      const Config& config) :
        HtmlRendererBase(templ),
        _config(config) {
    }

private:
    void doRender(tinja::DataMap& data) const override {
        std::vector<std::string> aps;
        for (const auto& ap : _config.aps()) {
            aps.push_back(ap.first);
        }
        data["aps"] = aps;
    }

    const Config& _config;
};
