//
// Created by dante on 2/14/25.
//

#ifndef SIMULATIONMANAGER_H
#define SIMULATIONMANAGER_H

#include "../../Domain/Task.h"

class SimulationManager {
public:
    void executeTask(Task* task);
    void move(ArmPosition* position);
};



#endif //SIMULATIONMANAGER_H
