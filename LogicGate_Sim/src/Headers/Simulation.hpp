#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Node
{
public:
	bool state;
	sf::Vector2f position;

public:
	void draw(sf::RenderWindow& window);
};

class Simulation
{
public:
	Simulation(std::string dirPath);

private:
	sf::Shader bgShader;
	const float spacing = 50.0f;
	const int thickness = 1;

private:
	Node test;

public:
	void update(sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);
};

