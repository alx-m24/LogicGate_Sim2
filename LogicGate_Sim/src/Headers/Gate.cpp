#include "Gate.hpp"

Connector::Connector()
{
	circle.setFillColor(sf::Color::White);
	circle.setOutlineThickness(2.0f);
	circle.setOutlineColor(sf::Color::Black);
}

bool Connector::contains(sf::Vector2f point)
{
	return circle.getGlobalBounds().contains(point);
}

void Connector::draw(sf::RenderWindow& window, sf::Vector2f origin, float spacing)
{
	gloabalPosition = origin + localPosition;

	circle.setRadius(spacing / 8.0f);
	circle.setOrigin(circle.getRadius(), circle.getRadius());
	circle.setPosition(gloabalPosition);

	window.draw(circle);
}

void Gate::drawConnectors(sf::RenderWindow& window, float spacing)
{
	sf::Color color = isSelected ? sf::Color(127, 127, 127) : sf::Color::White;
	for (Connector& connector : inputs) {
		connector.setFillColor(color);
		connector.draw(window, position, spacing);
	}
	for (Connector& connector : outputs) {
		connector.setFillColor(color);
		connector.draw(window, position, spacing);
	}
}

void Gate::draw(sf::RenderWindow& window, float spacing)
{
	drawConnectors(window, spacing);
	drawText(window, spacing);

	isSelected = false;
}

void Gate::drawText(sf::RenderWindow& window, float spacing) const
{
	sf::Text text;
	text.setFont(font);
	text.setFillColor(sf::Color::Black);
	text.setString(name);
	text.setCharacterSize(spacing / 2.0f);
	text.setPosition(position - sf::Vector2f(spacing / 2.0f, spacing / 3.5f));

	window.draw(text);
}

void Gate::setuniforms(sf::Shader& shader, std::string prefix, float spacing)
{
	rect.setPosition(position);

	if (isSelected) {
		rect.setFillColor(sf::Color(static_cast<sf::Uint8>(color.r / 2.0f),
			static_cast<sf::Uint8>(color.g / 2.0f),
			static_cast<sf::Uint8>(color.b / 2.0f)));
	}
	else rect.setFillColor(color);

	shader.setUniform(prefix + "color", sf::Vector3f(static_cast<sf::Uint8>(rect.getFillColor().r), static_cast<sf::Uint8>(rect.getFillColor().g), static_cast<sf::Uint8>(rect.getFillColor().b)) / 255.0f);
	shader.setUniform(prefix + "outlineColor", sf::Vector3f(static_cast<sf::Uint8>(rect.getOutlineColor().r), static_cast<sf::Uint8>(rect.getOutlineColor().g), static_cast<sf::Uint8>(rect.getOutlineColor().b)) / 255.0f);
	shader.setUniform(prefix + "position", position);
	shader.setUniform(prefix + "halfSize", rect.getSize() / 2.0f);
	shader.setUniform(prefix + "radius", spacing / 5.0f);
	shader.setUniform(prefix + "outlineThickness", rect.getOutlineThickness());
}

bool Gate::contains(sf::Vector2f point)
{
	return rect.getGlobalBounds().contains(point);
}
