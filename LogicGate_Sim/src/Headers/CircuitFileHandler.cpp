#include "CircuitFileHandler.hpp"

std::pair<std::vector<unsigned int>, std::vector<unsigned int>> analyzeNodes(std::vector<Node*>& nodes, std::vector<Wire>& wires, std::vector<Gate*>& gates)
{
    std::pair<std::vector<unsigned int>, std::vector<unsigned int>> analyzedNodes;

    std::unordered_set<bool*> outputNodes;
    std::unordered_set<bool*> inputNodes;

    for (const Wire& wire : wires) {
        inputNodes.insert(wire.input);
        outputNodes.insert(wire.output);
    }

    for (Gate* gate : gates) {
        for (Node* node : gate->custom_inputs) {
            void* tempPtr = node;
            bool* nodePtr = static_cast<bool*>(tempPtr);

            outputNodes.insert(nodePtr);
        }
        for (Node* node : gate->custom_outputs) {
            void* tempPtr = node;
            bool* nodePtr = static_cast<bool*>(tempPtr);

            inputNodes.insert(nodePtr);
        }
    }

    for (int i = 0; i < nodes.size(); ++i) {
        void* tempPtr = nodes[i];
        bool* nodePtr = static_cast<bool*>(tempPtr);

        // If found in output nodes and NOT in input nodes
        if (outputNodes.find(nodePtr) != outputNodes.end() && (inputNodes.find(nodePtr) == inputNodes.end())) {
            analyzedNodes.second.push_back(i);
        }
        // If found in input nodes and NOT in output nodes
        else if (inputNodes.find(nodePtr) != inputNodes.end() && (outputNodes.find(nodePtr) == outputNodes.end())) {
            analyzedNodes.first.push_back(i);
        }
    }

    std::sort(analyzedNodes.first.begin(), analyzedNodes.first.end(),
        [&nodes](const unsigned int& a, const unsigned int& b) {
            return nodes[a]->position.y < nodes[b]->position.y;
        });

    std::sort(analyzedNodes.second.begin(), analyzedNodes.second.end(),
        [&nodes](const unsigned int& a, const unsigned int& b) {
            return nodes[a]->position.y < nodes[b]->position.y;
        });

    return analyzedNodes;
}

std::vector<std::string> getCustomGates(std::string path)
{
    std::vector<std::string> gates;

    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            gates.push_back(entry.path().stem().string());
        }
    }

    return gates;
}

AnalyzedCiruit analyzeCircuit(const Components& components)
{
    AnalyzedCiruit circuit;

    circuit.wireNum = components.wires.size();
    circuit.nodeNum = components.nodes.size();
    circuit.gateNum = components.gates.size();
    circuit.total = circuit.wireNum + circuit.nodeNum + circuit.gateNum;

    circuit.analyzedNodes = analyzeNodes(components.nodes, components.wires, components.gates);

    circuit.inputNodesNum = circuit.analyzedNodes.first.size();
    circuit.outputNodesNum = circuit.analyzedNodes.second.size();

    return circuit;
}

std::string toString(const AnalyzedCiruit& circuit)
{
    std::string text;

    text += "Components num: " + std::to_string(circuit.total);
    text += "\nWires: " + std::to_string(circuit.wireNum);
    text += "\nGates: " + std::to_string(circuit.gateNum);
    text += "\nNodes: " + std::to_string(circuit.nodeNum);
    text += "\nInput Nodes num: " + std::to_string(circuit.inputNodesNum);
    text += "\nOutput Nodes num: " + std::to_string(circuit.outputNodesNum);

    text += "\nInput nodes: ";

    for (int i = 0; i < circuit.inputNodesNum; ++i) {
        text += std::to_string(circuit.analyzedNodes.first[i]);
        if (i < circuit.inputNodesNum - 1) text += ", ";
    }

    text += "\nOutput nodes: ";

    for (int i = 0; i < circuit.outputNodesNum; ++i) {
        text += std::to_string(circuit.analyzedNodes.second[i]);
        if (i < circuit.outputNodesNum - 1) text += ", ";
    }

    return text;
}

void reorderNodes(std::vector<Node*>& nodes)
{
    std::stable_partition(nodes.begin(), nodes.end(), [](Node* node) {
        return !node->shouldDraw;
        });
}

void load(std::string path, std::string name, Components& components, float spacing, sf::Font& arial)
{
    loadFromPath(path + name + ".json", components, spacing, arial);
}

void loadasCustom(std::string path, std::string name, Components& components, float spacing, sf::Font& arial)
{
    json data;
    std::ifstream file(path + name + ".json");
    if (file.is_open()) {
        file >> data;
        file.close();
    }
    else {
        std::cout << "Couldn't open: " << path + name + ".json" << std::endl;
    }

    unsigned int gateIdxOffset = components.gates.size();
    unsigned int originalNodeOffset = components.nodes.size();

    for (json gate : data["Gates"]) {
        std::string name = gate["Name"];

        if (name == "NOT") components.gates.emplace_back(new NotGate(spacing, arial));
        else if (name == "AND") components.gates.emplace_back(new AndGate(spacing, arial));
        else if (name == "OR") components.gates.emplace_back(new OrGate(spacing, arial));
        else if (name == "XOR") components.gates.emplace_back(new XOrGate(spacing, arial));
        else {
            loadasCustom(path, name, components, spacing, arial);
        }

        components.gates.back()->position = { gate["Position"][0], gate["Position"][1] };
        components.gates.back()->shouldDraw = false;
    }

    unsigned int nodeIdxOffset = components.nodes.size();

    for (json node : data["Nodes"]) {
        components.nodes.emplace_back(new Node());
        components.nodes.back()->position = { node["Position"][0], node["Position"][1] };
        components.nodes.back()->state = node["State"];
        components.nodes.back()->shouldDraw = false;
    }

    unsigned int WireIdxOffset = components.wires.size();

    for (json wire : data["Wires"]) {
        sf::Vector2f* p1 = nullptr;
        sf::Vector2f* p2 = nullptr;
        bool* input = nullptr;
        bool* output = nullptr;

        if (wire["inputNodeIdx"] != -1) {
            p1 = &components.nodes[nodeIdxOffset + wire["inputNodeIdx"]]->position;
            input = &components.nodes[nodeIdxOffset + wire["inputNodeIdx"]]->state;
        }
        else if (wire["gateInputIdx"] != -1) {
            Connector& connector = components.gates[wire["gateInputIdx"] + gateIdxOffset]->outputs[wire["inputConnectorIdx"]];
            p1 = &connector.gloabalPosition;
            input = &connector.state;
        }

        if (wire["outputNodeIdx"] != -1) {
            p2 = &components.nodes[nodeIdxOffset + wire["outputNodeIdx"]]->position;
            output = &components.nodes[nodeIdxOffset + wire["outputNodeIdx"]]->state;
        }
        else if (wire["gateOutputIdx"] != -1) {
            Connector& connector = components.gates[wire["gateOutputIdx"] + gateIdxOffset]->inputs[wire["OutputConnectorIdx"]];
            p2 = &connector.gloabalPosition;
            output = &connector.state;
        }

        components.wires.emplace_back(Wire(p1, p2));
        components.wires.back().input = input;
        components.wires.back().output = output;
        components.wires.back().shouldDraw = false;
    }

    components.gates.push_back(new Custom(data["InputNodeNum"], data["OutputNodeNum"], spacing, data["Name"], sf::Color(data["Color"][0], data["Color"][1], data["Color"][2]), arial));

    int connectorIdx = 0;
    for (int i : data["InputNode"]) {
        components.gates.back()->custom_inputs[connectorIdx] = components.nodes[originalNodeOffset + i];

        ++connectorIdx;
    }

    connectorIdx = 0;
    for (int i : data["OutputNode"]) {
        components.gates.back()->custom_outputs[connectorIdx] = components.nodes[originalNodeOffset + i];

        ++connectorIdx;
    }
}

void loadFromPath(std::string path, Components& components, float spacing, sf::Font& arial)
{
    json data;
    std::ifstream file(path);
    if (file.is_open()) {
        file >> data;
        file.close();
    }
    else {
        std::cout << "Couldn't open: " << path << std::endl;
    }

    path = path.substr(0, path.length() - std::string(".json").length() - std::string(data["Name"]).length());

    unsigned int gateIdxOffset = components.gates.size();

    for (json gate : data["Gates"]) {
        std::string name = gate["Name"];

        if (name == "NOT") components.gates.emplace_back(new NotGate(spacing, arial));
        else if (name == "AND") components.gates.emplace_back(new AndGate(spacing, arial));
        else if (name == "OR") components.gates.emplace_back(new OrGate(spacing, arial));
        else if (name == "XOR") components.gates.emplace_back(new XOrGate(spacing, arial));
        else {
            loadasCustom(path, name, components, spacing, arial);
        }

        components.gates.back()->position = { gate["Position"][0], gate["Position"][1] };
    }

    unsigned int nodeIdxOffset = components.nodes.size();

    for (json node : data["Nodes"]) {
        components.nodes.emplace_back(new Node());
        components.nodes.back()->position = { node["Position"][0], node["Position"][1] };
        components.nodes.back()->state = node["State"];
    }

    unsigned int WireIdxOffset = components.wires.size();

    for (json wire : data["Wires"]) {
        sf::Vector2f* p1 = nullptr;
        sf::Vector2f* p2 = nullptr;
        bool* input = nullptr;
        bool* output = nullptr;

        if (wire["inputNodeIdx"] != -1) {
            p1 = &components.nodes[nodeIdxOffset + wire["inputNodeIdx"]]->position;
            input = &components.nodes[nodeIdxOffset + wire["inputNodeIdx"]]->state;
        }
        else if (wire["gateInputIdx"] != -1) {
            Connector& connector = components.gates[wire["gateInputIdx"] + gateIdxOffset]->outputs[wire["inputConnectorIdx"]];
            p1 = &connector.gloabalPosition;
            input = &connector.state;
        }

        if (wire["outputNodeIdx"] != -1) {
            p2 = &components.nodes[nodeIdxOffset + wire["outputNodeIdx"]]->position;
            output = &components.nodes[nodeIdxOffset + wire["outputNodeIdx"]]->state;
        }
        else if (wire["gateOutputIdx"] != -1) {
            Connector& connector = components.gates[wire["gateOutputIdx"] + gateIdxOffset]->inputs[wire["OutputConnectorIdx"]];
            p2 = &connector.gloabalPosition;
            output = &connector.state;
        }

        components.wires.emplace_back(Wire(p1, p2));
        components.wires.back().input = input;
        components.wires.back().output = output;
    }
}

void save(std::string path, std::string name, const Components& components, ImVec4 color)
{
    reorderNodes(components.nodes);

    AnalyzedCiruit circuit = analyzeCircuit(components);

    json data = {
        {"ComponentNum", circuit.total},
        {"NodeNum", circuit.nodeNum},
        {"GateNum", circuit.gateNum},
        {"WireNum", circuit.wireNum},
        {"InputNodeNum", circuit.inputNodesNum},
        {"OutputNodeNum", circuit.outputNodesNum},
        {"InputNode", circuit.analyzedNodes.first},
        {"OutputNode", circuit.analyzedNodes.second},
        {"Name", name},
        {"Color", {color.x * 255.0f, color.y * 255.0f, color.z * 255.0f}}
    };

    json jsonNodes = json::array();
    for (Node* node : components.nodes) {
        if (!node->shouldDraw) continue;

        json jsonNode = {
            {"Position", {node->position.x, node->position.y}},
            {"State", node->state }
        };
        jsonNodes.push_back(jsonNode);
    }

    data["Nodes"] = jsonNodes;

    json jsonGates = json::array();
    for (Gate* gate : components.gates) {
        if (!gate->shouldDraw) continue;

        json jsonGate = {
            {"Position", {gate->position.x, gate->position.y}},
            {"Name", gate->name}
        };
        jsonGates.push_back(jsonGate);
    }

    data["Gates"] = jsonGates;

    struct FormattedWire {
        int inputNodeIdx = -1;
        int outputNodeIdx = -1;
        int gateInputIdx = -1;
        int inputConnectorIdx = -1;
        int gateOutputIdx = -1;
        int OutputConnectorIdx = -1;
    };
    std::vector<FormattedWire> formatedWires;

    for (const Wire& wire : components.wires) {
        if (!wire.shouldDraw) continue;

        FormattedWire currWire;

        int idx = 0;
        for (int i = 0; i < components.nodes.size(); ++i) {
            if (!components.nodes[i]->shouldDraw) continue;
            if (&components.nodes[i]->state == wire.input) {
                currWire.inputNodeIdx = idx;
            }
            else if (&components.nodes[i]->state == wire.output) {
                currWire.outputNodeIdx = idx;
            }
            ++idx;
        }

        for (int i = 0; i < components.gates.size(); ++i) {
            for (int j = 0; j < components.gates[i]->inputs.size(); ++j) {
                if (&components.gates[i]->inputs[j].state == wire.input) {
                    currWire.gateInputIdx = i;
                    currWire.inputConnectorIdx = j;
                }
                else if (&components.gates[i]->inputs[j].state == wire.output) {
                    currWire.gateOutputIdx = i;
                    currWire.OutputConnectorIdx = j;
                }
            }
            for (int j = 0; j < components.gates[i]->outputs.size(); ++j) {
                if (&components.gates[i]->outputs[j].state == wire.output) {
                    currWire.gateOutputIdx = i;
                    currWire.OutputConnectorIdx = j;
                }
                else if (&components.gates[i]->outputs[j].state == wire.input) {
                    currWire.gateInputIdx = i;
                    currWire.inputConnectorIdx = j;
                }
            }
        }

        formatedWires.push_back(currWire);
    }

    json jsonWires = json::array();
    for (const FormattedWire& wire : formatedWires) {
        json jsonWire = {
            {"inputNodeIdx", wire.inputNodeIdx},
            {"outputNodeIdx", wire.outputNodeIdx},
            {"gateInputIdx", wire.gateInputIdx},
            {"inputConnectorIdx", wire.inputConnectorIdx},
            {"gateOutputIdx", wire.gateOutputIdx},
            {"OutputConnectorIdx", wire.OutputConnectorIdx},
        };
        jsonWires.push_back(jsonWire);
    }

    data["Wires"] = jsonWires;

    std::ofstream file(path + name + ".json");
    file.clear();
    if (file.is_open()) {
        file << data.dump(4);
        file.close();
    }
}

