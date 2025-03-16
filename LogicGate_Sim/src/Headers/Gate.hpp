#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <iostream>

class Connector {
public:
	bool state = false;
	sf::Vector2f localPosition;
	sf::Vector2f gloabalPosition;

public:
	Connector();

public:
	bool contains(sf::Vector2f point);
	void draw(sf::RenderWindow& window, sf::Vector2f origin, float spacing);
	void setFillColor(sf::Color color) { circle.setFillColor(color); }
	
private:
	sf::CircleShape circle;
};

class Gate
{
public:
	std::vector<Connector> inputs;
	std::vector<Connector> outputs;

public:
	sf::Vector2f position;
	bool isSelected = false;

protected:
	const std::string name;
	sf::RectangleShape rect;
	sf::Color color;
	sf::Font& font;

public:
	Gate(std::string name, sf::Color color, sf::Font& font) : name(name), color(color), font(font) {};

public:
	void drawConnectors(sf::RenderWindow& window, float spacing);
	void draw(sf::RenderWindow& window, float spacing);
	void drawText(sf::RenderWindow& window, float spacing) const;
	void setuniforms(sf::Shader& shader, std::string prefix, float spacing);
	bool contains(sf::Vector2f point);
	virtual void update() {};
	virtual void resize(float spacing) {};
};

class NotGate : public Gate {
public:
	NotGate(float spacing, sf::Font& font) : Gate("NOT",sf::Color(219, 22, 47), font) {
		inputs.resize(1);
		outputs.resize(1);

		resize(spacing);
	};

public:
	virtual void update() {
		outputs[0].state = !inputs[0].state;
	}
	virtual void resize(float spacing) {
		spacing *= 1.5f;

		rect.setSize(sf::Vector2f(spacing, spacing / 2.0f));
		rect.setOrigin(rect.getSize() / 2.0f);
		rect.setOutlineThickness(2.0f);
		rect.setOutlineColor(sf::Color::Black);

		inputs[0].localPosition.x = -rect.getSize().y;
		outputs[0].localPosition.x = rect.getSize().y;
	}
};

class AndGate : public Gate {
public:
	AndGate(float spacing, sf::Font& font) : Gate("AND", sf::Color(2, 103, 193), font) {
		inputs.resize(2);
		outputs.resize(1);

		resize(spacing);
	};

public:
	virtual void update() {
		outputs[0].state = inputs[0].state && inputs[1].state;
	}

	virtual void resize(float spacing) {
		spacing *= 1.5f;

		rect.setSize(sf::Vector2f(spacing, spacing / 2.0f));
		rect.setOrigin(rect.getSize() / 2.0f);
		rect.setOutlineThickness(2.0f);
		rect.setOutlineColor(sf::Color::Black);

		inputs[0].localPosition.x = -rect.getSize().y;
		inputs[1].localPosition.x = -rect.getSize().y;
		outputs[0].localPosition.x = rect.getSize().y;

		inputs[0].localPosition.y = -rect.getSize().y / 2.0f + spacing / 8.5f;
		inputs[1].localPosition.y = rect.getSize().y / 2.0f - spacing / 8.5f;
	}
};

class OrGate : public Gate {
public:
	OrGate(float spacing, sf::Font& font) : Gate("OR", sf::Color(255, 169, 41), font) {
		inputs.resize(2);
		outputs.resize(1);

		resize(spacing);
	};

public:
	virtual void update() {
		outputs[0].state = inputs[0].state || inputs[1].state;
	}

	virtual void resize(float spacing) {
		spacing *= 1.5f;

		rect.setSize(sf::Vector2f(spacing, spacing / 2.0f));
		rect.setOrigin(rect.getSize() / 2.0f);
		rect.setOutlineThickness(2.0f);
		rect.setOutlineColor(sf::Color::Black);

		inputs[0].localPosition.x = -rect.getSize().y;
		inputs[1].localPosition.x = -rect.getSize().y;
		outputs[0].localPosition.x = rect.getSize().y;

		inputs[0].localPosition.y = -rect.getSize().y / 2.0f + spacing / 8.5f;
		inputs[1].localPosition.y = rect.getSize().y / 2.0f - spacing / 8.5f;
	}
};