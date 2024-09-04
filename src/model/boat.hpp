#pragma once

#ifndef BOAT_H
#define BOAT_H
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "animation.hpp"

class Boat {
   public:
    Boat();
    Animation boat;

    sf::FloatRect start_pos;
    sf::FloatRect pos;
    sf::FloatRect abs_pos;
    sf::Vector2f velocity;

    void draw(sf::RenderWindow *w);
    void update();
    void set_position(float left, float top);
    void reset_position();
    float getLandingPointLeft();
    float getLandingPointTop();
    float getLandingPointRadius();
};
#endif
