#pragma once
#include <SFML/Graphics.hpp>

class Node
{
public:
	bool state;
	sf::Vector2f position;

public:
	void draw(sf::RenderWindow& window);
};
