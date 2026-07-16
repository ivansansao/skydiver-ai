#include "boat.hpp"

#include <cmath>

#include "iostream"

using namespace std;

Boat::Boat() {
    boat.init(2, 0.05f, "./src/asset/image/boat.png", sf::IntRect({0, 0}, {110, 40}), true, 0, -33, false);

    start_pos = sf::FloatRect({735, 783}, {106, 8});
    abs_pos = pos;
    velocity = sf::Vector2f(-0.1f, 0.f);  // 10 km/h

    reset_position();
    start_position_random();
}

void Boat::set_position(float left, float top) {
    pos.position.x = left;
    pos.position.y = top;
    start_pos.position.x = left;
    start_pos.position.y = top;
}
void Boat::reset_position() {
    this->pos = sf::FloatRect({start_pos.position.x, start_pos.position.y}, {start_pos.size.x, start_pos.size.y});
}
void Boat::start_position_random(sf::RenderWindow *window) {
    const float screenWidth = window ? window->getView().getSize().x : 1600.f;
    float maxLeft = screenWidth - pos.size.x;
    if (maxLeft < 0.f) {
        maxLeft = 0.f;
    }

    const float normalizedRand = (static_cast<float>(Tools::getRand()) + 1.f) * 0.5f;  // convert [-1,1] to [0,1]
    pos.position.x = normalizedRand * maxLeft;
    pos.position.x = std::floor(pos.position.x);
}
void Boat::update(sf::RenderWindow *window) {
    pos.position.x += velocity.x;

    const float screenLeft = 0.f;
    const float screenRight = window ? window->getView().getSize().x : 1600.f;

    // Clamp to the visible area and invert direction when touching the borders.
    if (pos.position.x < screenLeft) {
        pos.position.x = screenLeft;
        velocity.x = std::abs(velocity.x);
    } else if (pos.position.x + pos.size.x > screenRight) {
        pos.position.x = screenRight - pos.size.x;
        velocity.x = -std::abs(velocity.x);
    }
}

float Boat::getLandingPointLeft() {
    return pos.position.x + (pos.size.x * 0.5);
}
float Boat::getLandingPointTop() {
    return pos.position.y;
}
float Boat::getLandingPointRadius() {
    return pos.size.x;
}

void Boat::draw(sf::RenderWindow *w) {
    if (false) {
        sf::RectangleShape rectangle;
        rectangle.setSize(sf::Vector2f(pos.size.x, pos.size.y));
        rectangle.setFillColor(sf::Color(0, 0, 0, 0));
        rectangle.setOutlineColor(sf::Color::Red);
        rectangle.setOutlineThickness(1);
        rectangle.setPosition(sf::Vector2f(pos.position.x, pos.position.y));
        w->draw(rectangle);

        sf::RectangleShape centerPoint;
        centerPoint.setSize(sf::Vector2f(2, 2));
        centerPoint.setFillColor(sf::Color(0, 0, 0, 0));
        centerPoint.setOutlineColor(sf::Color::Red);
        centerPoint.setOutlineThickness(3);
        centerPoint.setPosition(sf::Vector2f(sf::Vector2f(getLandingPointLeft(), getLandingPointTop())));
        w->draw(centerPoint);
    }
    boat.draw(pos.position.x, pos.position.y, w);
    boat.anime(sf::IntRect({boat.getFrame() * 110, 0}, {110, 40}), 1);
}
