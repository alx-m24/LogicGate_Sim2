#include "Simulation.hpp"

Simulation::Simulation(std::string dirPath)
{
	bgShader.loadFromFile(dirPath + "\\Shaders\\background.frag", sf::Shader::Fragment);

	bgShader.setUniform("spacing", spacing);
	bgShader.setUniform("thickness", thickness);

	clock.restart();
	
	// Seed the random number generator
	std::srand(static_cast<unsigned>(std::time(0)));
}

void Simulation::update(sf::RenderWindow& window)
{
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
	sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

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

	if (movedNodeIdx != -1) nodes[movedNodeIdx].position = mousePos;

	lastLeft = LEFTMOUSE;
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

