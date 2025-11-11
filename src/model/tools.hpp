#pragma once

#ifndef TOOLS_H
#define TOOLS_H

#include <SFML/Graphics.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

class Tools {
   public:
    Tools();

    static sf::Text objText;
    static sf::Font font_spacemono_regular;
    static void configure();
    static int getStartSprite(int, int);
    static float ceil_special(float num, float biggerthan);
    static float floor_special(float num, float lessthan);
    static std::string addZero(int seconds);
    static std::string seconds_to_hour(int seconds);
    static int time_dif_in_seconds(std::chrono::system_clock::time_point, std::chrono::system_clock::time_point);
    static int wordOccurrence(std::string sentence, std::string word);
    static bool hasString(std::string line, std::string str);
    static std::string get_lines_from_dtm(std::string filename, std::string tag, std::string endLine);
    static void say(sf::RenderWindow* w, std::string text, int left, int top, int fontSize = 12, sf::Color color = sf::Color::Black);
    static double getRand();
    static void printHour();
    static std::string formatDouble(double value, int decimalPlaces);
    static sf::Color hslaToRgba(float h, float s, float l, float a);
    static float map(float x, float a, float b, float c, float d);
    static void fileLog(std::string log, const std::string& arquivo);
    static std::string onOff(bool logicVar);
    static void log(std::string log);
};
#endif
