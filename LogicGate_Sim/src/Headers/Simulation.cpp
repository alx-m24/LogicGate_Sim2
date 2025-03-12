#include "Simulation.hpp"

Simulation::Simulation(std::string dirPath)
{
	bgShader.loadFromFile(dirPath + "\\Shaders\\background.frag", sf::Shader::Fragment);
	bgShader.setUniform("thickness", thickness);

	clock.restart();
	
	// Seed the random number generator
	std::srand(static_cast<unsigned>(std::time(0)));
}

void Simulation::update(sf::RenderWindow& window)
{
#pragma region Spacing
	spacing = std::min(window.getSize().x, window.getSize().y) / gridSize;
	bgShader.setUniform("spacing", spacing);
#pragma endregion

#pragma region KeyBinds
	if (ADD_NODE && !addedNodeLastFrame) {
		addedNodeLastFrame = true;
		nodes.emplace_back(Node());
		nodes.back().state = false;
		nodes.back().position = (sf::Vector2f(window.getSize()) / 2.0f) + getRandomOffset(-50.0f, 50.0f);
	}
	else {
		addedNodeLastFrame = ADD_NODE;
	}
#pragma endregion

#pragma region Mouse

#pragma region Position
	sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
#pragma endregion

#pragma region Left mouse btn
	if (LEFTMOUSE && !lastLeft) {
		for (int i = 0; i < nodes.size() && !movingObject; ++i) {
			if (nodes[i].contains(mousePos)) {
				movingObject = true;
				movedNodeIdx = i;
			}
		}
	}
	if (lastLeft && !LEFTMOUSE) {
		movingObject = false;
		movedNodeIdx = -1;
	}

	lastLeft = LEFTMOUSE;

#pragma endregion

#pragma region Right Mouse btn
	if (RIGHTMOUSE) {
		for (int i = 0; i < nodes.size() && !movingObject; ++i) {
			if (nodes[i].contains(mousePos)) {
				nodes[i].selected = true;
				break;
			}
		}
	}
	// Just released the right mouse btn
	else if (lastRight) {
		for (int i = 0; i < nodes.size() && !movingObject; ++i) {
			if (nodes[i].contains(mousePos)) {
				nodes[i].state = !nodes[i].state;
				break;
			}
		}
	}

	lastRight = RIGHTMOUSE;
#pragma endregion

#pragma endregion	

#pragma region Move node
	if (movedNodeIdx != -1) {
		nodes[movedNodeIdx].position = mousePos;
		nodes[movedNodeIdx].selected = true;
		if (SHIFT) {
			// Clamping position
			nodes[movedNodeIdx].position = sf::Vector2f(sf::Vector2i(nodes[movedNodeIdx].position / spacing)) * spacing;
			nodes[movedNodeIdx].position += sf::Vector2f(spacing, spacing) / 2.0f;
		}
	}
#pragma endregion
}

void Simulation::draw(sf::RenderWindow& window)
{
	sf::Vector2f windowSize = sf::Vector2f(window.getSize());

	sf::RectangleShape background;
	background.setSize(windowSize);
	background.setPosition(0.0f, 0.0f);

	bgShader.setUniform("resolution", windowSize);
	bgShader.setUniform("time", clock.getElapsedTime().asSeconds());

	int nodeNum = nodes.size();
	bgShader.setUniform("nodeNum", nodeNum);
	for (int i = 0; i < nodeNum; ++i) {
		std::string name = "nodes[" + std::to_string(i) + "].";
		bgShader.setUniform(name + "state", nodes[i].state);
		bgShader.setUniform(name + "position", nodes[i].position);
	}

	window.draw(background, &bgShader);

	for (Node& node : nodes) node.draw(window);
}

