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
#pragma region KeyBinds
	if (ADD_NODE && !addedNodeLastFrame) {
		nodes.emplace_back(new Node());
		nodes.back()->state = false;
		nodes.back()->position = (sf::Vector2f(window.getSize()) / 2.0f) + getRandomOffset(-50.0f, 50.0f);
	}

	addedNodeLastFrame = ADD_NODE;
#pragma endregion

#pragma region Mouse

#pragma region Position
	mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
#pragma endregion

#pragma region Left mouse btn
	if (LEFTMOUSE && !lastLeft) {
		for (int i = 0; i < nodes.size() && !movingObject; ++i) {
			if (nodes[i]->contains(mousePos)) {
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
			if (nodes[i]->contains(mousePos)) {
				nodes[i]->selected = true;
				break;
			}
		}
	}
	// Just released the right mouse btn
	else if (lastRight) {
		for (int i = 0; i < nodes.size() && !movingObject; ++i) {
			if (nodes[i]->contains(mousePos)) {
				nodes[i]->state = !nodes[i]->state;
				break;
			}
		}
	}

	lastRight = RIGHTMOUSE;
#pragma endregion

#pragma region Middle Mouse btn
	if (!addingWire) {
		if (MIDDLEMOUSE) {
			for (int i = 0; i < nodes.size() && !movingObject; ++i) {
				if (nodes[i]->contains(mousePos)) {
					nodes[i]->selected = true;
					break;
				}
			}
		}
		// Just released the middle mouse btn
		else if (lastMid) {
			for (int i = 0; i < nodes.size() && !movingObject; ++i) {
				if (nodes[i]->contains(mousePos)) {
					addingWire = true;

					wires.emplace_back(Wire(&(nodes[i]->position), &mousePos));

					void* tempPtr = &(*nodes[i]); // pointer to pointe: ik it fcks with your brain
					wires.back().input = static_cast<bool*>(tempPtr);
					break;
				}
			}
		}
	}
	else {
		if (MIDDLEMOUSE) {
			for (int i = 0; i < nodes.size() && !movingObject; ++i) {
				if (nodes[i]->contains(mousePos)) {
					nodes[i]->selected = true;
					break;
				}
			}
		}
		// Just released the middle mouse btn
		else if (lastMid) {
			addingWire = false;
			bool addedWire = false;
			for (int i = 0; i < nodes.size() && !movingObject; ++i) {
				if (nodes[i]->contains(mousePos)) {
					wires.back().p2 = &(nodes[i]->position);

					void* tempPtr = &(*nodes[i]);
					wires.back().output = static_cast<bool*>(tempPtr);

					addedWire = true;
					break;
				}
			}
			if (!addedWire) {
				wires.pop_back();
			}
		}
	}

	lastMid = MIDDLEMOUSE;
#pragma endregion

#pragma endregion	

#pragma region Move node
	if (movedNodeIdx != -1) {
		nodes[movedNodeIdx]->position = mousePos;
		nodes[movedNodeIdx]->selected = true;
		if (SHIFT) {
			// Clamping position
			nodes[movedNodeIdx]->position = clampToGrid(nodes[movedNodeIdx]->position, spacing) + sf::Vector2f(spacing, spacing) / 2.0f;
		}
	}
#pragma endregion

	for (Wire& wire : wires) wire.update();
}

void Simulation::draw(sf::RenderWindow& window)
{
	sf::Vector2f windowSize = sf::Vector2f(window.getSize());

#pragma region Background
	sf::RectangleShape background;
	background.setSize(windowSize);
	background.setPosition(0.0f, 0.0f);

	bgShader.setUniform("resolution", windowSize);
	bgShader.setUniform("time", clock.getElapsedTime().asSeconds());

	int nodeNum = nodes.size();
	bgShader.setUniform("nodeNum", nodeNum);
	for (int i = 0; i < nodeNum; ++i) {
		std::string name = "nodes[" + std::to_string(i) + "].";
		bgShader.setUniform(name + "state", nodes[i]->state);
		bgShader.setUniform(name + "position", nodes[i]->position);
	}
	int wireNum = wires.size();
	bgShader.setUniform("wireNum", wireNum);
	for (int i = 0; i < wireNum; ++i) {
		std::string name = "wires[" + std::to_string(i) + "].";
		bgShader.setUniform(name + "state", wires[i].getState());
		bgShader.setUniform(name + "p1", *wires[i].p1);
		bgShader.setUniform(name + "p2", *wires[i].p2);
	}

	window.draw(background, &bgShader);
#pragma endregion

	for (Node* node : nodes) {
		node->draw(window, spacing / 2.0f);
	}
}

void Simulation::zoom(sf::RenderWindow& window)
{
#pragma region Spacing
	spacing = std::min(window.getSize().x, window.getSize().y) / gridSize;
	bgShader.setUniform("spacing", spacing);

	if (gridSize > 20.0f) thickness = 1;
	else thickness = 2;
#pragma endregion

	for (Node* node : nodes) {
		sf::Vector2f viewCenter = sf::Vector2f(0.0f, window.getSize().y);
		node->position = (node->position - viewCenter) * oldGridSize / gridSize + viewCenter;
	}

	oldGridSize = gridSize;
}
