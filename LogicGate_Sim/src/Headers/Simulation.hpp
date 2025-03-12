#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "Node.hpp"
#include "Wire.hpp"
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
	sf::Vector2f mousePos;

public:
	float gridSize = 10.0f;

private:
	int thickness = 2;
	float spacing = 50.0f;

private:
	bool addingWire = false;
	bool addedNodeLastFrame = false;
	bool movingObject = false;
	bool lastLeft = false;
	bool lastRight = false;
	bool lastMid = false;
	int movedNodeIdx = -1;
	float oldGridSize = gridSize;

private:
	// Pointer: as 'wire' references nodes and thus changing the location(when changing size) will affect the wires
	std::vector<Node*> nodes;
	std::vector<Wire> wires;

public:
	void update(sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);
	void zoom(sf::RenderWindow& window);
};

