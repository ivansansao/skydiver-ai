#include "plane.hpp"

#include <cmath>

#include "iostream"

using namespace std;

Plane::Plane() {
    plane.init(2, 0.01f, "./src/asset/image/plane.png", sf::IntRect({0, 0}, {152, 47}), true, 0, 0, false);

    start_pos = sf::FloatRect({1600, 50}, {152, 47});
    abs_pos = pos;
    velocity = sf::Vector2f(-1.5f, 0.f);  // 150 km/h
    door = sf::Vector2f(90, 28);

    reset_position();
}

void Plane::reverse_direction(bool condition) {
    if (condition) {
        velocity.x = -velocity.x;
        set_start_pos();
    }
}

void Plane::set_start_pos() {
    if (velocity.x > 0) {
        start_pos = sf::FloatRect({-152, 50}, {152, 47});
    } else {
        start_pos = sf::FloatRect({1600, 50}, {152, 47});
    }
}

void Plane::set_position(float left, float top) {
    pos.position.x = left;
    pos.position.y = top;
    start_pos.position.x = left;
    start_pos.position.y = top;
}
void Plane::reset_position() {
    this->pos = sf::FloatRect({start_pos.position.x, start_pos.position.y}, {152, 47});
}
void Plane::update() {
    pos.position.x += velocity.x;

    // Going to right
    if (velocity.x > 0) {
        if (pos.position.x > 1600) {
            round++;
            this->on = false;
        }
    } else {  // Going to left
        if (pos.position.x + pos.size.x < 0) {
            round++;
            this->on = false;
        }
    }
}

void Plane::draw(sf::RenderWindow *w) {
    // actorJetpack.anime(sf::IntRect({Tools::getStartSprite(actorJetpack.getFrame(), direction_x) * pos.size.x}, {0, direction_x * pos.size.x, pos.size.y}), direction_x);
    const int dir_x = velocity.x > 0 ? 1 : -1;
    const int frameWidth = static_cast<int>(pos.size.x);
    const int frameHeight = static_cast<int>(pos.size.y);

    plane.draw(pos.position.x, pos.position.y, w);
    if (dir_x > 0) {
        plane.anime(sf::IntRect({frameWidth * (plane.getFrame() + 1), 0}, {-frameWidth, frameHeight}), dir_x);
    } else {
        plane.anime(sf::IntRect({frameWidth * plane.getFrame(), 0}, {frameWidth, frameHeight}), dir_x);
    }
}

void Plane::start_round() {
    on = true;
    round = 1;
    reset_position();
}
