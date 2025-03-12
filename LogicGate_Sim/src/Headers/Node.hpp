#pragma once
#include <SFML/Graphics.hpp>


class Node
{
private:
	sf::CircleShape circle;

public:
	bool state;
	bool selected = false;
	sf::Vector2f position;

public:
	void draw(sf::RenderWindow& window, float radius);

public:
	inline bool contains(sf::Vector2f point) { return circle.getGlobalBounds().contains(point); }
};
