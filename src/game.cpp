#include "game.h"
#include "logger.h"

#include <iostream>
#include <string>

Game::Game(unsigned int window_width, unsigned int window_height,
           std::string window_title)
{
    LOG_INFO("Game constructed %", 1);

    m_window.create(sf::VideoMode { { window_width, window_height } }, window_title);
}

Game::~Game()
{
    LOG_INFO("Game destructed %", 1);
}

void Game::run()
{
    while (m_window.isOpen())
    {
        while (std::optional event = m_window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                m_window.close();
            }
        }

        m_window.clear();
        m_window.display();
    }
}
