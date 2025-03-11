#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main() {
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;

	sf::RenderWindow window(sf::VideoMode(1000, 600), "LogicGate Simulator", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);
	sf::Vector2u windowSize = window.getSize();

	std::string currentDir = fs::current_path().string() + "\\src";

	sf::Shader backgroundShader;
	backgroundShader.loadFromFile(currentDir + "\\Shaders\\background.frag", sf::Shader::Fragment);

	float spacing = 10.0f;
	int thickness = 3;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				windowSize = window.getSize();
				break;
			case sf::Event::MouseWheelScrolled:
				spacing += event.mouseWheelScroll.delta;
				break;
			case sf::Event::KeyPressed:
				break;
			default:
				break;
			}

			window.clear();

			sf::RectangleShape background;
			background.setSize(sf::Vector2f(windowSize));
			background.setPosition(0.0f, 0.0f);

			backgroundShader.setUniform("spacing", spacing);
			backgroundShader.setUniform("thickness", thickness);

			window.draw(background, &backgroundShader);

			window.display();
		}
	}

	return EXIT_SUCCESS;
}