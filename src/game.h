#pragma once

#include <SFML/Graphics.hpp>

#include <string>

class Game
{
private:
    sf::RenderWindow m_window { };

public:
    Game(unsigned int window_width = 640, unsigned int window_height = 480,
         std::string window_title = "Relic Engine v1.0");
    ~Game();

    void run();
};
