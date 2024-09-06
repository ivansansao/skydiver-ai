#pragma once

#ifndef SKYDIVER_H
#define SKYDIVER_H
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "animation.hpp"
#include "boat.hpp"
#include "force.hpp"
#include "mynn/NeuralNetwork.hpp"
#include "plane.hpp"

class Skydiver {
   public:
    Skydiver();
    ~Skydiver();

    NeuralNetwork mind = NeuralNetwork(7);

    Animation skydiverFall;
    Animation skydiverOnPlane;

    Animation skydiverParaOpening00;
    Animation skydiverParaOpening50;
    Animation skydiverParaLeft;
    Animation skydiverParaRight;
    Animation skydiverParaCenter;
    Animation skydiverParaDiedWater;
    Animation skydiverDiedWater;
    Animation skydiverDiedBoat;
    Animation skydiverParaBoatCenter;

    sf::RenderWindow window;
    sf::FloatRect start_pos;
    sf::FloatRect pos;

    sf::FloatRect abs_pos;
    sf::Vector2f velocity = sf::Vector2f(0.f, 0.f);

    float boatTouchPlaceLeft = 0;

    const float max_slide_speed = 1.8;                    // 180 km/h
    const float max_fall_speed = 1.6;                     // 160 km/h
    const float max_opened_parachutes_fall_speed = 0.35;  // 35 km/h
    const float min_opened_parachutes_fall_speed = 0.08;  // 8 km/h
    const float parachutes_fall_ratio_brake = 0.03;
    const float gravity = 0.0035;

    const float parachutes_flying_ratio_brake = 0.005;

    Force parachutes_brake = Force(0.0, 0.27, 0.0, 0.001);  // value, max, min, ratio
    bool landed = false;
    bool died = false;

    uint grade_landing_softly = 0;
    uint grade_landing_place = 0;
    float grade_max_velocity_right = 0;
    float grade_max_velocity_left = 0;
    int grade_direction_changes = 0;
    int getScore();

    int timer = 0;
    int last_time_change_direction = 0;
    int last_direction = -1;

    enum State {
        ON_PLANE,
        ON_AIR,
        ON_BOAT,
        ON_WATER
    } state = ON_PLANE;

    enum DiedPlace {
        BOAT,
        WATER
    } diedPlace;

    enum ParachutesState {
        CLOSED,
        OPENING,
        OPEN,
    } parachuteState = CLOSED;

    enum Action {
        NOTHING,
        JUMP,
        PARACHUTES_OPEN,
        PARACHUTES_RIGHT,
        PARACHUTES_LEFT,
        PARACHUTES_UP,
        PARACHUTES_DOWN,
    } action = NOTHING;

    bool on_ground;

    bool key_released = true;
    bool up_released = true;
    bool z_released = true;
    bool space_released = true;
    bool lcontroll_released = true;
    bool rcontroll_released = true;
    bool zerokey_released = true;

    void draw(sf::RenderWindow* w, Boat boat);
    void update(Plane plane, Boat boat);
    void set_position(float left, float top);
    void reset_position();
    // Force wind_force_x = Force(0.997, 1, 0, 0.0000001);  // Value, max, min, ratio

    void parachutesOpen();
    void parachutesGoRight();
    void parachutesGoLeft();
    void parachutesGoUp();
    void parachutesGoDown();

    void add_gravity();
    void jump();
    void think(Plane plane, Boat boat);
    float getAltitudeFromBoat(Boat boat);
    float getGroundTop();
    float getLandingPointDistanceH(Boat boat);

    bool touchedBoat(Boat boat);
    bool isLand(Boat boat);
    void setBoatTouchPlace(Boat boat);
    void saveScoreLanding(Boat boat);
};
#endif
