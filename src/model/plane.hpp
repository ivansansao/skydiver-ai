#pragma once

#ifndef PLANE_H
#define PLANE_H
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "animation.hpp"
#include "force.hpp"
#include "tools.hpp"

class Plane {
   public:
    Plane();
    Animation plane;

    sf::FloatRect start_pos;
    sf::FloatRect pos;
    sf::FloatRect abs_pos;
    sf::Vector2f velocity;
    sf::Vector2f door;

    unsigned int round = 1;
    void draw(sf::RenderWindow *w);
    void update();
    void set_position(float left, float top);
    void reset_position();
    void start_round();
    void reverse_direction(bool condition);
    void set_start_pos();
    bool on = true;
};
#endif
