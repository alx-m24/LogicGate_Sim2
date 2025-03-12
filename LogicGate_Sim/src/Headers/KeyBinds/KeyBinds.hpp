#pragma once
#include <SFML/Graphics.hpp>
#include <map>

extern std::map<sf::Keyboard::Key, bool> keys;

#define CTRL (keys[sf::Keyboard::LControl] || keys[sf::Keyboard::RControl])
#define ADD_NODE (keys[sf::Keyboard::N] && CTRL)
#define LEFTMOUSE (sf::Mouse::isButtonPressed(sf::Mouse::Left))