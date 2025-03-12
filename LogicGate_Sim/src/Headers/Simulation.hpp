#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "Node.hpp"
#include "KeyBinds/KeyBinds.hpp"

inline sf::Vector2f getRandomOffset(float minOffset, float maxOffset) {
	float offsetX = minOffset + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX) / (maxOffset - minOffset));
	float offsetY = minOffset + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX) / (maxOffset - minOffset));

	return sf::Vector2f(offsetX, offsetY);
}

class Simulation
{
public:
	Simulation(std::string dirPath);

private:
	sf::Clock clock;
	sf::Shader bgShader;

private:
	const float spacing = 50.0f;
	const int thickness = 1;

private:
	bool addedNodeLastFrame = false;
	bool movingObject = false;
	bool lastLeft = false;
	int movedNodeIdx = -1;

private:
	std::vector<Node> nodes;

public:
	void update(sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);
};

