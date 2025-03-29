#include "Simulation.hpp"

Simulation::Simulation(std::string dirPath, unsigned int maxtickRate) : maxtickRate(maxtickRate)
{
	shader.loadFromFile(dirPath + "\\Shaders\\mainShader.frag", sf::Shader::Fragment);
	shader.setUniform("thickness", thickness);

	if (!arial.loadFromFile("C:\\Windows\\Fonts\\Arial.ttf")) std::cout << "Could not load font" << std::endl;

	clock.restart();
	
	// Seed the random number generator
	std::srand(static_cast<unsigned>(std::time(0)));
}

Simulation::~Simulation()
{
	for (Node* node : nodes) {
		delete node;
	}
	for (Gate* gate : gates) {
		delete gate;
	}
}

void Simulation::update(sf::RenderWindow& window)
{
#pragma region KeyBinds
	if (ADD_GATE && !addedGateLastFrame) {
	}
	else if (ADD_NODE && !addedNodeLastFrame) {
		nodes.emplace_back(new Node());
		nodes.back()->state = false;
		nodes.back()->position = (sf::Vector2f(window.getSize()) / 2.0f) + getRandomOffset(-50.0f, 50.0f);
	}

	slowed = SPACE;
	if (slowed) {
		sf::Clock delay;
		while (delay.getElapsedTime().asSeconds() < 0.75f);
	}

	addedNodeLastFrame = ADD_NODE;
	addedGateLastFrame = ADD_GATE;
#pragma endregion

#pragma region Mouse

#pragma region Position
	lastmousePos = mousePos;
	mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
#pragma endregion

#pragma region Left mouse btn
	if (LEFTMOUSE && !lastLeft) {
		for (int i = nodes.size() - 1; i >= 0 && !movingObject; --i) {
			if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
				movingObject = true;
				movedNodeIdx = i;
				break;
			}
		}
		for (int i = gates.size() - 1; i >= 0 && !movingObject; --i) {
			if (gates[i]->contains(mousePos) && gates[i]->shouldDraw) {
				movingObject = true;
				movedGateIdx = i;
				break;
			}
		}

	}
	if (lastLeft && !LEFTMOUSE) {
		movingObject = false;
		movedNodeIdx = -1;
		movedGateIdx = -1;
	}

	lastLeft = LEFTMOUSE;

#pragma endregion

#pragma region Right Mouse btn
	if (RIGHTMOUSE) {
		for (int i = nodes.size() - 1; i >= 0 && !movingObject; --i) {
			if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
				nodes[i]->selected = true;
				break;
			}
		}
	}
	// Just released the right mouse btn
	else if (lastRight) {
		for (int i = 0; i < nodes.size() && !movingObject; ++i) {
			if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
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
				if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
					nodes[i]->selected = true;
					break;
				}
			}
			for (int i = 0; i < gates.size() && !movingObject; ++i) {
				if (!gates[i]->shouldDraw) continue;
				for (Connector& connector : gates[i]->inputs) {
					if (connector.contains(mousePos)) {
						gates[i]->isSelected = true;
						break;
					}
				}
				for (Connector& connector : gates[i]->outputs) {
					if (connector.contains(mousePos)) {
						gates[i]->isSelected = true;
						break;
					}
				}
			}
		}
		// Just released the middle mouse btn
		else if (lastMid) {
			for (int i = 0; i < nodes.size() && !movingObject; ++i) {
				if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
					addingWire = true;

					wires.emplace_back(Wire(&(nodes[i]->position), &mousePos));

					void* tempPtr = &(*nodes[i]); // pointer to pointe: ik it fcks with your brain
					wires.back().input = static_cast<bool*>(tempPtr);
					break;
				}
			}
			for (int i = 0; i < gates.size() && !movingObject; ++i) {
				if (!gates[i]->shouldDraw) continue;
				for (Connector& connector : gates[i]->inputs) {
					if (connector.contains(mousePos)) {
						addingWire = true;

						wires.emplace_back(Wire(&connector.gloabalPosition, &mousePos));

						wires.back().input = &connector.state;
						break;
					}
				}
				for (Connector& connector : gates[i]->outputs) {
					if (connector.contains(mousePos)) {
						addingWire = true;

						wires.emplace_back(Wire(&connector.gloabalPosition, &mousePos));

						wires.back().input = &connector.state;
						break;
					}
				}
			}
		}
	}
	else {
		if (MIDDLEMOUSE) {
			for (int i = 0; i < nodes.size() && !movingObject; ++i) {
				if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
					nodes[i]->selected = true;
					break;
				}
			}
			for (int i = 0; i < gates.size() && !movingObject; ++i) {
				if (!gates[i]->shouldDraw) continue;
				for (Connector connector : gates[i]->inputs) {
					if (connector.contains(mousePos)) {
						gates[i]->isSelected = true;
						break;
					}
				}
				for (Connector connector : gates[i]->outputs) {
					if (connector.contains(mousePos)) {
						gates[i]->isSelected = true;
						break;
					}
				}
			}
		}
		// Just released the middle mouse btn
		else if (lastMid) {
			addingWire = false;
			bool addedWire = false;
			for (int i = 0; i < nodes.size() && !movingObject; ++i) {
				if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
					wires.back().p2 = &(nodes[i]->position);

					void* tempPtr = &(*nodes[i]);
					wires.back().output = static_cast<bool*>(tempPtr);

					addedWire = true;
					break;
				}
			}
			for (int i = 0; i < gates.size() && !movingObject; ++i) {
				if (!gates[i]->shouldDraw) continue;
				for (Connector& connector : gates[i]->inputs) {
					if (connector.contains(mousePos)) {
						wires.back().p2 = &connector.gloabalPosition;
						wires.back().output = &connector.state;

						addedWire = true;
						break;
					}
				}
				for (Connector& connector : gates[i]->outputs) {
					if (connector.contains(mousePos)) {
						wires.back().p2 = &connector.gloabalPosition;
						wires.back().output = &connector.state;

						addedWire = true;
						break;
					}
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

#pragma region Move gate
	if (movedGateIdx != -1) {
		gates[movedGateIdx]->position = mousePos;
		gates[movedGateIdx]->isSelected = true;
		if (SHIFT) {
			// Clamping position
			gates[movedGateIdx]->position = clampToGrid(gates[movedGateIdx]->position, spacing) + sf::Vector2f(spacing, spacing) / 2.0f;
		}
	}
#pragma endregion

#pragma region Physics frames
	if (slowed) {
		for (Wire& wire : wires) wire.update();
		for (Gate* gate : gates) gate->update();
	}
	else {
		for (unsigned int i = 0; i < std::min(static_cast<unsigned int>(gates.size() + 1), maxtickRate); ++i) {
			for (Wire& wire : wires) wire.update();
			for (Gate* gate : gates) gate->update();
		}
	}
#pragma endregion
}

void Simulation::draw(sf::RenderWindow& window)
{
	sf::Vector2f windowSize = sf::Vector2f(window.getSize());

#pragma region Background
	sf::RectangleShape background;
	background.setSize(windowSize);
	background.setPosition(0.0f, 0.0f);

	shader.setUniform("resolution", windowSize);
	shader.setUniform("time", clock.getElapsedTime().asSeconds());

	int nodeNum = 0;
	for (Node* node : nodes) {
		if (!node->shouldDraw) continue;

		std::string name = "nodes[" + std::to_string(nodeNum) + "].";
		shader.setUniform(name + "state", node->state);
		shader.setUniform(name + "position", node->position);
		shader.setUniform(name + "activeColor", sf::Vector3f(static_cast<sf::Uint8>(node->activeColor.r), static_cast<sf::Uint8>(node->activeColor.g), static_cast<sf::Uint8>(node->activeColor.b)));

		++nodeNum;
	}
	shader.setUniform("nodeNum", nodeNum);

	int wireNum = 0;
	for (const Wire& wire : wires) {
		if (!wire.shouldDraw) continue;

		std::string name = "wires[" + std::to_string(wireNum) + "].";
		shader.setUniform(name + "state", wire.getState());
		shader.setUniform(name + "p1", *wire.p1);
		shader.setUniform(name + "p2", *wire.p2);
		shader.setUniform(name + "activeColor", sf::Vector3f(static_cast<sf::Uint8>(wire.activeColor.r), static_cast<sf::Uint8>(wire.activeColor.g), static_cast<sf::Uint8>(wire.activeColor.b)));

		++wireNum;
	}
	shader.setUniform("wireNum", wireNum);

	int gateNum = 0;
	for (Gate* gate : gates) {
		if (!gate->shouldDraw) continue;

		std::string name = "gates[" + std::to_string(gateNum) + "].";
		gate->setuniforms(shader, name, spacing);

		++gateNum;
	}
	shader.setUniform("gateNum", gateNum);

	window.draw(background, &shader);
#pragma endregion

	for (Gate* gate : gates) {
		gate->draw(window, spacing);
	}
	for (Node* node : nodes) {
		node->draw(window, spacing / 2.0f);
	}
}

void Simulation::zoom(sf::RenderWindow& window)
{
#pragma region Spacing
	spacing = std::min(window.getSize().x, window.getSize().y) / gridSize;
	shader.setUniform("spacing", spacing);

	if (gridSize > 20.0f) thickness = 1;
	else thickness = 2;
#pragma endregion

	sf::Vector2f viewCenter = mousePos;
	for (Node* node : nodes) {
		node->position = (node->position - viewCenter) * oldGridSize / gridSize + viewCenter;
	}

	for (Gate* gate : gates) {
		gate->position = (gate->position - viewCenter) * oldGridSize / gridSize + viewCenter;
		gate->resize(spacing);
	}

	oldGridSize = gridSize;
}

void Simulation::addGate(std::string gate, sf::RenderWindow& window)
{
	if (gate == "NOT") gates.emplace_back(new NotGate(spacing, arial));
	if (gate == "AND") gates.emplace_back(new AndGate(spacing, arial));
	if (gate == "OR") gates.emplace_back(new OrGate(spacing, arial));
	if (gate == "XOR") gates.emplace_back(new XOrGate(spacing, arial));

	gates.back()->position = (sf::Vector2f(window.getSize()) / 2.0f) + getRandomOffset(-50.0f, 50.0f);
}

void Simulation::addNode(sf::RenderWindow& window)
{
	nodes.emplace_back(new Node());
	nodes.back()->state = false;
	nodes.back()->position = (sf::Vector2f(window.getSize()) / 2.0f) + getRandomOffset(-50.0f, 50.0f);
}

std::vector<Gate> Simulation::getGates()
{
	std::vector<Gate> gates;

	gates.emplace_back(NotGate(spacing, arial));
	gates.emplace_back(AndGate(spacing, arial));
	gates.emplace_back(OrGate(spacing, arial));
	gates.emplace_back(XOrGate(spacing, arial));

	return gates;
}

std::string Simulation::analyze()
{
	return toString(analyzeCircuit(getComponents()));
}

std::pair<unsigned int, unsigned int> Simulation::getNodeNum()
{
	AnalyzedCiruit circuit = analyzeCircuit(getComponents());
	return std::pair<unsigned int, unsigned int>(circuit.inputNodesNum, circuit.outputNodesNum);
}

Components Simulation::getComponents()
{
	return Components{ nodes, gates, wires };
}
