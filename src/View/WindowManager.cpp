#include "IWindowManager.h"
#include <bits/algorithmfwd.h>

#include "../Domain/Node.h"
#include "../Domain/NodeActivation.h"
namespace ed = ax::NodeEditor;

struct LinkInfo
{
    ed::LinkId Id;
    ed::PinId InputId;
    ed::PinId OutputId;
};

class WindowManager : public IWindowManager
{
    GLFWwindow* window = nullptr;
    // Single instruction Window
    int inputX = 0;
    int inputY = 0;
    int inputZ = 0;
    float inputXDegrees = 0;
    float inputYDegrees = 0;
    float inputZDegrees = 0;
    int gripforce = 0;
    std::string textInput;

    // Render Nodes Window
    ImVector<LinkInfo> m_Links;
    std::vector<Node> m_Nodes;
    bool m_FirstFrame = true;
    int m_NextLinkId = 100;
    ImVec2 m_NextNodePosition = ImVec2(0, 0);
    int m_NextNodeId = 1;

    // Node Selector Window


public:
    void SetupImGui(GLFWwindow* existingWindow) override
    {
        window = existingWindow;

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
        {
            std::cerr << "Failed to initialize GLEW\n";
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        std::cout << "Window initialized successfully\n";
    }


    void RenderUI(ed::EditorContext* g_Context) override
    {
        // Start ImGui frame (only once per frame)
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render both UI windows
        RenderSingleInstructionWindow();
        RenderNodeSelectorWindow();
        RenderImGuiNodesEditor(g_Context);

        // Render the final ImGui frame
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

private:
    void RenderNodeSelectorWindow() {
        bool showWindow = true;
        ImGui::Begin("Node Selector", &showWindow);

        // Create a row for each NodeActivation enum value
        for (int i = 0; i < static_cast<int>(RobotActions::NodeActivation::COUNT); ++i) {
            auto action = static_cast<RobotActions::NodeActivation>(i);
            std::string label = RobotActions::toString(action).data();

            // Display plus button, label, and minus button in a row
            ImGui::Text("%s", label.c_str());
            ImGui::SameLine();
            if (ImGui::Button(("+##" + label).c_str())) {
                // Create new node
                Node newNode(label.c_str(), action);
                int currentId = m_NextNodeId;
                newNode.initializeNodeIds(currentId);
                m_NextNodeId = currentId;
                m_Nodes.push_back(newNode);

                m_NextNodePosition.x += 50;
                m_NextNodePosition.y += 50;
            }
            ImGui::SameLine();
            if (ImGui::Button(("-##" + label).c_str())) {
                // Find and remove the last node of this type
                for (auto it = m_Nodes.rbegin(); it != m_Nodes.rend(); ++it) {
                    if (it->getActivation() == action) {
                        m_Nodes.erase((++it).base());
                        break;
                    }
                }
            }
        }
        ImGui::End();
    }

    void RenderSingleInstructionWindow()
    {
        bool showWindow = true;
        ImGui::Begin("Single instruction", &showWindow);
        ImGui::InputInt("X", &inputX);
        ImGui::InputInt("Y", &inputY);
        ImGui::InputInt("Z", &inputZ);
        ImGui::SliderAngle("X Degrees", &inputXDegrees, -360, 360);
        ImGui::SliderAngle("Y Degrees", &inputYDegrees, -360, 360);
        ImGui::SliderAngle("Z Degrees", &inputZDegrees, -360, 360);
        ImGui::SliderInt("Grip Force", &gripforce, 0, 10);

        if (ImGui::Button("Show values"))
        {
            textInput = "X = " + std::to_string(inputX) +
                ", Y = " + std::to_string(inputY) +
                ", Z = " + std::to_string(inputZ) +
                " X Degrees = " + std::to_string(inputXDegrees) +
                " Y Degrees = " + std::to_string(inputYDegrees) +
                " Z Degrees = " + std::to_string(inputZDegrees);
        }
        ImGui::Text(textInput.c_str());
        ImGui::End();
    }


    void CreateNodeInEditor(const char* nodeTitle, ed::NodeId nodeA_Id, ed::PinId nodeA_InputPinId,
                            ed::PinId nodeA_OutputPinId)
    {
        ed::BeginNode(nodeA_Id);
        ImGui::Text(nodeTitle);
        ed::BeginPin(nodeA_InputPinId, ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(nodeA_OutputPinId, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();
        ed::EndNode();
    }

    void RenderImGuiNodesEditor(ed::EditorContext* g_Context)
    {
        ImGui::Begin("Node Editor");
        ed::SetCurrentEditor(g_Context);
        ed::Begin("My Editor");

        // Render all stored nodes
        for (const auto& node : m_Nodes) {
            CreateNodeInEditor(node.getTitle().c_str(), node.getNodeId(),
                             node.getNodeInputPinId(), node.getNodeOutputPinId());
        }
        // Submit Links
        for (auto &linkInfo: m_Links) {
            ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);
        }

        // 2) Handle interactions
        // Handle creation action, returns true if editor want to create new object (node or link)
        if (ed::BeginCreate()) {
            ed::PinId inputPinId, outputPinId;
            if (ed::QueryNewLink(&inputPinId, &outputPinId)) {
                if (inputPinId && outputPinId) {
                    // Validate connection:
                    // 1. Can't connect input to input or output to output
                    // 2. Can't create self-loops
                    bool validateLink = true;

                    // Find source and target nodes
                    Node* sourceNode = nullptr;
                    Node* targetNode = nullptr;
                    for (auto& node : m_Nodes) {
                        if (node.getNodeOutputPinId() == outputPinId ||
                            node.getNodeInputPinId() == inputPinId) {
                            sourceNode = &node;
                            }
                        if (node.getNodeInputPinId() == inputPinId ||
                            node.getNodeOutputPinId() == outputPinId) {
                            targetNode = &node;
                            }
                    }

                    // Validate connection
                    if (sourceNode && targetNode) {
                        // Check for self-loops
                        if (sourceNode == targetNode) {
                            validateLink = false;
                        }

                        // Ensure input connects to output
                        if (outputPinId == sourceNode->getNodeInputPinId() ||
                            inputPinId == targetNode->getNodeOutputPinId()) {
                            validateLink = false;
                            }
                    }

                    if (validateLink && ed::AcceptNewItem()) {
                        // Create new link
                        m_Links.push_back({ed::LinkId(m_NextLinkId++), inputPinId, outputPinId});
                        ed::Link(m_Links.back().Id, m_Links.back().InputId, m_Links.back().OutputId);
                    }
                }
            }
        }
        ed::EndCreate(); // Wraps up object creation action handling.


        // Handle deletion action
        if (ed::BeginDelete()) {
            // There may be many links marked for deletion, let's loop over them.
            ed::LinkId deletedLinkId;
            while (ed::QueryDeletedLink(&deletedLinkId)) {
                // If you agree that link can be deleted, accept deletion.
                if (ed::AcceptDeletedItem()) {
                    // Then remove link from your data.
                    for (auto &link: m_Links) {
                        if (link.Id == deletedLinkId) {
                            m_Links.erase(&link);
                            break;
                        }
                    }
                }
            }
        }
        ed::EndDelete();


        if (m_FirstFrame)
            ed::NavigateToContent(0.0f);

        ed::End();
        ed::SetCurrentEditor(nullptr);
        m_FirstFrame = false;
        ImGui::End();
    }

public:
    void CleanupImGui(ed::EditorContext* g_Context) override
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        ed::DestroyEditor(g_Context);
    }
};
