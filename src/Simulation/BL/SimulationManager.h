//
// Created by dante on 2/14/25.
//

#ifndef SIMULATIONMANAGER_H
#define SIMULATIONMANAGER_H

#include <glm/glm.hpp>


using namespace glm;

class Repo;
class RobotArm;
class Task;
class Position;
class Tool;
class SimulationManager {
private:
    Repo* repo_;
protected:
    mat4 getTransformationMatrix(vec3 position, vec3 rotation);
    void inverseToolFrame(mat4 &toolFrame);
    mat4 toolToArm(const Position* position, const Tool* tool);
    void armToSphericalWrist(mat4& endOfArm);
public:
    explicit SimulationManager(Repo* repo);
    ~SimulationManager();
    void executeTask(const Task* task);
    bool move(RobotArm* arm, Position* position);

    void inverseKinematics(RobotArm* arm, Position* position);
};



#endif //SIMULATIONMANAGER_H
