#pragma once
#include <SFML/Graphics.hpp>

class Wire
{
public:
	bool* input = nullptr;
	bool* output = nullptr;

public:
	sf::Vector2f* p1;
	sf::Vector2f* p2;
	sf::Color activeColor = sf::Color(219, 22, 47);

public:
	Wire(sf::Vector2f* p1, sf::Vector2f* p2) : p1(p1), p2(p2) {}

public:
	void update();
	bool getState() const;
};
