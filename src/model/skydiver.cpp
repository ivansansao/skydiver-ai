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
    skydiverOnPlane.init(1, 0.5f, "./src/asset/image/skydiver_on_plane.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop);
    skydiverFall.init(3, 0.5f, "./src/asset/image/skydiver_fall.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop);
    skydiverParaOpening00.init(1, 0.5f, "./src/asset/image/skydiver_parachutes_opening00.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop);
    skydiverParaOpening50.init(1, 0.5f, "./src/asset/image/skydiver_parachutes_opening50.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop);
    skydiverParaCenter.init(3, 0.5f, "./src/asset/image/skydiver_parachutes_flying_center.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop);
    skydiverParaDiedWater.init(1, 0.5f, "./src/asset/image/skydiver_parachutes_died_on_water.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop);
    skydiverDiedWater.init(1, 0.5f, "./src/asset/image/skydiver_died_on_water.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop);
    // start_pos = sf::FloatRect(800.f, 64.f, 64.f, 64.f);
    start_pos = sf::FloatRect(1700.f, 64.f, 64.f, 64.f);
    abs_pos = pos;
    on_ground = false;

    // sf::Color color = sf::Color::Magenta;
    sf::Color color = skydiverOnPlane.setRandomColor();
    skydiverFall.setColor(color);
    skydiverParaOpening00.setColor(color);
    skydiverParaOpening50.setColor(color);
    skydiverParaCenter.setColor(color);
    skydiverParaDiedWater.setColor(color);
    reset_position();

    // NeuralNetwork* mind = new NeuralNetwork(3);

    // mind.addLayer(4, [](double x) { return 1.0 / (1.0 + std::exp(-x)); });
    mind.addLayer(8, [](double x) { return std::max(0.0, x); });
    mind.compile();
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
void Skydiver::think(Plane plane, Boat boat) {
    const float inpState = 1 / state;                                // On the air?
    const float inpParachuteState = 1 / parachuteState;              // Is parachutes flying?
    const float inpPosLeftMid = 1 / (pos.left + (pos.width * 0.5));  // Pos mid os skydiver
    const float inpPosBotton = 1 / (pos.top + pos.height);           // Foot position os skydiver
    const float inpVelocityX = velocity.x;                           // Pos x of skydiver
    const float inpVelocityY = velocity.y;                           // Pos y of skydiver
    const float landingPointLeft = 1 / boat.getLandingPointLeft();   // Center point of landing
    const float landingPointTop = 1 / boat.getLandingPointTop();     // Top point of landing

    std::vector<double> input = {inpState, inpParachuteState, inpPosLeftMid, inpPosBotton, inpVelocityX, inpVelocityY, landingPointLeft, landingPointTop};
    std::vector<double> output = mind.think(input);

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
        mind.mutate(4 * 6);
        // std::cout << "Mutated: " << std::endl;
    }

    action = Action::NOTHING;

    if (output[0] > 0.5) {
        jump();
        // std::cout << "jump" << std::endl;
        action = Action::JUMP;
    } else if (output[1] > 0.5) {
        parachutesOpen();
        // std::cout << "parachutesOpen" << std::endl;
        action = Action::PARACHUTES_OPEN;
    } else if (output[2] > 0.5) {
        parachutesGoRight();
        // std::cout << "parachutesGoRight " << skydiverFall.sprite.getColor().toInteger() << std::endl;
        action = Action::PARACHUTES_RIGHT;
    } else if (output[3] > 0.5) {
        parachutesGoLeft();
        // std::cout << "parachutesGoLeft" << std::endl;
        action = Action::PARACHUTES_LEFT;
    } else if (output[4] > 0.5) {
        parachutesGoUp();
        // std::cout << "parachutesGoUp" << std::endl;
        action = Action::PARACHUTES_UP;
    } else if (output[5] > 0.5) {
        parachutesGoDown();
        // std::cout << "parachutesGoDown" << std::endl;
        action = Action::PARACHUTES_DOWN;
    }

    if (state == State::ON_PLANE) {
        mind.mutate(10);
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
    if (state == State::ON_AIR) {
        if (parachuteState == ParachutesState::CLOSED) {
            parachuteState = ParachutesState::OPENING;
        }
    }
}
void Skydiver::parachutesGoRight() {
    if (parachuteState == ParachutesState::OPEN) {
        velocity.x += 0.01;
    }
}
void Skydiver::parachutesGoLeft() {
    if (parachuteState == ParachutesState::OPEN) {
        velocity.x -= 0.01;
    }
}
void Skydiver::parachutesGoUp() {
    if (parachuteState == ParachutesState::OPEN) {
        parachutes_brake.increase();
    }
}
void Skydiver::parachutesGoDown() {
    if (parachuteState == ParachutesState::OPEN) {
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
    } else if (this->state == State::DIED) {
        return;
    }

    // SKYDIVER
    if (parachuteState == ParachutesState::OPEN) {
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
            parachuteState = ParachutesState::OPEN;
        }
    }

    // Flying parachutes.
    if (parachuteState == ParachutesState::OPEN) {
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
        parachutes_brake.reset();
        state = State::DIED;
        diedPlace = DiedPlace::WATER;
    }
}

void Skydiver::draw(sf::RenderWindow* w) {
    if (state == State::DIED) {
        if (diedPlace == DiedPlace::WATER) {
            if (parachuteState == ParachutesState::OPEN) {
                skydiverParaDiedWater.draw(pos.left, pos.top, w);
            } else {
                skydiverDiedWater.draw(pos.left, pos.top, w);
            }
        }
    } else if (state == State::ON_PLANE) {
        skydiverOnPlane.draw(pos.left, pos.top, w);
    } else {
        if (parachuteState == ParachutesState::CLOSED) {
            skydiverFall.draw(pos.left, pos.top, w);
        } else if (parachuteState == ParachutesState::OPENING) {
            if (velocity.y > max_opened_parachutes_fall_speed * 2) {
                skydiverParaOpening00.draw(pos.left, pos.top, w);
            } else {
                skydiverParaOpening50.draw(pos.left, pos.top, w);
            }
        } else if (parachuteState == ParachutesState::OPEN) {
            skydiverParaCenter.draw(pos.left, pos.top, w);
        } else {
            skydiverFall.draw(pos.left, pos.top, w);
        }
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

    if (true) {
        if (state == State::ON_AIR) {
            switch (action) {
                case Action::PARACHUTES_LEFT:
                    Tools::say(w, "L", pos.left - 4, pos.top - 4);
                    break;
                case Action::PARACHUTES_RIGHT:
                    Tools::say(w, "R", pos.left + 10, pos.top - 4);
                    break;
                case Action::PARACHUTES_UP:
                    Tools::say(w, "U", pos.left + 1, pos.top - 26);
                    break;
                case Action::PARACHUTES_DOWN:
                    Tools::say(w, "D", pos.left + 1, pos.top + 8);
                    break;
                case Action::PARACHUTES_OPEN:
                    Tools::say(w, "O", pos.left + 1, pos.top + 12);
                    break;
                case Action::JUMP:
                    Tools::say(w, "J", pos.left + 1, pos.top + 16);
                    break;
                default:
                    // Tools::say(w, to_string(action), pos.left, pos.top + 8);
                    break;
            }
        }
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
