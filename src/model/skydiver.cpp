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
    skydiverOnPlane.init(1, 0.5f, "./src/asset/image/skydiver_on_plane.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop, false);
    skydiverFall.init(3, 0.5f, "./src/asset/image/skydiver_fall.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop, false);
    skydiverParaOpening00.init(1, 0.5f, "./src/asset/image/skydiver_parachutes_opening00.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop, false);
    skydiverParaOpening50.init(1, 0.5f, "./src/asset/image/skydiver_parachutes_opening50.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop, false);
    skydiverParaCenter.init(3, 0.5f, "./src/asset/image/skydiver_parachutes_flying_center.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop, false);
    skydiverParaDiedWater.init(3, 0.15f, "./src/asset/image/skydiver_parachutes_died_on_water.png", sf::IntRect(0, 0, 43, 64), false, moveLeft, moveTop, false);
    skydiverDiedWater.init(2, 0.01f, "./src/asset/image/skydiver_died_on_water.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop, false);
    skydiverDiedBoat.init(1, 0, "./src/asset/image/skydiver_died_on_boat.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop, false);
    skydiverParaBoatCenter.init(5, 0.1f, "./src/asset/image/skydiver_parachutes_landing_on_boat_center.png", sf::IntRect(0, 0, 43, 64), true, moveLeft, moveTop, true);

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
    skydiverParaBoatCenter.setColor(color);
    reset_position();

    // NeuralNetwork* mind = new NeuralNetwork(3);

    // mind.addLayer(4, [](double x) { return 1.0 / (1.0 + std::exp(-x)); });
    mind.addLayer(8, [](double x) { return std::max(0.0, x); });
    mind.compile();
    // mind.mutate(100);
}
Skydiver::~Skydiver() {
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
    if (died) return;
    const float altitudeFromBoat = 1 / getAltitudeFromBoat(boat);
    const float landingPointDistanceH = 1 / getLandingPointDistanceH(boat);
    const float skydiverState = 1 / state;
    const float inpParachuteState = 1 / parachuteState;

    std::vector<double> input = {skydiverState, inpParachuteState, altitudeFromBoat, landingPointDistanceH, boat.velocity.x, velocity.x, velocity.y};
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

    // if (state == State::ON_PLANE) {
    //     mind.mutate(10);
    // }
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
void Skydiver::update(Plane plane, Boat boat) {
    // PLANE
    if (this->state == State::ON_PLANE) {
        pos.left = plane.pos.left + plane.door.x;
        pos.top = plane.pos.top + plane.door.y;
        velocity.x = plane.velocity.x;
        velocity.y = plane.velocity.y;
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

        if (state == State::ON_AIR) {
            // SAVE SCORES.
            if (velocity.x > grade_max_velocity_right) grade_max_velocity_right = velocity.x;
            if (velocity.x < grade_max_velocity_left) grade_max_velocity_left = velocity.x;

            if (timer - last_time_change_direction > 100) {
                const int direction = velocity.x < 0 ? -1 : 1;
                if (direction != last_direction) grade_direction_changes++;
                last_direction = direction;
                last_time_change_direction = timer;
            }
        }
    }

    if (touchedBoat(boat)) {
        state = State::ON_BOAT;
        setBoatTouchPlace(boat);
        if (this->isLand(boat)) {
            saveScoreLanding(boat);
        } else {
            diedPlace = DiedPlace::BOAT;
            died = true;
        }
    } else if (pos.top >= getGroundTop()) {
        // pos.top = start_pos.top;
        // pos.left = start_pos.left;
        // velocity.y = 0.0;
        parachutes_brake.reset();
        died = true;
        diedPlace = DiedPlace::WATER;
    } else {
        pos.left += velocity.x;
        pos.top += velocity.y;
    }

    if (state == State::ON_BOAT) {
        pos.left = boat.pos.left + boatTouchPlaceLeft;
        pos.top = boat.pos.top - pos.height - 1;
    }
}
float Skydiver::getAltitudeFromBoat(Boat boat) {
    const float myFeetTop = pos.top + pos.height;
    return boat.getLandingPointTop() - myFeetTop;
}
float Skydiver::getGroundTop() {
    return 790;
}
float Skydiver::getLandingPointDistanceH(Boat boat) {
    const float myMidLeft = pos.left + (pos.width / 2);
    return myMidLeft - boat.getLandingPointLeft();
}

void Skydiver::draw(sf::RenderWindow* w, Boat boat) {
    if (died) {
        if (diedPlace == DiedPlace::WATER) {
            if (parachuteState == ParachutesState::OPEN) {
                skydiverParaDiedWater.draw(pos.left, pos.top, w);
                skydiverParaDiedWater.animeAuto();
            } else {
                skydiverDiedWater.draw(pos.left, pos.top, w);
                skydiverDiedWater.animeAuto();
            }
        } else if (diedPlace == DiedPlace::BOAT) {
            skydiverDiedBoat.draw(pos.left, pos.top, w);
        }
    } else if (state == State::ON_PLANE) {
        skydiverOnPlane.draw(pos.left, pos.top, w);
    } else if (state == State::ON_BOAT) {
        skydiverParaBoatCenter.draw(pos.left, pos.top, w);
        skydiverParaBoatCenter.animeAuto();
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

    if (false) {
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
    }
    if (0 && !died) {
        std::vector<std::string> messages = {
            // "Velocidade horizontal: " + to_string(this->velocity.x) + " ps:" + to_string(parachuteState),
            // "Velocidade vertical: " + to_string(this->velocity.y),
            // "Parachutes brake: " + to_string(parachutes_brake.value),
            // "pos - left:" + to_string(pos.left) + ". top: " + to_string(pos.top),
            // "------",
            // "GPS",
            // "1 Altitude (boat) .....: " + to_string(getAltitudeFromBoat(boat)),
            // "2 Boat velocity .......: " + to_string(boat.velocity.x),
            // "3 Landing point (H): ..: " + to_string(getLandingPointDistanceH(boat)),
            "4 Horizontal velocity .: " + to_string(velocity.x),
            "5 Vertical velocity ...: " + to_string(velocity.y),
            // "6 Place ...............: " + to_string(state),
            // "7 Parachutes ..........: " + to_string(parachuteState)},
            "SCORE  " + to_string(getScore()),
            "Grade - Landing place ..: " + to_string(grade_landing_place),
            "Grade - Landing softly..: " + to_string(grade_landing_softly),
            "Grade - Max vel right...: " + to_string(grade_max_velocity_right),
            "Grade - Max vel left....: " + to_string(grade_max_velocity_left),
            "Grade - Direc changes...: " + to_string(grade_direction_changes)};

        for (int i = 0; i < (int)messages.size(); ++i) {
            Tools::say(w, messages[i], pos.left + 24, pos.top - 30 + (15 * i));
        }
    }

    timer++;
}

bool Skydiver::isLand(Boat boat) {
    if (parachuteState == ParachutesState::OPEN) {
        // If it touch speedly, consider died
        // if (velocity.x < 0.20 && std::abs(velocity.y) < 0.10) {
        return true;
        // }
    }
    return false;
}

bool Skydiver::touchedBoat(Boat boat) {
    if (state == State::ON_BOAT) {
        return false;  // One time only
    } else {
        float footLeft = pos.left + (pos.width / 2);
        float footTop = pos.top + pos.height;

        if (footLeft > boat.pos.left && footLeft < boat.pos.left + boat.pos.width) {        // It is in the boat area
            if (footTop >= boat.pos.top - 1 && footTop < boat.pos.top + boat.pos.height) {  // Touched ground
                return true;
            }
        }
    }
    return false;
}

void Skydiver::setBoatTouchPlace(Boat boat) {
    boatTouchPlaceLeft = pos.left - boat.pos.left;
}
int Skydiver::getScore() {
    return grade_landing_softly + grade_landing_place + grade_max_velocity_right + grade_max_velocity_left + grade_direction_changes;
}
void Skydiver::saveScoreLanding(Boat boat) {
    // Landing velocity - Heigher is better.
    const int max_velocity = max_slide_speed + max_fall_speed;
    grade_landing_softly = (max_velocity * 100) - (std::abs(velocity.x * 100) + std::abs(velocity.y * 100));

    // Place - How much near center higher
    int landingLength = std::abs(boat.getLandingPointLeft() - boat.pos.left);
    float footLeft = pos.left + (pos.width / 2);
    grade_landing_place = landingLength - std::abs(boat.getLandingPointLeft() - footLeft);

    grade_max_velocity_right = (int)std::abs(grade_max_velocity_right * 100);
    grade_max_velocity_left = (int)std::abs(grade_max_velocity_left * 100);

    // Means 10 points each change
    grade_direction_changes = grade_direction_changes * 10;
}