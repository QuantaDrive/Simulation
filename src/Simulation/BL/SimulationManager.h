//
// Created by dante on 2/14/25.
//

#ifndef SIMULATIONMANAGER_H
#define SIMULATIONMANAGER_H

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
protected:
    mat4 getTransformationMatrix(vec3 position, vec3 rotation);
    mat4 getDhTransformationMatrix(float joint, float alpha, float d, float a);
    void inverseToolFrame(mat4 &toolFrame);
    mat4 toolToArm(const domain::Position* position, const domain::Tool* tool);
    mat4 armToSphericalWrist(mat4 j6);
    vector<vector<float>> getParamsJ1Zero(float j1, mat4& sphericalWrist);
    float findJ3(float j1, float j2);
public:
    explicit SimulationManager(Repo* repo, simulation::RobotArm* simulationArm);
    ~SimulationManager();
    void executeTask(const domain::Task* task);
    bool move(domain::RobotArm* arm, domain::Position* position);

    vector<float> inverseKinematics(domain::RobotArm* arm, domain::Position* position);
};



#endif //SIMULATIONMANAGER_H
