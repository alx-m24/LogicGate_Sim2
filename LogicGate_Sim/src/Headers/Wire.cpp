#include "Wire.hpp"

void Wire::update()
{
	if (input != nullptr && output != nullptr)
		*output = *input;
}

void Wire::draw(sf::RenderWindow& window) const
{
	sf::VertexArray line(sf::PrimitiveType::Lines);
	sf::Color c1 = sf::Color(147, 145, 150);
	sf::Color c2 = sf::Color(147, 145, 150);

	if (input != nullptr) c1 = (*input) ? sf::Color(219, 22, 47) : sf::Color(147, 145, 150);
	if (output != nullptr) c2 = (*output) ? sf::Color(219, 22, 47) : sf::Color(147, 145, 150);

	if (p1 != nullptr) line.append(sf::Vertex(*p1, c1));
	if (p2 != nullptr) line.append(sf::Vertex(*p2, c2));

	window.draw(line);
}

bool Wire::getState() const
{
	if (input != nullptr && output != nullptr)
		return *input;

	return false;
}
