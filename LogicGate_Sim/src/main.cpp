#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>
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

	std::string currentDir = fs::current_path().string();

	// Initialize ImGui-SFML
	ImGui::SFML::Init(window);
	applyStyle();
#pragma endregion

#pragma region Objects
	Simulation simulation(currentDir + "\\src", 100u);
	simulation.zoom(window);

	//Components components = simulation.getComponents();
	//load(currentDir + "\\Saves\\Untitled.json", components, simulation.spacing, simulation.arial);
#pragma endregion

#pragma region Main loop
	sf::Clock deltaClock;
	std::string name = "Untitled";
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

		static bool addElementcollapsed;
		float addElementHeight = (addElementcollapsed) ? (float)window.getSize().y - ImGui::GetFrameHeight() : (float)window.getSize().y - 100;
		{
			ImGui::SetNextWindowSize({ (float)window.getSize().x, 100 });
			ImGui::SetNextWindowPos(ImVec2(0, addElementHeight));

			ImGui::Begin("Add elements", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

			addElementcollapsed = ImGui::IsWindowCollapsed();

			if (!addElementcollapsed) {
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

		{
			ImGui::SetNextWindowPos({ 0, 0 });
			ImGui::Begin("Circuit", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Text(simulation.analyze().c_str());


			if (ImGui::Button("Import")) {
				Components components = simulation.getComponents();
				//load(currentDir + "\\Saves\\4Bit-Adder.json", components, simulation.spacing, simulation.arial);
				load(currentDir + "\\Saves\\4Bit-Adder.json", components, simulation.spacing, simulation.arial);
			}
			if (ImGui::Button("Import As Gate")) {
				Components components = simulation.getComponents();
				//loadasCustom(currentDir + "\\Saves\\4Bit-Adder.json", components, simulation.spacing, simulation.arial);
				loadasCustom(currentDir + "\\Saves\\4Bit-Adder.json", components, simulation.spacing, simulation.arial);
			}

			ImGui::End();
		}

		static bool chipPreviewcollapsed;
		float chipPreviewHeight = (chipPreviewcollapsed) ? addElementHeight - ImGui::GetFrameHeight() : addElementHeight - 100;
		{
			ImGui::SetNextWindowSize({ 150, 100 });
			ImGui::SetNextWindowPos({ (float)window.getSize().x - 150, chipPreviewHeight});
			ImGui::Begin("Chip preview", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

			ImGui::InputText("Name", &name, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll);

			if (ImGui::Button("Create")) {
				std::cout << name << std::endl;
				save(currentDir + "\\Saves\\" + name + ".json", simulation.getComponents());
				name = "Untitled";
			}

			chipPreviewcollapsed = ImGui::IsWindowCollapsed();

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