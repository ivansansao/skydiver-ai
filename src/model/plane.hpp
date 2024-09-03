#pragma once

#ifndef PLANE_H
#define PLANE_H
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "animation.hpp"
#include "force.hpp"

class Plane {
   public:
    Plane();
    Animation plane;

    sf::FloatRect start_pos;
    sf::FloatRect pos;
    sf::FloatRect abs_pos;
    sf::Vector2f velocity;
    sf::Vector2f door;

    void draw(sf::RenderWindow *w);
    void update();
    void set_position(float left, float top);
    void reset_position();
};
#endif
