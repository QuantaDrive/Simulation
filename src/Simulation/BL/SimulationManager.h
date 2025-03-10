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
protected:
    mat4 getTransformationMatrix(vec3 position, vec3 rotation);
    mat4 getDhTransformationMatrix(float joint, float alpha, float d, float a);
    mat4 toolToArm(const domain::Position* position, const domain::Tool* tool);
    mat4 armToSphericalWrist(const mat4& j6);
    vector<vector<float>> getParamsJ1Zero(mat4& sphericalWrist);
    vector<domain::Position*> interpolate(const domain::Position* currentPosition, domain::Position* newPosition);
public:
    explicit SimulationManager(Repo* repo, simulation::RobotArm* simulationArm);
    ~SimulationManager();
    void executeTask(const domain::Task* task);
    void executeInstruction(const domain::Instruction* instruction);
    bool move(domain::Position* position);
    void grip(float gripForce);

    vector<float> inverseKinematics(domain::Position* position);
};



#endif //SIMULATIONMANAGER_H
