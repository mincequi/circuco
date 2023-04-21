#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <unity.h>
#include <tinja.hpp>

#ifdef ARDUINO
#include <io/FileSystem.hpp>
#endif

void minifyHtml(std::string& str) {
    //std::cout << "HTML template minified from " << str.size();
    const std::regex rx2("\\s+<");
    const std::regex rx3(">\\s+");
    //_htmlTemplate = std::regex_replace(_htmlTemplate, rx1, "");
    str = std::regex_replace(str, rx2, "<");
    str = std::regex_replace(str, rx3, ">");
    //std::cout << " to " << str.size() << " bytes" << std::endl;
    str.shrink_to_fit();
}

std::string readHtmlFile(const std::string& fileName) {
    std::ifstream file(fileName);
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    minifyHtml(str);
    return str;
}

void tinja_parses_setup(void) {
#ifdef ARDUINO
    FileSystem fs;
    const auto str = fs.setupHtmlTemplate();
#else
    const auto str = readHtmlFile("data/setup.html");
#endif

    tinja::Template templ;
    TEST_ASSERT_EQUAL_INT(8, templ.parse(str));
    tinja::DataMap map;
    map["aps"] = tinja::Strings { "ap1", "ap2", "ap3" };
    tinja::Template::Tokens tokens;
    templ.renderTo(map, tokens);
    TEST_ASSERT_EQUAL_INT(13, tokens.size());
}

auto runTinjaTest() {
    UNITY_BEGIN();
    RUN_TEST(tinja_parses_setup);
    return UNITY_END();
}