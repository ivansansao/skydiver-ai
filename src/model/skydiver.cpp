#include "skydiver.hpp"

#include <cmath>

#include "iostream"
#include "tools.hpp"

using namespace std;

// Altura Salto de paraquedismo recreativo: A altura mais comum é entre 3.000 a 4.000 metros
// A 160 km/h, leva aproximadamente 67,5 segundos para percorrer 3.000 metros
// A 160 km/h, em 10 segundos, você percorreria aproximadamente 444,44 metros
// Durante a queda livre, um paraquedista pode atingir uma velocidade mínima de 160 km/h em posição de "tracking".
// Em posição de "mergulho", a velocidade máxima durante a queda livre pode chegar a 320 km/h.
// Com o paraquedas aberto, a velocidade de descida típica é reduzida para 15 a 30 km/h.
// Com paraquedas de alta performance, a velocidade máxima durante a descida pode chegar a 50 km/h.

Skydiver::Skydiver() {
    skydiverFall.init(3, 0.5f, "./src/asset/image/skydiver_fall.png", sf::IntRect(0, 0, 8, 12), true);
    start_pos = sf::FloatRect(672.f, 64.f, 64.f, 64.f);
    abs_pos = pos;
    velocity = sf::Vector2f(0.f, 0.f);  // Inicial pode ser 1.8 = 180 km/h
    on_ground = false;
    reset_position();
}

void Skydiver::add_gravity() {
    velocity.y += 1;
    pos.top += velocity.y;
}
void Skydiver::set_position(float left, float top) {
    pos.left = left;
    pos.top = top;
    start_pos.left = left;
    start_pos.top = top;
}
void Skydiver::reset_position() {
    this->pos = sf::FloatRect(start_pos.left, start_pos.top, 8.f, 12.f);
}
void Skydiver::update() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Joystick::isButtonPressed(0, sf::Joystick::X)) {
        if (space_released) {
            space_released = false;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        if (lcontroll_released) {
            lcontroll_released = false;
        }
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad0)) {
        if (zerokey_released) {
            zerokey_released = false;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocity.x += 0.01;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocity.x -= 0.01;
    }

    if (velocity.x > 1.80) velocity.x = 1.80;    // 180 km/h
    if (velocity.x < -1.80) velocity.x = -1.80;  // 180 km/h

    const float gravity = 0.0035;
    velocity.y += gravity;

    if (velocity.y > 1.6) velocity.y = 1.6;  // 160 km/h de velocidade com os braços de pernas abertas.

    pos.left += velocity.x;
    pos.top += velocity.y;

    if (pos.top > 700) {
        pos.top = start_pos.top;
        velocity.y = 0.0;
    }
}

void Skydiver::draw(sf::RenderWindow *w) {
    skydiverFall.draw(pos.left, pos.top, w);

    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(pos.width, pos.height));
    rectangle.setFillColor(sf::Color(0, 0, 0, 0));
    rectangle.setOutlineColor(sf::Color::Red);
    rectangle.setOutlineThickness(2.f);
    rectangle.setPosition(sf::Vector2f(pos.left, pos.top));
    // w->draw(rectangle);

    int offset = 30;

    Tools::say(w, "Velocidade horizontal: " + to_string(this->velocity.x), pos.left + 24, pos.top - offset + (15 * 0));
    Tools::say(w, "Velocidade vertical: " + to_string(this->velocity.y), pos.left + 24, pos.top - offset + (15 * 1));
}
