#include "boat.hpp"

#include <cmath>

#include "iostream"

using namespace std;

Boat::Boat() {
    boat.init(2, 0.05f, "./src/asset/image/boat.png", sf::IntRect(0, 0, 110, 40), true, 0, -33, false);

    start_pos = sf::FloatRect(735, 783, 106, 8);
    abs_pos = pos;
    velocity = sf::Vector2f(-0.1f, 0.f);  // 10 km/h

    reset_position();
}

void Boat::set_position(float left, float top) {
    pos.left = left;
    pos.top = top;
    start_pos.left = left;
    start_pos.top = top;
}
void Boat::reset_position() {
    this->pos = sf::FloatRect(start_pos.left, start_pos.top, start_pos.width, start_pos.height);
}
void Boat::update() {
    pos.left += velocity.x;
    if (pos.left + pos.width < 0) {
        reset_position();
    }
}

float Boat::getLandingPointLeft() {
    return pos.left + (pos.width * 0.5);
}
float Boat::getLandingPointTop() {
    return pos.top;
}
float Boat::getLandingPointRadius() {
    return pos.width;
}

void Boat::draw(sf::RenderWindow *w) {
    if (false) {
        sf::RectangleShape rectangle;
        rectangle.setSize(sf::Vector2f(pos.width, pos.height));
        rectangle.setFillColor(sf::Color(0, 0, 0, 0));
        rectangle.setOutlineColor(sf::Color::Red);
        rectangle.setOutlineThickness(1);
        rectangle.setPosition(sf::Vector2f(pos.left, pos.top));
        w->draw(rectangle);

        sf::RectangleShape centerPoint;
        centerPoint.setSize(sf::Vector2f(2, 2));
        centerPoint.setFillColor(sf::Color(0, 0, 0, 0));
        centerPoint.setOutlineColor(sf::Color::Red);
        centerPoint.setOutlineThickness(3);
        centerPoint.setPosition(sf::Vector2f(getLandingPointLeft(), getLandingPointTop()));
        w->draw(centerPoint);
    }
    boat.draw(pos.left, pos.top, w);
    boat.anime(sf::IntRect(boat.getFrame() * 110, 0, 110, 40), 1);
}
