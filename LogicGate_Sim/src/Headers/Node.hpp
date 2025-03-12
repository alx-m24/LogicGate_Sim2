#pragma once
#include <SFML/Graphics.hpp>


class Node
{
public:
	bool state = false;
	bool selected = false;
	sf::Vector2f position;

private:
	sf::CircleShape circle;

public:
	void draw(sf::RenderWindow& window, float radius);

public:
	inline bool contains(sf::Vector2f point) { return circle.getGlobalBounds().contains(point); }
};
