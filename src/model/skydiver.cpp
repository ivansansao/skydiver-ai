#include "skydiver.hpp"

#include <cmath>

#include "game.hpp"
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
Skydiver::Skydiver() : id(0) {
}
Skydiver::Skydiver(uint16_t id, int qtd_skydivers, int hiddenLayers, int layersSize) : id(id), qtd_skydivers(qtd_skydivers), hiddenLayers(hiddenLayers), layersSize(layersSize) {
    this->id = id;
    this->qtd_skydivers = qtd_skydivers;
    this->hiddenLayers = hiddenLayers;
    this->layersSize = layersSize;
    const float moveLeft = -17;
    const float moveTop = -52;
    skydiverOnPlane.init(1, 0.5f, "./src/asset/image/skydiver_on_plane.png", sf::IntRect({0, 0}, {43, 64}), true, moveLeft, moveTop, false);
    skydiverFall.init(3, 0.5f, "./src/asset/image/skydiver_fall.png", sf::IntRect({0, 0}, {43, 64}), true, moveLeft, moveTop, false);
    skydiverParaOpening00.init(1, 0.5f, "./src/asset/image/skydiver_parachutes_opening00.png", sf::IntRect({0, 0}, {43, 64}), true, moveLeft, moveTop, false);
    skydiverParaOpening50.init(1, 0.5f, "./src/asset/image/skydiver_parachutes_opening50.png", sf::IntRect({0, 0}, {43, 64}), true, moveLeft, moveTop, false);

    skydiverParaCenter.init(3, 0.5f, "./src/asset/image/skydiver_parachutes_flying_center.png", sf::IntRect({0, 0}, {43, 64}), true, moveLeft, moveTop, false);
    skydiverParaLeft.init(3, 0.5f, "./src/asset/image/skydiver_parachutes_flying_left.png", sf::IntRect({0, 0}, {43, 64}), true, moveLeft, moveTop, false);
    skydiverParaRight.init(3, 0.5f, "./src/asset/image/skydiver_parachutes_flying_right.png", sf::IntRect({0, 0}, {43, 64}), true, moveLeft, moveTop, false);
    skydiverParaBrake.init(3, 0.5f, "./src/asset/image/skydiver_parachutes_flying_brake.png", sf::IntRect({0, 0}, {43, 64}), true, moveLeft, moveTop, false);
    skydiverParaDive.init(3, 0.5f, "./src/asset/image/skydiver_parachutes_flying_dive.png", sf::IntRect({0, 0}, {43, 64}), true, moveLeft, moveTop, false);

    skydiverParaDiedWater.init(3, 0.15f, "./src/asset/image/skydiver_parachutes_died_on_water.png", sf::IntRect({0, 0}, {43, 64}), false, moveLeft, moveTop, false);
    skydiverDiedWater.init(2, 0.01f, "./src/asset/image/skydiver_died_on_water.png", sf::IntRect({0, 0}, {43, 64}), true, moveLeft, moveTop, false);
    skydiverDiedBoat.init(1, 0, "./src/asset/image/skydiver_died_on_boat.png", sf::IntRect({0, 0}, {43, 64}), true, moveLeft, moveTop, false);
    skydiverParaBoatCenter.init(5, 0.1f, "./src/asset/image/skydiver_parachutes_landing_on_boat_center.png", sf::IntRect({0, 0}, {43, 64}), true, moveLeft, moveTop, true);

    start_pos = sf::FloatRect({1700.f, 64.f}, {64.f, 64.f});
    abs_pos = pos;

    sf::Color color = skydiverOnPlane.setRandomColor();
    skydiverFall.setColor(color);
    skydiverParaOpening00.setColor(color);
    skydiverParaOpening50.setColor(color);
    skydiverParaCenter.setColor(color);
    skydiverParaLeft.setColor(color);
    skydiverParaRight.setColor(color);
    skydiverParaBrake.setColor(color);
    skydiverParaDive.setColor(color);
    skydiverParaDiedWater.setColor(color);
    skydiverParaBoatCenter.setColor(color);
    reset_position();

    // mind.addLayer(6, [](double x) { return 1.0 / (1.0 + std::exp(-x)); });
    mind.inputNames = {"Place", "Parachutes", "Altitude", "Boat longitude", "Boat speed", "Sd longitude", "Sd side speed", "Sd drop speed"};
    mind.outputNames = {"Jump", "Open parachutes", "Right", "Left", "Up", "Down", "Wait"};
    for (int i = 0; i < hiddenLayers; i++) {
        mind.addLayer(layersSize, [](double x) { return std::max(0.0, x); });
    }
    mind.addLayer(7, [](double x) { return std::max(0.0, x); });
    mind.compile();

    leftText = std::abs(Tools::getRand()) * 1500;
    topText = std::abs(Tools::getRand()) * 700;
}
Skydiver::~Skydiver() {
}

void Skydiver::add_gravity() {
    velocity.y += 1;
    pos.position.y += velocity.y;
}
void Skydiver::set_position(float left, float top) {
    pos.position.x = left;
    pos.position.y = top;
    start_pos.position.x = left;
    start_pos.position.y = top;
}
void Skydiver::reset_position() {
    this->pos = sf::FloatRect({start_pos.position.x, start_pos.position.y}, {8.f, 12.f});
}
void Skydiver::think(Plane plane, Boat boat, bool boot) {
    const float altitudeFromBoat = getAltitudeFromBoat(boat) / 1000;
    const float longitudeFromBoat = getLongitudeFromBoat(boat) / 1000;
    const float sdLongitude = this->pos.position.x / 10000;

    const float place = (state + 0.0) / 10;
    const float statusParachute = (parachuteState + 0.0) / 10;

    std::vector<double> input = {place, statusParachute, altitudeFromBoat, longitudeFromBoat, boat.velocity.x, sdLongitude, velocity.x / 10, velocity.y / 10};
    std::vector<double> output = mind.think(input);

    action = "";

    if (output.empty()) {
        return;
    }

    auto maxOutput = std::max_element(output.begin(), output.end());
    int16_t greater = std::distance(output.begin(), maxOutput);

    if (greater == 0) {
        action = "J";
    } else if (greater == 1) {
        action = "O";
    } else if (greater == 2) {
        action = "R";
    } else if (greater == 3) {
        action = "L";
    } else if (greater == 4) {
        action = "U";
    } else if (greater == 5) {
        action = "D";
    } else if (greater == 6) {
        action = "W";
    }

    if (boot) {
        if (state == State::ON_PLANE) {
            mind.mutate(1, true);
        }
    }
}
void Skydiver::doAction() {
    bool success = false;

    if (action == "J") {
        success = jump();
    } else if (action == "O") {
        success = parachutesOpen();
    } else if (action == "R") {
        success = parachutesGoRight();
    } else if (action == "L") {
        success = parachutesGoLeft();
    } else if (action == "U") {
        success = parachutesGoUp();
    } else if (action == "D") {
        success = parachutesGoDown();
    } else if (action == "W") {
        success = true;
    }

    if (success && usedActions.find(action) == std::string::npos) {
        usedActions += action;
    }
}
bool Skydiver::jump() {
    if (state == State::ON_PLANE) {
        if (pos.position.x > 0 && pos.position.x + pos.size.x < 1600) {
            state = State::ON_AIR;
            return true;
        }
    }
    return false;
}
bool Skydiver::parachutesOpen() {
    if (state == State::ON_AIR) {
        if (parachuteState == ParachutesState::CLOSED) {
            parachuteState = ParachutesState::OPENING;
            return true;
        }
    }
    return false;
}
bool Skydiver::parachutesGoRight() {
    if (parachuteState == ParachutesState::OPEN) {
        velocity.x += 0.01;
        return true;
    }
    return false;
}
bool Skydiver::parachutesGoLeft() {
    if (parachuteState == ParachutesState::OPEN) {
        velocity.x -= 0.01;
        return true;
    }
    return false;
}
bool Skydiver::parachutesGoUp() {
    if (parachuteState == ParachutesState::OPEN) {
        parachutes_brake.increase();
        return true;
    }
    return false;
}
bool Skydiver::parachutesGoDown() {
    if (parachuteState == ParachutesState::OPEN) {
        parachutes_brake.decrease();
        return true;
    }
    return false;
}
void Skydiver::update(Plane plane, Boat boat, int positionCounter, Game* game) {
    if (died) {
        if (state == State::ON_BOAT) {
            pos.position.x = boat.pos.position.x + boatTouchPlaceLeft;
            pos.position.y = boat.pos.position.y - pos.size.y - 1;
        }
        return;
    }
    // PLANE
    if (this->state == State::ON_PLANE) {
        if (plane.velocity.x > 0)
            pos.position.x = plane.pos.position.x + plane.pos.size.x - plane.door.x - pos.size.x;
        else
            pos.position.x = plane.pos.position.x + plane.door.x;

        pos.position.y = plane.pos.position.y + plane.door.y;
        velocity.x = plane.velocity.x;
        velocity.y = plane.velocity.y;
        return;
    } else if (this->state == State::ON_AIR) {
        // TIME ON AIR
        grade_time_on_air++;
    }

    // SKYDIVER
    if (parachuteState == ParachutesState::CLOSED) {
        if (this->state == State::ON_AIR) {
            velocity.x = velocity.x * 0.9974;  // Consider the wind!
        }
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

            const int direction = velocity.x < 0 ? -1 : 1;
            if (direction != last_direction) {
                if (timer - last_time_change_direction > 100) {  // 100 is about 2s
                    grade_direction_changes++;
                }
                last_direction = direction;
                last_time_change_direction = timer;
            }
        }
    }

    if (touchedBoat(boat)) {
        state = State::ON_BOAT;
        setBoatTouchPlace(boat);
        if (this->isLand(boat)) {
            landed = true;
            if (game) {
                game->onLand();
            }
            this->position = positionCounter + 1;
        } else {
            died = true;
        }
        saveScoreLanding(boat);
    } else if (pos.position.y >= getGroundTop()) {
        parachutes_brake.reset();
        state = State::ON_WATER;
        died = true;
        saveScoreLanding(boat);
    } else {
        pos.position.x += velocity.x;
        pos.position.y += velocity.y;
    }

    if (state == State::ON_BOAT) {
        pos.position.x = boat.pos.position.x + boatTouchPlaceLeft;
        pos.position.y = boat.pos.position.y - pos.size.y - 1;
    }

    visible = (pos.position.x + pos.size.x > 0 && pos.position.x < 1600);

    if (state == State::ON_AIR) {
        if (!visible) {
            died = true;
            saveScoreLanding(boat);
        }
    }

    timer++;
}
float Skydiver::getAltitudeFromBoat(Boat boat) {
    const float myFeetTop = pos.position.y + pos.size.y;
    return boat.getLandingPointTop() - myFeetTop - 1;
}
float Skydiver::getGroundTop() {
    return 790;
}
float Skydiver::getLongitudeFromBoat(Boat boat) {
    const float myMidLeft = pos.position.x + (pos.size.x / 2);
    return myMidLeft - boat.getLandingPointLeft();
}

bool Skydiver::isMaster() {
    return this->mind.mutated == 0;
}

void Skydiver::draw(sf::RenderWindow* w, Boat boat, bool show_information) {
    if (this->isMaster()) {
        if (show_information) {
            sf::VertexArray line(sf::PrimitiveType::Lines, 2);
            line[0].position = sf::Vector2f(pos.position.x + (pos.size.x * 0.5), pos.position.y - 12);              // Ponto A
            line[0].color = this->skydiverFall.sprite.getColor();                                     // Cor do ponto A
            line[1].position = sf::Vector2f(pos.position.x + (pos.size.x * 0.5), pos.position.y + pos.size.y - 2);  // Ponto B
            line[1].color = sf::Color::White;                                                         // Cor do ponto A
            w->draw(line);
            Tools::say(w, "Master", pos.position.x - 16, pos.position.y - 26);

            this->mind.draw(w, 400, 0);
        }
    }

    if (visible) {
        if (died) {
            if (state == State::ON_WATER) {
                if (parachuteState == ParachutesState::OPEN) {
                    skydiverParaDiedWater.draw(pos.position.x, pos.position.y, w);
                    skydiverParaDiedWater.animeAuto();
                } else {
                    skydiverDiedWater.draw(pos.position.x, pos.position.y, w);
                    skydiverDiedWater.animeAuto();
                }
            } else if (state == State::ON_BOAT) {
                skydiverDiedBoat.draw(pos.position.x, pos.position.y, w);
            }
        } else if (state == State::ON_PLANE) {
            skydiverOnPlane.draw(pos.position.x, pos.position.y, w);
        } else if (state == State::ON_BOAT) {
            skydiverParaBoatCenter.draw(pos.position.x, pos.position.y, w);
            skydiverParaBoatCenter.animeAuto();
        } else {
            if (parachuteState == ParachutesState::CLOSED) {
                skydiverFall.draw(pos.position.x, pos.position.y, w);
            } else if (parachuteState == ParachutesState::OPENING) {
                if (velocity.y > max_opened_parachutes_fall_speed * 2) {
                    skydiverParaOpening00.draw(pos.position.x, pos.position.y, w);
                } else {
                    skydiverParaOpening50.draw(pos.position.x, pos.position.y, w);
                }
            } else if (parachuteState == ParachutesState::OPEN) {
                if (action == "R") {
                    skydiverParaRight.draw(pos.position.x, pos.position.y, w);
                } else if (action == "L") {
                    skydiverParaLeft.draw(pos.position.x, pos.position.y, w);
                } else if (action == "U") {
                    skydiverParaBrake.draw(pos.position.x, pos.position.y, w);
                } else if (action == "D") {
                    skydiverParaDive.draw(pos.position.x, pos.position.y, w);
                } else {
                    skydiverParaCenter.draw(pos.position.x, pos.position.y, w);
                }
            } else {
                skydiverFall.draw(pos.position.x, pos.position.y, w);
            }
        }
    } else {
        sf::CircleShape circle;
        float radius = std::min(pos.size.x, pos.size.y) / 2.0f;
        circle.setRadius(radius);
        circle.setFillColor(sf::Color(0, 0, 0, 0));
        circle.setOutlineColor(sf::Color::Red);
        circle.setOutlineThickness(2.f);
        if (pos.position.x < 0) circle.setPosition(sf::Vector2f(0 + radius, pos.position.y + radius));
        if (pos.position.x > 1600) circle.setPosition(sf::Vector2f(1600 - pos.size.x - radius, pos.position.y + radius));
        w->draw(circle);
    }

    if (false) {
        // Tools::say(w, to_string(position) + " / " + to_string(grade_position), pos.position.x + 1, pos.position.y + 8);
        if (action.find("L") != std::string::npos) Tools::say(w, "L", pos.position.x - 4, pos.position.y - 4);
        if (action.find("R") != std::string::npos) Tools::say(w, "R", pos.position.x + 10, pos.position.y - 4);
        if (action.find("U") != std::string::npos) Tools::say(w, "U", pos.position.x + 1, pos.position.y - 26);
        if (action.find("D") != std::string::npos) Tools::say(w, "D", pos.position.x + 1, pos.position.y + 8);
        if (action.find("O") != std::string::npos) Tools::say(w, "O", pos.position.x + 1, pos.position.y + 12);
        if (action.find("J") != std::string::npos) Tools::say(w, "J", pos.position.x + 1, pos.position.y + 16);
        if (action == "") Tools::say(w, "x", pos.position.x + 1, pos.position.y + 20);

        if (false) {
            const float altitudeFromBoat = getAltitudeFromBoat(boat) / 1000;
            const float longitudeFromBoat = getLongitudeFromBoat(boat) / 1000;
            const float skydiverState = (state + 0.0);
            const float inpParachuteState = (parachuteState + 0.0);

            Tools::say(w, "altitudeFromBoat: " + to_string(altitudeFromBoat), pos.position.x + 20, pos.position.y - 30);
            Tools::say(w, "longitudeFromBoat: " + to_string(longitudeFromBoat), pos.position.x + 20, pos.position.y - 20);
            Tools::say(w, "skydiverState: " + to_string(skydiverState), pos.position.x + 20, pos.position.y - 10);
            Tools::say(w, "inpParachuteState: " + to_string(inpParachuteState), pos.position.x + 20, pos.position.y - 0);
            Tools::say(w, "Bias mutated: " + to_string(mind.mutated), pos.position.x + 20, pos.position.y + 10);
        }
    }

    if (0) {
        sf::RectangleShape rectangle;
        rectangle.setSize(sf::Vector2f(pos.size.x, pos.size.y));
        rectangle.setFillColor(sf::Color(0, 0, 0, 0));
        rectangle.setOutlineColor(sf::Color::Red);
        rectangle.setOutlineThickness(2.f);
        rectangle.setPosition(sf::Vector2f(pos.position.x, pos.position.y));
        w->draw(rectangle);
    }

    if (0 && state == State::ON_BOAT && landed) {
        sf::VertexArray line(sf::PrimitiveType::Lines, 2);
        line[0].position = sf::Vector2f(leftText, topText);  // Ponto A
        line[0].color = sf::Color::Red;                      // Cor do ponto A
        line[1].position = sf::Vector2f(pos.position.x, pos.position.y);  // Ponto B
        line[1].color = sf::Color::Blue;                     // Cor do ponto B
        w->draw(line);

        const int grade_mvl = grade_max_velocity_left;
        const int grade_mvr = grade_max_velocity_right;

        float footLeft = pos.position.x + (pos.size.x / 2);
        int landingLength = std::abs(boat.getLandingPointLeft() - boat.pos.position.x);

        std::vector<std::string> messages = {
            // "Velocidade horizontal: " + to_string(this->velocity.x) + " ps:" + to_string(parachuteState),
            // "Velocidade vertical: " + to_string(this->velocity.y),
            // "Parachutes brake: " + to_string(parachutes_brake.value),
            // "pos - left:" + to_string(pos.position.x) + ". top: " + to_string(pos.position.y),
            // "------",
            "GPS",
            "1 Altitude (boat) .....: " + to_string(getAltitudeFromBoat(boat)),
            "2 Boat velocity .......: " + to_string(boat.velocity.x),
            "3 Landing point (H): ..: " + to_string(getLongitudeFromBoat(boat)),
            "4 Horizontal velocity .: " + to_string(velocity.x),
            "5 Vertical velocity ...: " + to_string(velocity.y),
            "6 Place ...............: " + to_string(state),
            "7 Parachutes ..........: " + to_string(parachuteState),
            "landingLength...............: " + to_string(landingLength),
            "footLeft....................: " + to_string(footLeft),
            "boat.getLandingPointLeft()..: " + to_string(boat.getLandingPointLeft()),
            "Mais perto de 0-53..........: " + to_string(landingLength - std::abs(boat.getLandingPointLeft() - footLeft)),
            "Mais perto de 0-53 %........: " + to_string((landingLength - std::abs(boat.getLandingPointLeft() - footLeft)) / landingLength * 100),
            "Time on air...0-3000.(%)....: " + to_string(grade_time_on_air),
            "SCORE  " + to_string(getScore()),
            "Grade - Landing place ..: " + to_string(grade_landing_place),
            "Grade - Landing softly..: " + to_string(grade_landing_softly),
            "Grade - Max vel right...: " + to_string(grade_mvr),
            "Grade - Max vel left....: " + to_string(grade_mvl),
            "Grade - Direct changes..: " + to_string(grade_direction_changes)};

        for (int i = 0; i < (int)messages.size(); ++i) {
            Tools::say(w, messages[i], leftText, topText + (15 * i));
        }
    }
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
        float footLeft = pos.position.x + (pos.size.x / 2);
        float footTop = pos.position.y + pos.size.y;

        if (footLeft > boat.pos.position.x && footLeft < boat.pos.position.x + boat.pos.size.x) {        // It is in the boat area
            if (footTop >= boat.pos.position.y - 1 && footTop < boat.pos.position.y + boat.pos.size.y) {  // Touched ground
                return true;
            }
        }
    }
    return false;
}

void Skydiver::setBoatTouchPlace(Boat boat) {
    boatTouchPlaceLeft = pos.position.x - boat.pos.position.x;
}
float Skydiver::getScore() {
    return score;
}

void Skydiver::saveScoreLanding(Boat boat) {
    // Landing velocity - Heigher is better.
    const int max_velocity = max_slide_speed + max_fall_speed;
    grade_landing_softly = (max_velocity - std::abs(velocity.x) + std::abs(velocity.y)) / max_velocity * 100;

    // Place - How much near center higher
    int landingLength = std::abs(boat.getLandingPointLeft() - boat.pos.position.x);
    float footLeft = pos.position.x + (pos.size.x / 2);
    float distance_mast = std::abs(boat.getLandingPointLeft() - footLeft);
    float width_screen = 1600;

    grade_landing_place = std::abs((width_screen - distance_mast)) / 1000;

    grade_max_velocity_right = std::abs(grade_max_velocity_right) / max_slide_speed * 100;
    grade_max_velocity_left = std::abs(grade_max_velocity_left) / max_slide_speed * 100;

    grade_max_velocity_right = (int)grade_max_velocity_right;
    grade_max_velocity_left = (int)grade_max_velocity_left;

    // Time on air
    float gTimeOnAir;
    gTimeOnAir = grade_time_on_air > 3000 ? 3000 : grade_time_on_air;
    gTimeOnAir = gTimeOnAir / 3000 * 100;

    // Define some importance to each grade.
    grade_direction_changes = grade_direction_changes;
    grade_landing_softly = grade_landing_softly * 0.1;
    gTimeOnAir = gTimeOnAir * 0.1;
    grade_max_velocity_right = grade_max_velocity_right * 0.1;
    grade_max_velocity_left = grade_max_velocity_left * 0.1;

    grade_time_on_air = (int)gTimeOnAir;
    grade_time_on_air = 0;                          // Disabled
    grade_used_actions = usedActions.size() * 100;  // Tools::map(usedActions.size(), 0, 7, 0, 10);

    grade_position = this->qtd_skydivers - this->position;

    // if (this->isMaster()) {
    //     std::cout << "landingLength: " << landingLength << " footLeft: " << footLeft << " boat.getLandingPointLeft(): " << boat.getLandingPointLeft() << " grade_landing_place:  " << grade_landing_place << std::endl;
    // }

    // if (!landed) grade_landing_place = 0;

    // Set Score
    // score = grade_position + grade_landing_softly + grade_landing_place + grade_max_velocity_right + grade_max_velocity_left + grade_direction_changes + grade_time_on_air + grade_used_actions;
    score = grade_landing_place + grade_direction_changes + grade_used_actions;

    if (landed) {
        score += 1000;
    }
}
