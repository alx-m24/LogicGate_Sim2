#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>
#include "Headers/Simulation.hpp"
#include "Headers/gui/Style/myStyle.hpp"

namespace fs = std::filesystem;
std::map<sf::Keyboard::Key, bool> keys = std::map<sf::Keyboard::Key, bool>();

int main() {
#pragma region Initialize
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	sf::RenderWindow window(sf::VideoMode(1000, 600), "LogicGate Simulator", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	std::string currentDir = fs::current_path().string() + "\\src";

	// Initialize ImGui-SFML
	ImGui::SFML::Init(window);
	applyStyle();
#pragma endregion

#pragma region Objects
	Simulation simulation(currentDir, 100u);
	simulation.zoom(window);
#pragma endregion

#pragma region Main loop
	sf::Clock deltaClock;
	while (window.isOpen()) {
#pragma region Input
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);  // Pass events to ImGui
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				window.setView(sf::View(sf::FloatRect(0.0f, 0.0f, static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y))));
				break;
			case sf::Event::MouseWheelScrolled:
				simulation.gridSize -= event.mouseWheelScroll.delta;
				simulation.zoom(window);
				break;
			case sf::Event::KeyPressed:
				keys[event.key.code] = true;
				break;
			case sf::Event::KeyReleased:
				keys[event.key.code] = false;
				break;
			default:
				break;
			}
		}
#pragma endregion

#pragma region Update
		ImGui::SFML::Update(window, deltaClock.restart());
		simulation.update(window);

		{
			static bool collapsed;

			ImGui::SetNextWindowSize({ (float)window.getSize().x, 100 });

			if (!collapsed) ImGui::SetNextWindowPos(ImVec2(0, (float)window.getSize().y - 100));
			else ImGui::SetNextWindowPos(ImVec2(0, (float)window.getSize().y - ImGui::GetFrameHeight()));

			ImGui::Begin("Add elements", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

			collapsed = ImGui::IsWindowCollapsed();

			if (!collapsed) {
				if (ImGui::Button("Node")) {
					simulation.addNode(window);
				}

				std::vector<Gate> gates = simulation.getGates();

				for (Gate& gate : gates) {
					ImGui::SameLine();
					if (ImGui::Button(gate.name.c_str())) {
						simulation.addGate(gate.name, window);
					}
				}
			}
			ImGui::End();
		}
#pragma endregion

#pragma region Display
		window.clear();

		simulation.draw(window);
		ImGui::SFML::Render(window);

		window.display();
#pragma endregion
	}
#pragma endregion

#pragma region Terminate
	ImGui::SFML::Shutdown();
	return EXIT_SUCCESS;
#pragma endregion
}