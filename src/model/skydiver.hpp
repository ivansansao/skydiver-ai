#pragma once

#ifndef SKYDIVER_H
#define SKYDIVER_H
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "animation.hpp"
#include "force.hpp"

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

    const float max_slide_speed = 1.8;                    // 180 km/h
    const float max_fall_speed = 1.6;                     // 160 km/h
    const float max_opened_parachutes_fall_speed = 0.35;  // 35 km/h
    const float min_opened_parachutes_fall_speed = 0.08;  // 8 km/h
    const float parachutes_fall_ratio_brake = 0.03;
    const float gravity = 0.0035;

    const float parachutes_flying_ratio_brake = 0.005;

    Force parachutes_brake = Force(0.0, 0.27, 0.0, 0.001);  // value, max, min, ratio

    enum ParachutesState {
        CLOSED,
        OPENING,
        FLYING,
        OPEN_ON_FLOOR,
    } parachuteState = CLOSED;

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
