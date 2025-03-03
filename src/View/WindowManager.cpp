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
    ImVector<Node> m_Nodes;
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

        // Create a button for all NodeActivation enum values
        for (int i = 0; i < static_cast<int>(RobotActions::NodeActivation::COUNT); ++i) {
            auto action = static_cast<RobotActions::NodeActivation>(i);
            if (ImGui::Button(RobotActions::toString(action).data())) {
                // Create a new node at the next position
                std::string title = RobotActions::toString(action).data();
                Node newNode(title.c_str(), action);
                int currentId = m_NextNodeId;
                newNode.initializeNodeIds(currentId);
                m_NextNodeId = currentId;
                m_Nodes.push_back(newNode);

                // Increment position for next node
                m_NextNodePosition.x += 50;
                m_NextNodePosition.y += 50;
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
                // QueryNewLink returns true if editor want to create new link between pins.
                //
                // Link can be created only for two valid pins, it is up to you to
                // validate if connection make sense. Editor is happy to make any.
                //
                // Link always goes from input to output. User may choose to drag
                // link from output pin or input pin. This determine which pin ids
                // are valid and which are not:
                //   * input valid, output invalid - user started to drag new line from input pin
                //   * input invalid, output valid - user started to drag new line from output pin
                //   * input valid, output valid   - user dragged link over other pin, can be validated

                if (inputPinId && outputPinId) // both are valid, let's accept link
                {
                    // ed::AcceptNewItem() return true when user release mouse button.
                    if (ed::AcceptNewItem()) {
                        // Since we accepted new link, lets add one to our list of links.
                        m_Links.push_back({ed::LinkId(m_NextLinkId++), inputPinId, outputPinId});

                        // Draw new link.
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
            // Add node deletion
            ed::NodeId deletedNodeId;
            while (ed::QueryDeletedNode(&deletedNodeId)) {
                if (ed::AcceptDeletedItem()) {
                    // Here you would remove the node from your data structure
                    for (auto& node : m_Nodes) {
                        if (node.getNodeId() == deletedNodeId) {
                            m_Nodes.erase(&node);
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
