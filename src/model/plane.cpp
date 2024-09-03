#include "plane.hpp"

#include <cmath>

#include "iostream"

using namespace std;

Plane::Plane() {
    plane.init(2, 0.01f, "./src/asset/image/plane.png", sf::IntRect(0, 0, 152, 47), true, 0, 0);

    start_pos = sf::FloatRect(1600, 50, 152, 47);
    abs_pos = pos;
    velocity = sf::Vector2f(-1.5f, 0.f);  // 150 km/h
    door = sf::Vector2f(90, 28);

    reset_position();
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
    if (pos.left + pos.width < 0) {
        reset_position();
    }
}

void Plane::draw(sf::RenderWindow *w) {
    plane.draw(pos.left, pos.top, w);
    plane.anime(sf::IntRect(plane.getFrame() * pos.width, 0, pos.width, pos.height), 1);
}
