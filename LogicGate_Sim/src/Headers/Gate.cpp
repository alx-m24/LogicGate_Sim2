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
	for (Connector& connector : inputs) connector.draw(window, position, spacing);
	for (Connector& connector : outputs) connector.draw(window, position, spacing);
}

void Gate::draw(sf::RenderWindow& window, float spacing)
{
	rect.setPosition(position);

	if (isSelected) {
		rect.setFillColor(sf::Color(static_cast<sf::Uint8>(color.r / 2.0f),
			static_cast<sf::Uint8>(color.g / 2.0f),
			static_cast<sf::Uint8>(color.b / 2.0f)));
	}
	else rect.setFillColor(color);

	isSelected = false;

	window.draw(rect);
	drawConnectors(window, spacing);

	sf::Text text;
	text.setFont(font);
	text.setFillColor(sf::Color::Black);
	text.setString(name);
	text.setCharacterSize(spacing / 2.0f);
	text.setPosition(position - sf::Vector2f(spacing / 2.0f, spacing / 3.5f));

	window.draw(text);
}

bool Gate::contains(sf::Vector2f point)
{
	return rect.getGlobalBounds().contains(point);
}
