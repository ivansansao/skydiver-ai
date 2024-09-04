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
// Velocidade do avião, 150km/h

Skydiver::Skydiver() {
    const float moveLeft = -17;
    const float moveTop = -52;
    skydiverFall.init(3, 0.5f, "./src/asset/image/skydiver_fall.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop);
    skydiverParaOpening00.init(1, 0.5f, "./src/asset/image/skydiver_parachutes_opening00.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop);
    skydiverParaOpening50.init(1, 0.5f, "./src/asset/image/skydiver_parachutes_opening50.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop);
    skydiverParaCenter.init(3, 0.5f, "./src/asset/image/skydiver_parachutes_flying_center.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop);
    // start_pos = sf::FloatRect(800.f, 64.f, 64.f, 64.f);
    start_pos = sf::FloatRect(1700.f, 64.f, 64.f, 64.f);
    abs_pos = pos;
    on_ground = false;

    // sf::Color color = sf::Color::Magenta;
    sf::Color color = skydiverFall.setRandomColor();
    skydiverFall.setColor(color);
    skydiverParaOpening00.setColor(color);
    skydiverParaOpening50.setColor(color);
    skydiverParaCenter.setColor(color);
    reset_position();

    // NeuralNetwork* mind = new NeuralNetwork(3);

    // mind->addLayer(4, [](double x) { return 1.0 / (1.0 + std::exp(-x)); });
    mind->addLayer(6, [](double x) { return 1.0 / (1.0 + std::exp(-x)); });
    mind->compile();
    mind->mutate(4 * 6);
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
void Skydiver::think(Plane plane) {
    const float inpState = state;                                  // On the air?
    const float inpParachuteState = parachuteState;                // Is parachutes flying?
    const float inpPosLeftMid = 1 / (pos.left + (pos.width / 2));  // Pos mid os skydiver
    const float inpPosBotton = 1 / (pos.top + pos.height);         // Foot position os skydiver
    const float inpVelocityX = 1 / velocity.x;                     // Pos x of skydiver
    const float inpVelocityY = 1 / velocity.y;                     // Pos y of skydiver

    std::vector<double> input = {inpState, inpParachuteState, inpPosLeftMid, inpPosBotton, inpVelocityX, inpVelocityY};
    std::vector<double> output = mind->think(input);

    // std::cout << "Output -> output: " << output[0];
    // std::cout << std::endl;
    // for (const auto &value : output) {
    //     std::cout << value << " ";
    // }
    // std::cout << std::endl;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::T)) {
        jump();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
        mind->mutate(4 * 6);
        // std::cout << "Mutated: " << std::endl;
    }

    if (output[0] > 0.5) {
        jump();
        // std::cout << "jump" << std::endl;
    } else if (output[1] > 0.5) {
        parachutesOpen();
        // std::cout << "parachutesOpen" << std::endl;
    } else if (output[2] > 0.5) {
        parachutesGoRight();
        // std::cout << "parachutesGoRight" << std::endl;
    } else if (output[3] > 0.5) {
        parachutesGoLeft();
        // std::cout << "parachutesGoLeft" << std::endl;
    } else if (output[4] > 0.5) {
        parachutesGoUp();
        // std::cout << "parachutesGoUp" << std::endl;
    } else if (output[5] > 0.5) {
        parachutesGoDown();
        // std::cout << "parachutesGoDown" << std::endl;
    }
}
void Skydiver::jump() {
    if (state == State::ON_PLANE) {
        if (pos.left > 0 && pos.left + pos.width < 1600) {
            state = State::ON_AIR;
        }
    }
}
void Skydiver::parachutesOpen() {
    if (parachuteState == ParachutesState::CLOSED) {
        if (parachuteState != ParachutesState::OPENING) {
            parachuteState = ParachutesState::OPENING;
        }
    }
}
void Skydiver::parachutesGoRight() {
    if (parachuteState == ParachutesState::FLYING) {
        velocity.x += 0.01;
    }
}
void Skydiver::parachutesGoLeft() {
    if (parachuteState == ParachutesState::FLYING) {
        velocity.x -= 0.01;
    }
}
void Skydiver::parachutesGoUp() {
    if (parachuteState == ParachutesState::FLYING) {
        parachutes_brake.increase();
    }
}
void Skydiver::parachutesGoDown() {
    if (parachuteState == ParachutesState::FLYING) {
        parachutes_brake.decrease();
    }
}
void Skydiver::update(Plane plane) {
    // PLANE
    if (this->state == State::ON_PLANE) {
        pos.left = plane.pos.left + plane.door.x;
        pos.top = plane.pos.top + plane.door.y;
        velocity.x = plane.velocity.x;
        velocity.y = plane.velocity.y;
        return;
    }

    // SKYDIVER
    if (parachuteState == ParachutesState::FLYING) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            parachutesGoRight();
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            parachutesGoLeft();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            parachutesGoUp();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            parachutesGoDown();
        }

    } else if (parachuteState == ParachutesState::CLOSED) {
        if (this->state == State::ON_AIR) {
            velocity.x = velocity.x * 0.9974;  // Consider the wind!
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        parachutesOpen();
    }

    if (velocity.x > max_slide_speed) velocity.x = max_slide_speed;    // 180 km/h
    if (velocity.x < -max_slide_speed) velocity.x = -max_slide_speed;  // 180 km/h

    velocity.y += gravity;

    if (velocity.y > max_fall_speed) velocity.y = max_fall_speed;  // 160 km/h de velocidade com os braços de pernas abertas.

    // Opening parachutes.
    if (parachuteState == ParachutesState::OPENING) {
        if (velocity.y > max_opened_parachutes_fall_speed) {
            velocity.y -= parachutes_fall_ratio_brake;
        }
        if (velocity.y <= max_opened_parachutes_fall_speed) {
            parachuteState = ParachutesState::FLYING;
        }
    }

    // Flying parachutes.
    if (parachuteState == ParachutesState::FLYING) {
        if (velocity.y > max_opened_parachutes_fall_speed - parachutes_brake.value) {
            velocity.y = max_opened_parachutes_fall_speed - parachutes_brake.value;
        }
        if (velocity.y < min_opened_parachutes_fall_speed) {
            velocity.y = min_opened_parachutes_fall_speed;
        }
    }

    pos.left += velocity.x;
    pos.top += velocity.y;

    if (pos.top > 790) {
        // pos.top = start_pos.top;
        // pos.left = start_pos.left;
        // velocity.y = 0.0;
        parachuteState = ParachutesState::CLOSED;
        parachutes_brake.reset();
        state = State::ON_PLANE;
        mind->mutate(400);
    }
}

void Skydiver::draw(sf::RenderWindow* w) {
    if (parachuteState == ParachutesState::CLOSED) {
        skydiverFall.draw(pos.left, pos.top, w);
    } else if (parachuteState == ParachutesState::OPENING) {
        if (velocity.y > max_opened_parachutes_fall_speed * 2) {
            skydiverParaOpening00.draw(pos.left, pos.top, w);
        } else {
            skydiverParaOpening50.draw(pos.left, pos.top, w);
        }
    } else if (parachuteState == ParachutesState::FLYING) {
        skydiverParaCenter.draw(pos.left, pos.top, w);
    } else {
        skydiverFall.draw(pos.left, pos.top, w);
    }

    if (pos.left < 0 || pos.left > 1600 + pos.width) {
        sf::CircleShape circle;
        float radius = std::min(pos.width, pos.height) / 2.0f;
        circle.setRadius(radius);
        circle.setFillColor(sf::Color(0, 0, 0, 0));
        circle.setOutlineColor(sf::Color::Red);
        circle.setOutlineThickness(2.f);
        if (pos.left < 0) circle.setPosition(sf::Vector2f(0 + radius, pos.top + radius));
        if (pos.left > 1600 + pos.width) circle.setPosition(sf::Vector2f(1600 - pos.width - radius, pos.top + radius));
        w->draw(circle);
    }

    if (0) {
        sf::RectangleShape rectangle;
        rectangle.setSize(sf::Vector2f(pos.width, pos.height));
        rectangle.setFillColor(sf::Color(0, 0, 0, 0));
        rectangle.setOutlineColor(sf::Color::Red);
        rectangle.setOutlineThickness(2.f);
        rectangle.setPosition(sf::Vector2f(pos.left, pos.top));
        w->draw(rectangle);

        int offset = 30;
        Tools::say(w, "Velocidade horizontal: " + to_string(this->velocity.x) + " ps:" + to_string(parachuteState), pos.left + 24, pos.top - offset + (15 * 0));
        Tools::say(w, "Velocidade vertical: " + to_string(this->velocity.y), pos.left + 24, pos.top - offset + (15 * 1));
        Tools::say(w, "Parachutes brake: " + to_string(parachutes_brake.value), pos.left + 24, pos.top - offset + (15 * 2));
        Tools::say(w, "pos - left:" + to_string(pos.left) + ". top: " + to_string(pos.top), pos.left + 24, pos.top - offset + (15 * 3));
    }
}
