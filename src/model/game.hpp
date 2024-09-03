#pragma once

#ifndef GAME_H
#define GAME_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "animation.hpp"
#include "skydiver.hpp"
#include "tools.hpp"

class Game {
   public:
    Game();

    std::vector<Skydiver *> skydivers;

    Animation scenario;

    sf::RenderWindow window;

    sf::View view;
    sf::Font font_roboto;
    sf::Text text_gameover;

    bool gameover_loaded = false;
    int phase_current = 0;
    int phase_total = 0;

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

    struct profile {
        int completed_phases = 0;
        int lifes = 0;
        int seconds_playing = 0;
    } profile;

    void play();

    void close();

    void loop_events();
    void run();
};
#endif
