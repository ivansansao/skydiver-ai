#include "tools.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <random>

sf::Text Tools::objText;
sf::Font Tools::font_spacemono_regular;

Tools::Tools() {
}
void Tools::configure() {
    font_spacemono_regular.loadFromFile("./src/asset/fonts/SpaceMono-Regular.ttf");
    objText.setFont(font_spacemono_regular);
}
int Tools::getStartSprite(int index, int direction) {
    if (direction == -1)
        return index + 1;

    return index;
}
float Tools::ceil_special(float num, float biggerthan) {
    const int int_num = num;
    const float dec = (num - int_num);
    if (dec >= biggerthan) {
        return int_num + 1;
    }
    return num;
}

float Tools::floor_special(float num, float lessthan) {
    const int int_num = num;
    const float dec = (num - int_num);
    if (dec <= lessthan) {
        return int_num - 1;
    }
    return num;
}

std::string Tools::addZero(int x) {
    if (x < 10) return "0" + std::to_string(x);
    return std::to_string(x);
}

std::string Tools::seconds_to_hour(int seconds) {
    int hours, minutes, remainingSeconds;

    hours = seconds / 3600;  // 1 hour = 3600 seconds
    remainingSeconds = seconds % 3600;
    minutes = remainingSeconds / 60;  // 1 minute = 60 seconds
    remainingSeconds = remainingSeconds % 60;

    return addZero(hours) + ":" + addZero(minutes) + ":" + addZero(remainingSeconds);
}

int Tools::time_dif_in_seconds(std::chrono::system_clock::time_point start, std::chrono::system_clock::time_point end) {
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    return duration.count();
}

int Tools::wordOccurrence(std::string sentence, std::string word) {
    int slen = sentence.length();
    int wordlen = word.length();
    int count = 0;
    int i, j;
    for (i = 0; i < slen; i++) {
        for (j = 0; j < wordlen; j++) {
            if (sentence[i + j] != word[j])
                break;
        }
        if (j == wordlen) {
            count++;
        }
    }
    return count;
}
bool Tools::hasString(std::string line, std::string str) {
    int i_string = (int)line.find(str);
    if (i_string > 999)
        return false;
    return i_string > -1;
}

std::string Tools::get_lines_from_dtm(std::string filename, std::string tag, std::string endLine) {
    bool recording = tag.empty() || tag == "[]";
    std::string chunk = "";
    std::string line;

    std::ifstream map_file(filename);

    while (getline(map_file, line)) {
        if (recording && hasString(line, "[ENDBLOCK]")) {
            break;
        } else if (!recording && hasString(line, tag)) {
            recording = true;
            continue;
        }
        if (recording) {
            chunk += line + endLine;
        }
    }
    map_file.close();

    return chunk;
}

void Tools::say(sf::RenderWindow* w, std::string text, int left, int top, int fontSize, sf::Color color) {
    objText.setCharacterSize(fontSize);
    objText.setFillColor(color);
    objText.setString(text);
    objText.setPosition(sf::Vector2f(left, top));
    w->draw(objText);
}

double Tools::getRand() {
    static std::random_device rd;                     // Obtém uma semente aleatória do hardware
    static std::mt19937 gen(rd());                    // Inicializa o gerador Mersenne Twister com a semente
    std::uniform_real_distribution<> dis(-1.0, 1.0);  // Define a distribuição entre -1 e 1
    // Gera o número aleatório entre -1 e 1
    return dis(gen);
}

void Tools::printHour() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);
    std::cout << "Hora atual: " << std::put_time(localTime, "%H:%M:%S") << std::endl;
}

std::string Tools::formatDouble(double value, int decimalPlaces) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimalPlaces) << value;
    return oss.str();
}
sf::Color Tools::hslaToRgba(float h, float s, float l, float a) {
    float c = (1 - std::abs(2 * l - 1)) * s;
    float x = c * (1 - std::abs(std::fmod(h / 60.0, 2) - 1));
    float m = l - c / 2;

    float r = 0, g = 0, b = 0;
    if (0 <= h && h < 60) {
        r = c;
        g = x;
        b = 0;
    } else if (60 <= h && h < 120) {
        r = x;
        g = c;
        b = 0;
    } else if (120 <= h && h < 180) {
        r = 0;
        g = c;
        b = x;
    } else if (180 <= h && h < 240) {
        r = 0;
        g = x;
        b = c;
    } else if (240 <= h && h < 300) {
        r = x;
        g = 0;
        b = c;
    } else if (300 <= h && h < 360) {
        r = c;
        g = 0;
        b = x;
    }

    r = (r + m) * 255;
    g = (g + m) * 255;
    b = (b + m) * 255;

    return sf::Color(static_cast<sf::Uint8>(r), static_cast<sf::Uint8>(g), static_cast<sf::Uint8>(b), static_cast<sf::Uint8>(a * 255));
}
float Tools::map(float x, float a, float b, float c, float d) {
    return c + (x - a) * (d - c) / (b - a);
}