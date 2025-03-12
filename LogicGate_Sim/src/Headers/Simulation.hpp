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
inline sf::Vector2f clampToGrid(sf::Vector2f position, float gridSize) {
	return sf::Vector2f(
		std::floor(position.x / gridSize) * gridSize,
		std::floor(position.y / gridSize) * gridSize
	);
}

class Simulation
{
public:
	Simulation(std::string dirPath);

private:
	sf::Clock clock;
	sf::Shader bgShader;

public:
	float gridSize = 10.0f;

private:
	const int thickness = 2;
	float spacing = 50.0f;

private:
	bool addedNodeLastFrame = false;
	bool movingObject = false;
	bool lastLeft = false;
	bool lastRight = false;
	int movedNodeIdx = -1;
	float oldGridSize = gridSize;

private:
	std::vector<Node> nodes;

public:
	void update(sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);
	void zoom(sf::RenderWindow& window);
};

