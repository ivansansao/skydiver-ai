#pragma once

#ifndef SKYDIVER_H
#define SKYDIVER_H
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "animation.hpp"

class Skydiver {
   public:
    Skydiver();
    Animation skydiverFall;
    Animation skydiverParaLeft;
    Animation skydiverParaCenter;
    Animation skydiverParaRight;

    sf::RenderWindow window;
    sf::FloatRect start_pos;
    sf::FloatRect pos;

    sf::FloatRect abs_pos;
    sf::Vector2f velocity;

    enum States { Normal
    } state = Normal;

    bool on_ground;

    bool key_released = true;
    bool up_released = true;
    bool z_released = true;
    bool space_released = true;
    bool lcontroll_released = true;
    bool rcontroll_released = true;
    bool zerokey_released = true;

    void draw(sf::RenderWindow *w);
    void update();
    void set_position(float left, float top);
    void reset_position();

    void add_gravity();
};
#endif
