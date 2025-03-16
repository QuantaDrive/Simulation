//
// Created by dante on 2/14/25.
//

#ifndef SIMULATIONMANAGER_H
#define SIMULATIONMANAGER_H

#include <memory>
#include <glm/glm.hpp>
#include <vector>

#include "../src/Visualization.h"
#include "../src/RobotArm.h"

namespace simulation
{
    class RobotArm;

}

using namespace glm;
using namespace std;

namespace domain
{
    class Instruction;
    class RobotArm;
    class Task;
    class Position;
    class Tool;
}

class Repo;

class SimulationManager {
private:
    Repo* repo_;
    simulation::RobotArm* simulationArm_;
    domain::RobotArm* robotArm_;
    // Camera control state
    glm::vec3 m_CameraTarget = glm::vec3(0, 750/2, 0);
    glm::vec3 m_CameraPosition = glm::vec3(0, 750/2, 1000);
    float m_CameraDistance = 1000.0f;
    // Add horizontal rotation angle
    float m_Yaw = 0.0f;
    // Add vertical rotation angle
    float m_Pitch = 0.0f;
    // Add field of view
    double m_LastX = 0.0;
    double m_LastY = 0.0;
    bool m_FirstMouse = true;
    bool m_DragActive = false;

protected:
    mat4 getTransformationMatrix(vec3 position, vec3 rotation);
    mat4 getDhTransformationMatrix(float joint, float alpha, float d, float a);
    mat4 toolToArm(const domain::Position* position, const domain::Tool* tool);
    mat4 armToSphericalWrist(const mat4& j6);
    vector<vector<float>> getParamsJ1Zero(mat4& sphericalWrist);
    vector<domain::Position*> interpolate(const domain::Position* currentPosition, domain::Position* newPosition);
    bool checkMove(pair<int,float> joint, float time, bool relative);
    float calcETA(pair<int,float> joint, bool relative);
public:
    explicit SimulationManager(Repo* repo, simulation::RobotArm* simulationArm);
    ~SimulationManager();
    void setRobotArm(domain::RobotArm* robotArm);
    [[nodiscard]] domain::RobotArm* getRobotArm();
    void executeTask(const domain::Task* task);
    void executeInstruction(const domain::Instruction* instruction);
    bool move(domain::Position* position, float velocity, bool relative=false);
    void grip(float gripForce);

    vector<float> inverseKinematics(domain::Position* position);
    // camera handling
    void initializeCamera();
    void handleMouseDrag(double xpos, double ypos);
    void handleMouseScroll(double yoffset);
    void setDragActive(bool active);
private:
    void updateCameraPosition();
};



#endif //SIMULATIONMANAGER_H
