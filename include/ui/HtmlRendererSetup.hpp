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
    }

    const Config& _config;
};
