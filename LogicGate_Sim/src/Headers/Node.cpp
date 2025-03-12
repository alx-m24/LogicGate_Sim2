#include "Node.hpp"

void Node::draw(sf::RenderWindow& window)
{
	circle.setRadius(20.0f);
	circle.setFillColor(state ? sf::Color(209, 0, 0) : sf::Color::White);
	circle.setOutlineThickness(3.0f);
	circle.setOutlineColor(sf::Color::Black);
	circle.setOrigin(circle.getRadius(), circle.getRadius());
	circle.setPosition(position);

	window.draw(circle);
}