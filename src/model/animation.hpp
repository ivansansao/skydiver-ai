#pragma once

#ifndef ANIMATION_H
#define ANIMATION_H

#include <SFML/Graphics.hpp>
#include <random>

using namespace std;

class Animation {
   public:
    Animation();

    sf::Texture texture;
    sf::Sprite sprite;
    int direction_x = 1;
    int q_frame = 0;
    float i_frame = 0;
    float step = 0.f;
    bool circularSprite = true;  // true = 0,1,2,3 then 0,1,2,3 false = 0,1,2,3 then 3,2,1,0

    void anime(sf::IntRect rect, int direction_x);
    void next();
    void draw(float i, float j, sf::RenderWindow *w);
    void setTexture(std::string file);
    void setTextureRect(sf::IntRect rect);
    void setPosition(float i, float j);
    int getFrame();

    void init(int q_frame, float step, std::string file, sf::IntRect rect, bool circularSprite);
    sf::Color setRandomColor();
    void setColor(sf::Color color);
};
#endif
