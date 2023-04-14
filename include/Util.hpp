#pragma once

#include <iomanip>
#include <sstream>
#include <string>

static std::string toTime(int minutes) {
    std::stringstream ss;
    ss  << std::setfill('0') << std::setw(2) << minutes/60 << ":"
        << std::setfill('0') << std::setw(2) << minutes%60;

    return ss.str();
}

static std::string toDuration(int seconds_, const std::string& nullString) {
    if (!seconds_) return nullString;

    const int hours = seconds_/3600;
    const int minutes = (seconds_%3600)/60;
    const int seconds = (seconds_%60);
    std::stringstream ss;
    if (hours) ss << hours << " h";
    if (hours && minutes) ss << " ";
    if (minutes) ss << minutes << " min";
    if (minutes && seconds) ss << " ";
    if (seconds) ss << " " << seconds << " s";

    return ss.str();
}