#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model)
{
    // scale the floats to percentages
    start_x *= 0.01f;
    start_y *= 0.01f;
    end_x *= 0.01f;
    end_y *= 0.01f;

    // find the closest nodes to (start_x, start_y)
    start_node = &m_Model.FindClosestNode(start_x, start_y);

    // find the closest nodes to (end_x, end_y)
    end_node = &m_Model.FindClosestNode(end_x, end_y);
}

// this method iterates from the end node to start node and returns the path
// in reverse order. The rendering code expects the path to be in reverse order
std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node)
{
    std::vector<RouteModel::Node> path_found{};
    distance = 0.0f;
    RouteModel::Node parent_node;

    while (current_node->parent != nullptr) {
        path_found.push_back(*current_node);
        parent_node = *(current_node->parent);
        distance += current_node->distance(parent_node);
        current_node = current_node->parent;
    }
    path_found.push_back(*current_node);
    distance *= m_Model.MetricScale();
    return path_found;
}

void RoutePlanner::AStarSearch()
{
    // initialise open_list vector with starting node
    start_node->visited = true;
    open_list.push_back(start_node);
    RouteModel::Node *current_node = nullptr;

    // expand the node till you reach the end node
    // use heuristic to prioritize what node to open
    while (open_list.size() > 0) {
        current_node = NextNode();
        // check if the current node is the end node
        if (current_node->distance(*end_node) == 0) {
            m_Model.path = ConstructFinalPath(current_node);
            return;
        } else {
            AddNeighbors(current_node);
        }
    }
}

// calculate Heuristic value from the current node to end node
float RoutePlanner::CalculateHValue(const RouteModel::Node *node)
{
    return node->distance(*end_node);
}

RouteModel::Node *RoutePlanner::NextNode()
{
    // sort the open list vector in increasing order of f-value
    std::sort(open_list.begin(), open_list.end(), [](const auto &first, const auto &second) {
        return (first->h_value + first->g_value < second->h_value + second->g_value);
    });

    RouteModel::Node *lowest_fvalueNode = open_list.front();

    // remove the first element of the vector as the first
    // element is the node with the lowest f-value
    open_list.erase(open_list.begin());
    return lowest_fvalueNode;
}

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node)
{
    // add all unvisited neighbors to the open list
    current_node->FindNeighbors();

    for (auto neighbor : current_node->neighbors) {
        neighbor->parent = current_node;
        neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);
        neighbor->h_value = CalculateHValue(neighbor);

        // add the neighbor to the open list
        open_list.push_back(neighbor);

        // mark the node as visited
        neighbor->visited = true;
    }
}
