#pragma once

#include <tinja.hpp>

#include <Util.hpp>

class HtmlRendererBase {
public:
    HtmlRendererBase(const std::string& templ) {
        const auto nodeCount = _templ.parse(templ);
        LOG(nodeCount << " nodes parsed");
    }

    void render() const {
        doRender(_data);
        _templ.renderTo(_data, _tokens);
        LOG(_tokens.size() << " tokens rendered");
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