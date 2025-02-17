//
// Created by dante on 2/14/25.
//

#ifndef SIMULATIONMANAGER_H
#define SIMULATIONMANAGER_H

#include "../../DAL/Repo.h"
#include "../../Domain/RobotArm.h"
#include "../../Domain/Task.h"

class SimulationManager {
private:
    Repo* repo_;
public:
    SimulationManager(Repo* repo);
    ~SimulationManager();
    void executeTask(const Task* task);
    bool move(RobotArm* arm, ArmPosition* position);
};



#endif //SIMULATIONMANAGER_H
