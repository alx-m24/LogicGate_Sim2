#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include "Headers/Simulation.hpp"

namespace fs = std::filesystem;

int main() {
#pragma region Initialize
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	sf::RenderWindow window(sf::VideoMode(1000, 600), "LogicGate Simulator", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	std::string currentDir = fs::current_path().string() + "\\src";
#pragma endregion

#pragma region Objects
	Simulation simulation(currentDir);
#pragma endregion

#pragma region Main loop
	while (window.isOpen()) {
#pragma region Input
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			default:
				break;
			}
		}
#pragma endregion

#pragma region Update
		simulation.update(window);
#pragma endregion

#pragma region Display
		window.clear();

		simulation.draw(window);

		window.display();
#pragma endregion
	}
#pragma endregion

#pragma region Terminate
	return EXIT_SUCCESS;
#pragma endregion
}