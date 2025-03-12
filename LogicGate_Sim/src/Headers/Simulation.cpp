#include "Simulation.hpp"

Simulation::Simulation(std::string dirPath)
{
	bgShader.loadFromFile(dirPath + "\\Shaders\\background.frag", sf::Shader::Fragment);

	bgShader.setUniform("spacing", spacing);
	bgShader.setUniform("thickness", thickness);
}

void Simulation::update(sf::RenderWindow& window)
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		test.position = sf::Vector2f(sf::Mouse::getPosition(window));
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) test.state = !test.state;
}

void Simulation::draw(sf::RenderWindow& window)
{
	sf::Vector2f windowSize = sf::Vector2f(window.getSize());

	sf::RectangleShape background;
	background.setSize(windowSize);
	background.setPosition(0.0f, 0.0f);

	bgShader.setUniform("resolution", windowSize);
	bgShader.setUniform("resolution", windowSize);
	bgShader.setUniform("node.state", test.state);
	bgShader.setUniform("node.position", test.position);

	window.draw(background, &bgShader);
	test.draw(window);
}

void Node::draw(sf::RenderWindow& window)
{
	sf::CircleShape circle(20.0f);
	circle.setFillColor(state ? sf::Color(209, 0, 0) : sf::Color::White);
	circle.setOutlineThickness(3.0f);
	circle.setOutlineColor(sf::Color::Black);
	circle.setOrigin(circle.getRadius(), circle.getRadius());
	circle.setPosition(position);

	window.draw(circle);
}
