#pragma once

#ifndef GAME_H
#define GAME_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

#include "animation.hpp"
#include "boat.hpp"
#include "plane.hpp"
#include "skydiver.hpp"
#include "tools.hpp"

struct Config {
    std::optional<int> round;
    std::optional<float> boatVelocityX;
    std::optional<float> planeVelocityX;
    std::optional<int> qtdSkydivers;
    std::optional<bool> fullscreen;
    std::optional<int> drawInterval;
    std::optional<int> score;
    std::optional<std::string> commandOnLand;
    std::optional<int> hiddenLayers;
    std::optional<int> layersSize;
    std::optional<bool> keepMaster;
    std::optional<std::string> weights;
    std::optional<std::string> biases;
};

class Game {
   public:
    Game();

    std::vector<Skydiver*> skydivers;
    bool paused = false;
    bool fullscreen = false;
    int qtd_skydivers = 30;

    Animation scenario;
    Plane plane;
    Boat boat;
    Skydiver* lastBetterSkydiver;

    Config config;
    sf::Clock roundClock;

    sf::RenderWindow window;

    sf::View view;
    sf::Font font_roboto;
    sf::Font font_spacemono_regular;
    sf::Text text_gameover;
    float playTimer = 0;

    bool gameover_loaded = false;
    bool drawing = true;
    bool training = false;

    int phase_current = 0;
    int phase_total = 0;
    int round = 1;

    sf::Text text_gamewin;
    sf::Text text_generic;

    Skydiver skydiver;

    enum pages {
        GAME_PLAY,
    };
    int page = pages::GAME_PLAY;

    static const int menuopc_size = 5;
    static std::string menuopc[menuopc_size];
    int menuopc_selected = 0;
    bool key_released = true;
    bool editing = false;
    int editing_framecount = 1;
    int frameCount = 0;
    bool show_information = false;
    bool syncronism = true;
    bool bootSkydivers = false;

    // 176, 70%, 57%

    float hsla_a = 176;
    float hsla_b = 0.70;
    float hsla_c = 0.57;
    float hsla_d = 1;

    uint16_t onPlane = 0;
    uint16_t onAir = 0;
    uint16_t onBoat = 0;
    uint16_t died = 0;
    uint16_t landedCount = 0;
    uint16_t sdTotal = 0;
    uint16_t positionCounter = 0;

    struct profile {
        int completed_phases = 0;
        int lifes = 0;
        int seconds_playing = 0;
    } profile;

    void play();

    void close();

    void loop_events();
    void run();
    // void saveWeights(std::string weights);
    // void saveBiases(std::string biases);
    // std::string loadWeights();
    // std::string loadBiases();

    void setWindowIcon(sf::RenderWindow* w);
    bool hasOnScreenAir();
    bool window_has_focus = true;

    void saveConfig(const Config& config, const std::string& arquivo);
    Config loadConfig(const std::string& arquivo);

    void onLand();
};
#endif
