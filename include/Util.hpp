#pragma once

#include <iomanip>
#include <regex>
#include <sstream>
#include <string>

#include "Logger.hpp"

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::string toTime(int minutes) {
    std::stringstream ss;
    ss  << std::setfill('0') << std::setw(2) << minutes/60 << ":"
        << std::setfill('0') << std::setw(2) << minutes%60;

    return ss.str();
}

std::string toDuration(int seconds_, const std::string& nullString) {
    if (!seconds_) return nullString;

    const int hours = seconds_/3600;
    const int minutes = (seconds_%3600)/60;
    const int seconds = (seconds_%60);
    std::stringstream ss;
    if (hours) ss << hours << " h";
    if (hours && minutes) ss << " ";
    if (minutes) ss << minutes << " min";
    if (minutes && seconds) ss << " ";
    if (seconds) ss << seconds << " s";

    return ss.str();
}

std::string toUptime(uint ts) {
    uint hours = ts/1000/60/60;
    const uint days = hours/24;
    hours %= 24;
    std::stringstream ss;
    if (days) ss << days << " d ";
    ss << hours << " h";

    return ss.str();
}

std::string shrinkHtml(std::string&& html) {
    const auto orgSize = html.size();
    // TODO: this causes an OOM
    //const std::regex rx1("<!--\\.+-->");
    const std::regex rx2("\\s+<");
    const std::regex rx3(">\\s+");
    //html = std::regex_replace(html, rx1, "");
    html = std::regex_replace(html, rx2, "<");
    html = std::regex_replace(html, rx3, ">");
    html.shrink_to_fit();

    LOG("HTML minified from " << orgSize << " to " << html.size() << " bytes");

    return html;
}
