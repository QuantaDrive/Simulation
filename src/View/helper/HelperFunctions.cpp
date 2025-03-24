//
// Created by renat on 10-3-2025.
//

#include "HelperFunctions.h"

#include <fstream>
#include <iostream>

#include "../../Domain/Instruction.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

const domain::Node *
NodeHelpers::findStartNode(const std::vector<domain::Node> &nodes, const ImVector<LinkInfo> &links) {
    if (nodes.empty()) return nullptr;

    std::vector<const domain::Node *> startNodes;
    for (const auto &node: nodes) {
        // A start node should have no input connections but must have output connections
        if (!hasInputConnection(node, links) && hasOutputConnection(node, links)) {
            startNodes.push_back(&node);
        }
    }

    // If no valid start nodes found, try to find any node without inputs
    if (startNodes.empty()) {
        for (const auto &node: nodes) {
            if (!hasInputConnection(node, links)) {
                startNodes.push_back(&node);
            }
        }
    }

    // If still no start nodes found, return nullptr
    if (startNodes.empty()) return nullptr;

    // If multiple start nodes exist, use the earliest created one
    return *std::min_element(startNodes.begin(), startNodes.end(),
                             [](const domain::Node *a, const domain::Node *b) {
                                 return static_cast<int>(static_cast<uintptr_t>(a->getNodeId())) <
                                        static_cast<int>(static_cast<uintptr_t>(b->getNodeId()));
                             });
}

const domain::Node *NodeHelpers::findNextNode(const domain::Node *currentNode, const std::vector<domain::Node> &nodes,
                                              const ImVector<LinkInfo> &links) {
    for (const auto &link: links) {
        if (link.InputId == currentNode->getNodeOutputPinId()) {
            for (const auto &node: nodes) {
                if (link.OutputId == node.getNodeInputPinId()) {
                    std::cout << "Next node: " << node.getTitle() << std::endl;
                    return &node;
                }
            }
        }
    }
    return nullptr;
}

bool NodeHelpers::isStartNode(const domain::Node &node, const ImVector<LinkInfo> &links) {
    //If there are no links at all, any node is a start node
    if (links.empty()) {
        return true;
    }

    //If the node has no connections at all, it's a start node
    if (!hasInputConnection(node, links) && !hasOutputConnection(node, links)) {
        return true;
    }

    // Node should have no input connections but have output connections (Node Chain)
    return !hasInputConnection(node, links) && hasOutputConnection(node, links);
}

bool NodeHelpers::hasInputConnection(const domain::Node &node, const ImVector<LinkInfo> &links) {
    for (const auto &link: links) {
        if (link.OutputId == node.getNodeInputPinId()) {
            return true;
        }
    }
    return false;
}

bool NodeHelpers::hasOutputConnection(const domain::Node &node, const ImVector<LinkInfo> &links) {
    for (const auto &link: links) {
        if (link.InputId == node.getNodeOutputPinId()) {
            return true;
        }
    }
    return false;
}

void NodeHelpers::renderNodeTooltip(RobotActions::NodeActivation action) {
    switch (action) {
        case RobotActions::NodeActivation::Relative:
            ImGui::Text("All following input nodes (Rapid or Linear) will move relative to the arm's current position");
            break;

        case RobotActions::NodeActivation::Absolute:
            ImGui::Text("All following input nodes (Rapid or Linear) will move to position specified by those nodes");
            break;

        case RobotActions::NodeActivation::AngleHead:
            ImGui::Text("Rotate the robot's head to a specific angle");
            ImGui::Text("Specify rotation angles in degrees");
            break;

        case RobotActions::NodeActivation::RapidMove:
            ImGui::Text("Move the robot at maximum speed");
            ImGui::Text("Use for quick positioning where precision isn't critical");
            break;

        case RobotActions::NodeActivation::LinearMove:
            ImGui::Text("Move the robot in a straight line");
            ImGui::Text("Maintains constant speed and precise path");
            break;

        case RobotActions::NodeActivation::LoopStart:
            ImGui::Text("Start a loop sequence");
            ImGui::Text("Specify number of iterations");
            break;

        case RobotActions::NodeActivation::LoopEnd:
            ImGui::Text("End a loop sequence");
            ImGui::Text("Must be connected to a Loop Start node");
            break;

        case RobotActions::NodeActivation::Wait:
            ImGui::Text("Pause execution for specified time");
            ImGui::Text("Duration in seconds");
            break;

        case RobotActions::NodeActivation::Home:
            ImGui::Text("Return robot to home position");
            ImGui::Text("Safe position with defined coordinates");
            break;

        default:
            ImGui::Text("Unknown action type");
            break;
    }
}

void NodeHelpers::calcRandomPosNextNode(ImVec2 *m_NextNodePosition) {
    float randomXNumber = (rand() % 100) - 30;
    float randomYNumber = (rand() % 100) - 30;
    m_NextNodePosition->x += randomXNumber;
    m_NextNodePosition->y += randomYNumber;
}

void NodeHelpers::renderHelpText() {
    ImGui::Text("Basic Controls:");
    ImGui::BulletText("Left-click and drag: Move nodes");
    ImGui::BulletText("Left-click on pin and drag: Create connections");
    ImGui::BulletText("Left-click on node or link and press Delete: Delete");
    ImGui::BulletText("Ctrl + C while node selected: Copy node");
    ImGui::Spacing();

    ImGui::Text("Creating Nodes:");
    ImGui::BulletText("Click buttons in Node Selector to create nodes");
    ImGui::BulletText("New nodes are placed at random positions");
    ImGui::BulletText("Hover over node buttons for detailed descriptions");
    ImGui::Spacing();

    ImGui::Text("Node Types:");
    ImGui::BulletText("Relative Move: Move robot relative to current position");
    ImGui::BulletText("Absolute Move: Move robot to exact coordinates");
    ImGui::BulletText("Loop Start/End: Create repeating sequences");
    ImGui::BulletText("Wait: Pause execution for specified duration");
    ImGui::BulletText("Home: Return robot to home position");
    ImGui::Spacing();

    ImGui::Text("Connections:");
    ImGui::BulletText("Connect 'Out' to 'In' pins to create sequence");
    ImGui::BulletText("Each node can have one input and one output");
    ImGui::BulletText("Cannot create loops or self-connections");
    ImGui::Spacing();

    ImGui::Text("Execution:");
    ImGui::BulletText("Click 'Send Instructions' to execute node sequence");
    ImGui::BulletText("Sequence starts from node with no input connection");
    ImGui::BulletText("Nodes execute in order of connections");
}

void NodeHelpers::renderNodeControls(domain::Node &node, SimulationManager *simulationManager,
                                     std::function<void(const std::string &)> showInfoCallback) {
    switch (node.getActivation()) {
        case RobotActions::NodeActivation::LoopStart: {
            ImGui::PushItemWidth(100);
            std::string idTitle = "loop " + node.getNodeId().Get();
            ImGui::PushID(idTitle.c_str());

            ImGui::Text("# loops");
            int loopCount = node.getLoopCount();
            ImGui::InputInt("", &loopCount);
            ImGui::PopID();
            ImGui::PopItemWidth();
            node.setLoopCount(loopCount);
            break;
        }
        case RobotActions::NodeActivation::Wait: {
            std::string idTitle = "wait " + node.getNodeId().Get();
            ImGui::PushID(idTitle.c_str());
            ImGui::PushItemWidth(100);
            ImGui::Text("s");
            int waitTimer = node.getWaitTimer();
            ImGui::InputInt("", &waitTimer);

            ImGui::PopID();
            ImGui::PopItemWidth();
            node.setWaitTimer(waitTimer);
            break;
        }
        case RobotActions::NodeActivation::LinearMove:
        case RobotActions::NodeActivation::RapidMove: {
            std::string idTitle = "linear_or_rapid_move " + node.getNodeId().Get();
            ImGui::PushID(idTitle.c_str());
            ImGui::PushItemWidth(100);

            // Get current coordinates move values
            glm::vec3 posCoords = node.getPosition()->getCoords();
            float xCoord = posCoords.x;
            float yCoord = posCoords.y;
            float zCoord = posCoords.z;
            int linearVelocity = node.getVelocity();

            // Create sliders for X, Y, Z
            bool changed = false;
            changed |= ImGui::InputFloat("X", &xCoord, -200.0f, 200.0f);
            changed |= ImGui::InputFloat("Y", &yCoord, -200.0f, 200.0f);
            changed |= ImGui::InputFloat("Z", &zCoord, -200.0f, 200.0f);

            if (node.getActivation() == RobotActions::NodeActivation::LinearMove) {
                changed |= ImGui::SliderInt("Velocity", &linearVelocity, 0, 10);
            }

            // Update values and trigger preview if changed
            if (changed) {
                auto *position = new domain::Position(vec3(xCoord, yCoord, zCoord), vec3(0, 0, 0));
                node.setPosition(position);
                if (node.getActivation() == RobotActions::NodeActivation::LinearMove) {
                    node.setVelocity(linearVelocity);
                }
                // Check if this node is currently selected
                ed::NodeId selectedNodeId;
                if (ed::GetSelectedNodes(&selectedNodeId, 1) && node.getNodeId() == selectedNodeId) {
                    if (!simulationManager->startPreview(position)) {
                        showInfoCallback("Error: current position is unreachable");
                    } else {
                        showInfoCallback("Previewing movement to new position");
                    }
                }
            }

            ImGui::PopID();
            ImGui::PopItemWidth();
            break;
        }
        case RobotActions::NodeActivation::AngleHead: {
            std::string idTitle = "angleHead" + node.getNodeId().Get();
            ImGui::PushID(idTitle.c_str());
            ImGui::PushItemWidth(100);

            // create input for yaw, pitch, roll
            float yaw = node.getRotationHead().x;
            float pitch = node.getRotationHead().y;
            float roll = node.getRotationHead().z;
            bool changed = false;

            changed |= ImGui::InputFloat("Yaw", &yaw, -200.0f, 200.0f);
            changed |= ImGui::InputFloat("Pitch", &pitch, -200.0f, 200.0f);
            changed |= ImGui::InputFloat("Roll", &roll, -200.0f, 200.0f);
            // Update values if changed
            if (changed) {
                node.setRotationHead(vec3(yaw, pitch, roll));
            }
            ImGui::PopID();
            ImGui::PopItemWidth();
            break;
        }
        default:
            break;
    }
}

void NodeHelpers::executeNode(const domain::Node &node, SimulationManager *simulationManager) {
    if (!simulationManager) return;

    switch (node.getActivation()) {
        case RobotActions::NodeActivation::Absolute:
            simulationManager->setAbsolute(true);
            break;

        case RobotActions::NodeActivation::Relative:
            simulationManager->setAbsolute(false);
            break;

        case RobotActions::NodeActivation::LinearMove: {
            auto *instruction = new domain::Instruction();
            instruction->setPosition(node.getPosition());
            instruction->setVelocity(node.getVelocity());
            instruction->setLinear(true);
            simulationManager->executeInstruction(instruction);
            delete instruction;
            break;
        }

        case RobotActions::NodeActivation::RapidMove: {
            auto *instruction = new domain::Instruction();
            instruction->setPosition(node.getPosition());
            instruction->setRapid(true);
            simulationManager->executeInstruction(instruction);
            delete instruction;
            break;
        }

        case RobotActions::NodeActivation::Wait: {
            auto *instruction = new domain::Instruction();
            instruction->setRelative(false);
            instruction->setWait(node.getWaitTimer());
            simulationManager->executeInstruction(instruction);
            delete instruction;
            break;
        }

        case RobotActions::NodeActivation::Home: {
            auto *instruction = new domain::Instruction();
            instruction->setRelative(false);
            instruction->setGoHome(true);
            simulationManager->executeInstruction(instruction);
            delete instruction;
            break;
        }

        case RobotActions::NodeActivation::AngleHead:
            simulationManager->setRotationOfHead(node.getRotationHead());
            break;

        default:
            break;
    }
}

bool NodeHelpers::shouldRemoveLink(const NodeHelpers::LinkInfo &link, const domain::Node &node) {
    return link.InputId == node.getNodeInputPinId() ||
           link.OutputId == node.getNodeOutputPinId();
}

void NodeHelpers::handleNodeCopy(std::vector<domain::Node> &nodes, int &nextNodeId, ImVec2 &nextNodePosition) {
    ed::NodeId selectedNodeId;
    if (ed::GetSelectedNodes(&selectedNodeId, 1) &&
        ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C)) {
        auto nodeIt = std::find_if(nodes.begin(), nodes.end(),
                                   [selectedNodeId](const domain::Node &node) {
                                       return node.getNodeId() == selectedNodeId;
                                   });

        if (nodeIt != nodes.end()) {
            domain::Node newNode = *nodeIt;
            int currentId = nextNodeId;
            newNode.initializeNodeIds(currentId);
            nextNodeId = currentId;

            nextNodePosition.x += 50.0f;
            nextNodePosition.y += 50.0f;

            nodes.push_back(newNode);
        }
    }
}

bool NodeHelpers::validateNewLink(const domain::Node *sourceNode, const domain::Node *targetNode) {
    if (!sourceNode || !targetNode) return false;
    if (sourceNode == targetNode) return false;

    // Additional validation rules can be added here
    return true;
}

void NodeHelpers::deleteNodeAndConnectedLinks(ed::NodeId nodeId, std::vector<domain::Node> &nodes,
                                              ImVector<NodeHelpers::LinkInfo> &links) {
    auto nodeIt = std::find_if(nodes.begin(), nodes.end(),
                               [nodeId](const domain::Node &node) {
                                   return node.getNodeId() == nodeId;
                               });

    if (nodeIt != nodes.end()) {
        ImVector<NodeHelpers::LinkInfo> newLinks;
        newLinks.reserve(links.size());

        for (const auto &link: links) {
            if (!shouldRemoveLink(link, *nodeIt)) {
                newLinks.push_back(link);
            }
        }

        links = newLinks;
        nodes.erase(nodeIt);
    }
}

void NodeHelpers::renderLinks(const ImVector<LinkInfo> &links) {
    for (auto &linkInfo: links) {
        ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);
    }
}

void NodeHelpers::handleNodeSelection(SimulationManager *simulationManager, vector<domain::Node> &nodes,
                                      ed::NodeId &lastSelectedNode) {
    ed::NodeId selectedNodeId;
    if (ed::GetSelectedNodes(&selectedNodeId, 1) == 1) {
        // Check if this is a new selection
        if (selectedNodeId != lastSelectedNode) {
            // Find the corresponding node in nodes vector
            auto it = std::find_if(nodes.begin(), nodes.end(),
                                   [selectedNodeId](const domain::Node &node) {
                                       return node.getNodeId() == selectedNodeId;
                                   });

            if (it != nodes.end()) {
                auto *position = it->getPosition();
                if (position) {
                    simulationManager->startPreview(position);
                }
            }
            lastSelectedNode = selectedNodeId;
        }
    } else {
        // No node selected, reset last selected node
        lastSelectedNode = ed::NodeId();
    }
}

void NodeHelpers::handleDeleteActions(ed::NodeId selectedNodeId,
                                      std::vector<domain::Node> &nodes,
                                      ImVector<LinkInfo> &links) {
    if (ed::GetSelectedNodes(&selectedNodeId, 1)) {
        deleteNodeAndConnectedLinks(selectedNodeId, nodes, links);
    }
}

void NodeHelpers::saveNodeEditor(
    const std::string &filename,
    const std::vector<domain::Node> &nodes,
    const ImVector<LinkInfo> &links
) {
    json j;

    // Save nodes
    json nodesArray = json::array();
    for (const auto &node: nodes) {
        json nodeObj;
        nodeObj["id"] = reinterpret_cast<uintptr_t>(node.getNodeId().AsPointer());
        nodeObj["title"] = node.getTitle();
        nodeObj["activation"] = static_cast<int>(node.getActivation());
        nodeObj["loopCount"] = node.getLoopCount();
        nodeObj["waitTimer"] = node.getWaitTimer();
        nodeObj["velocity"] = node.getVelocity();

        auto pos = node.getPosition();
        nodeObj["position"] = {
            {"x", pos->getCoords().x},
            {"y", pos->getCoords().y},
            {"z", pos->getCoords().z}
        };

        auto rot = node.getRotationHead();
        nodeObj["rotation"] = {
            {"x", rot.x},
            {"y", rot.y},
            {"z", rot.z}
        };

        nodesArray.push_back(nodeObj);
    }
    j["nodes"] = nodesArray;

    // Save links
    json linksArray = json::array();
    for (const auto &link: links) {
        json linkObj;
        linkObj["id"] = reinterpret_cast<uintptr_t>(link.Id.AsPointer());
        linkObj["inputId"] = reinterpret_cast<uintptr_t>(link.InputId.AsPointer());
        linkObj["outputId"] = reinterpret_cast<uintptr_t>(link.OutputId.AsPointer());
        linksArray.push_back(linkObj);
    }
    j["links"] = linksArray;

    // Write to file
    std::ofstream file(filename);
    file << j.dump(4);
}

void NodeHelpers::loadNodeEditor(
    const std::string &filename,
    std::vector<domain::Node> &nodes,
    ImVector<NodeHelpers::LinkInfo> &links,
    int &nextNodeId,
    int &nextLinkId
) {
    // Clear existing nodes and links
    nodes.clear();
    links.clear();
    nextNodeId = 1;

    // Read JSON from file
    std::ifstream file(filename);
    json j = json::parse(file);

    // Load nodes
    for (const auto &nodeObj: j["nodes"]) {
        auto node = std::make_unique<domain::Node>(
            nodeObj["title"].get<std::string>().c_str(),
            static_cast<RobotActions::NodeActivation>(nodeObj["activation"].get<int>())
        );

        // Initialize node IDs properly
        void *idPtr = reinterpret_cast<void *>(nodeObj["id"].get<uintptr_t>());
        ed::NodeId nodeId(idPtr);
        int currentId = static_cast<int>(reinterpret_cast<uintptr_t>(idPtr));
        node->initializeNodeIds(currentId);
        nextNodeId = std::max(nextNodeId, currentId + 1);

        // Set other node properties
        node->setLoopCount(nodeObj["loopCount"].get<int>());
        node->setWaitTimer(nodeObj["waitTimer"].get<int>());
        node->setVelocity(nodeObj["velocity"].get<float>());

        auto pos = new domain::Position(
            {
                nodeObj["position"]["x"].get<float>(),
                nodeObj["position"]["y"].get<float>(),
                nodeObj["position"]["z"].get<float>()
            },
            {0, 0, 0}
        );
        node->setPosition(pos);

        node->setRotationHead({
            nodeObj["rotation"]["x"].get<float>(),
            nodeObj["rotation"]["y"].get<float>(),
            nodeObj["rotation"]["z"].get<float>()
        });

        nodes.push_back(*node);
    }

    // Load links
    for (const auto &linkObj: j["links"]) {
        NodeHelpers::LinkInfo link;
        link.Id = ed::LinkId(reinterpret_cast<void *>(linkObj["id"].get<uintptr_t>()));
        link.InputId = ed::PinId(reinterpret_cast<void *>(linkObj["inputId"].get<uintptr_t>()));
        link.OutputId = ed::PinId(reinterpret_cast<void *>(linkObj["outputId"].get<uintptr_t>()));
        links.push_back(link);

        // Update next link ID
        uintptr_t linkId = linkObj["id"].get<uintptr_t>();
        nextLinkId = std::max(nextLinkId, static_cast<int>(linkId) + 1);
    }
}

void NodeHelpers::renderTooltip(const char *text) {
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", text);
        ImGui::EndTooltip();
    }
}
