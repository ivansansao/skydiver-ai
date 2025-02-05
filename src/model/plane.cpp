#include "plane.hpp"

#include <cmath>

#include "iostream"

using namespace std;

Plane::Plane() {
    plane.init(2, 0.01f, "./src/asset/image/plane.png", sf::IntRect(0, 0, 152, 47), true, 0, 0, false);

    start_pos = sf::FloatRect(1600, 50, 152, 47);
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
        start_pos = sf::FloatRect(-152, 50, 152, 47);
    } else {
        start_pos = sf::FloatRect(1600, 50, 152, 47);
    }
}

void Plane::set_position(float left, float top) {
    pos.left = left;
    pos.top = top;
    start_pos.left = left;
    start_pos.top = top;
}
void Plane::reset_position() {
    this->pos = sf::FloatRect(start_pos.left, start_pos.top, 152, 47);
}
void Plane::update() {
    pos.left += velocity.x;

    // Going to right
    if (velocity.x > 0) {
        if (pos.left > 1600) {
            round++;
            this->on = false;
        }
    } else {  // Going to left
        if (pos.left + pos.width < 0) {
            round++;
            this->on = false;
        }
    }
}

void Plane::draw(sf::RenderWindow *w) {
    // actorJetpack.anime(sf::IntRect(Tools::getStartSprite(actorJetpack.getFrame(), direction_x) * pos.width, 0, direction_x * pos.width, pos.height), direction_x);
    const int dir_x = velocity.x > 0 ? 1 : -1;

    plane.draw(pos.left, pos.top, w);
    if (dir_x > 0) {
        plane.anime(sf::IntRect(pos.width * (plane.getFrame() + 1), 0, -pos.width, pos.height), dir_x);
    } else {
        plane.anime(sf::IntRect(pos.width * plane.getFrame(), 0, pos.width, pos.height), dir_x);
    }
}

void Plane::start_round() {
    on = true;
    round = 1;
    reset_position();
}