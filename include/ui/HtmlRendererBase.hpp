#pragma once

#include <tinja.hpp>

#include <Util.hpp>

class HtmlRendererBase {
public:
    HtmlRendererBase(const std::string& templ) {
        const auto tokenCount = _templ.parse(templ);
        LOG("HTML template tokenized to " << tokenCount << " tokens");
    }

    void render() const {
        doRender(_data);
        _templ.renderTo(_data, _tokens);
    }

    const tinja::Template::Tokens& tokens() const {
        return _tokens;
    }

protected:
    virtual void doRender(tinja::DataMap& data) const {};

private:
    tinja::Template _templ;
    mutable tinja::DataMap _data;
    mutable tinja::Template::Tokens _tokens;
};