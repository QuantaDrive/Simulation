//
// Created by renat on 10-3-2025.
//

#include "HelperFunctions.h"

#include <iostream>

const domain::Node *NodeHelpers::FindStartNode(const std::vector<domain::Node> &nodes,
                                               const ImVector<LinkInfo> &links) {
    // If there's only one node and no links, consider it the start node
    if (nodes.size() == 1 && links.empty()) {
        return &nodes[0];
    }

    // Otherwise, find a node that only has output connections
    for (const auto &node: nodes) {
        if (IsStartNode(node, links)) {
            std::cout << "Starting node: " << node.getTitle() << std::endl;
            return &node;
        }
    }
    return nullptr;
}

const domain::Node *NodeHelpers::FindNextNode(const domain::Node *currentNode, const std::vector<domain::Node> &nodes,
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

bool NodeHelpers::IsStartNode(const domain::Node &node, const ImVector<LinkInfo> &links) {
    // If there are no links at all a single node is considered the start node
    if (links.empty()) {
        return true;
    }
    // Otherwise, node should have no input connections but have output connections
    return !HasInputConnection(node,links) && HasOutputConnection(node, links);
}

bool NodeHelpers::HasInputConnection(const domain::Node &node, const ImVector<LinkInfo> &links) {
    for (const auto &link: links) {
        if (link.OutputId == node.getNodeInputPinId()) {
            return true;
        }
    }
    return false;
}

bool NodeHelpers::HasOutputConnection(const domain::Node &node, const ImVector<LinkInfo> &links) {
    for (const auto &link: links) {
        if (link.InputId == node.getNodeOutputPinId()) {
            return true;
        }
    }
    return false;
}
